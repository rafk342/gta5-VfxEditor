#include "resourcechunk.h"

pVoid rage::datResourceChunk::GetAllocatedAddress() const
{
	return reinterpret_cast<pVoid>(DestAddr);
}

u64 rage::datResourceChunk::GetFixup() const
{
	return DestAddr - SrcAddr;
}

u8 rage::datResourceChunk::GetSizeShift() const
{
	return datResourceInfo::GetChunkSizeShift(static_cast<u32>(Size));
}

rage::datResourceSortedChunk::datResourceSortedChunk(u64 address, u64 size, u8 index)
{
	Address = address;
	IndexAndSize = static_cast<u64>(index) << SORTED_CHUNK_INDEX_SHIFT | size;
}

u8 rage::datResourceSortedChunk::GetChunkIndex() const
{
	return static_cast<u8>(IndexAndSize >> SORTED_CHUNK_INDEX_SHIFT & SORTED_CHUNK_INDEX_SIZE);
}

u64 rage::datResourceSortedChunk::GetSize() const
{
	return IndexAndSize & SORTED_CHUNK_SIZE_MASK;
}

u64 rage::datResourceSortedChunk::GetEndAddress() const
{
	return Address + GetSize();
}

u64 rage::datResourceSortedChunk::ContainsThisAddress(u64 addr) const
{
	return addr >= Address && addr < GetEndAddress();
}