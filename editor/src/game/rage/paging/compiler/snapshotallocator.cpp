#include "snapshotallocator.h"

#include "helpers/ranges.h"
#include "rage/paging/paging.h"


#include "rage/base/tlsContext.h"

u32 rage::pgSnapshotAllocator::Node::CreateGuard() const
{
	// 'reinterpret_cast': truncation
#pragma warning( push )
#pragma warning( disable : 4311)
#pragma warning( disable : 4302)
	return reinterpret_cast<u32>(this); // NOLINT(clang-diagnostic-pointer-to-int-cast)
#pragma warning( pop ) 
}

bool rage::pgSnapshotAllocator::Node::VerifyGuard() const
{
	return Guard == CreateGuard();
}

void rage::pgSnapshotAllocator::Node::AssertGuard() const
{
	if (!VerifyGuard())
	{
		LogInfo("SnapshotAllocator::Node::AssertGuard() -> Guard was trashed");
		exit(0);
	}
}

rage::pgSnapshotAllocator::Node::Node(u32 size)
{
	Guard = CreateGuard();
	Size = size;
}

rage::pgSnapshotAllocator::Node::~Node()
{
}

void rage::pgSnapshotAllocator::Node::FixupRefs(u64 newAddress) const
{
	//AM_DEBUGF("SnapshotAllocator::Node::FixupRefs() -> %u refs from: %#llx to: %#llx",
	//	Refs.GetSize(), (u64)GetBlock(), newAddress);

	for (size_t i = 0; i < Refs.size(); i++)
	{
		(*Refs[i]) = reinterpret_cast<void*>(newAddress);
	}

	for (size_t i = 0; i < OffsetRefs.size(); i++)
	{
		(*OffsetRefs[i].Ref) = reinterpret_cast<void*>(newAddress + OffsetRefs[i].Offset);
	}
}

char* rage::pgSnapshotAllocator::Node::GetBlock() const
{
	return (char*)this + sizeof(Node); // NOLINT(clang-diagnostic-cast-qual)
}

rage::pgSnapshotAllocator::Node* rage::pgSnapshotAllocator::Node::GetNext() const
{
	Node* next = (Node*)((u64)GetBlock() + Size);
	if (next->VerifyGuard())
		return next;
	return nullptr;
}

rage::pgSnapshotAllocator::Node* rage::pgSnapshotAllocator::GetNodeFromBlock(pVoid block) const
{
	Node* node = (Node*)((u64)block - sizeof(Node));
	if (node->VerifyGuard())
		return node;
	return nullptr;
}

rage::pgSnapshotAllocator::Node* rage::pgSnapshotAllocator::FindBlockThatContainsPointer(pVoid ptr) const
{
	Node* node = GetRootNode();
	while (node)
	{
		if (IS_WITHIN(ptr, node->GetBlock(), node->Size))
			return node;
		node = node->GetNext();
	}
	return nullptr;
}

rage::pgSnapshotAllocator::Node* rage::pgSnapshotAllocator::GetNodeFromBlockIndex(u32 index) const
{
	u32 i = 0;
	Node* node = GetRootNode();
	while (node)
	{
		if (i++ == index)
			return node;
		node = node->GetNext();
	}
	//AM_ASSERT(false, "SnapshotAllocator::GetNodeFromBlockIndex() -> Block index %i is not valid.", index);
	return nullptr; // Assert won't return
}

rage::pgSnapshotAllocator::Node* rage::pgSnapshotAllocator::GetRootNode() const
{
	Node* node = (Node*)m_Heap;
	if (node->VerifyGuard())
		return node;
	return nullptr;
}

void rage::pgSnapshotAllocator::SanityCheck() const
{
#ifdef DEBUG
	if (m_NodeCount > 0)
	{
		u32 nodeCount = 0;
		Node* node = (Node*)m_Heap;
		while (node)
		{
			node->AssertGuard();

			nodeCount++;
			node = node->GetNext();
		}
		AM_ASSERT(nodeCount == m_NodeCount, "pgSnapshotAllocator::SanityCheck() -> Node count mismatch.");
	}
#endif
}

rage::pgSnapshotAllocator::pgSnapshotAllocator(u32 size, bool isVirtual)
{
	m_HeapSize = size;

	m_Heap = rage::tlsContext::get()->m_allocator->Allocate(size);
	//m_Heap = GetMultiAllocator()->Allocate(size);
	m_IsVirtual = isVirtual;

	// Any trash in memory will make packing and compression worse
	memset(m_Heap, 0, size);
}

rage::pgSnapshotAllocator::~pgSnapshotAllocator()
{
	// We have to destruct nodes manually because they were constructed via new placement
	Node* node = GetRootNode();
	while (node)
	{
		Node* toDestruct = node;
		node = node->GetNext();
		toDestruct->~Node();
	}

	rage::tlsContext::get()->m_allocator->Free(m_Heap);
	m_Heap = nullptr;
}

pVoid rage::pgSnapshotAllocator::Allocate(u32 size)
{
	SanityCheck();

	if (size == 0)
		LogInfo("pgSnapshotAllocator::Allocate() -> Request size is zero");
	
	size = MAX(size, 16);
	size = ALIGN_16(size);

	pVoid block = (pVoid)((u64)m_Heap + m_Offset);
	Node* header = new (block) Node(size);

	m_Offset += size + sizeof(Node);
	m_NodeCount++;

	if (m_Offset > m_HeapSize)
		LogInfo("SnapshotAllocator::Allocate() -> Out of memory.");

	return header->GetBlock();
}

void rage::pgSnapshotAllocator::GetBlockSizes(atArray<u32>& outSizes) const
{
	Node* node = GetRootNode();
	while (node)
	{
		outSizes.push_back(node->Size);
		node = node->GetNext();
	}
}

void rage::pgSnapshotAllocator::FixupBlockReferences(u16 blockIndex, u64 newAddress) const
{
	GetNodeFromBlockIndex(blockIndex)->FixupRefs(newAddress);
}

u64 rage::pgSnapshotAllocator::GetBaseAddress() const
{
	return IsVirtual() ? PG_VIRTUAL_MASK : PG_PHYSICAL_MASK;
}

pVoid rage::pgSnapshotAllocator::GetBlock(u16 index) const
{
	return GetNodeFromBlockIndex(index)->GetBlock();
}

u32 rage::pgSnapshotAllocator::GetBlockSize(u16 index) const
{
	return GetNodeFromBlockIndex(index)->Size;
}

bool rage::pgSnapshotAllocator::IsVirtual() const
{
	return m_IsVirtual;
}
