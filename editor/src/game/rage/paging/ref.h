#pragma once
#include "common/types.h"
#include "resource.h"



// datRef -> resource::fixup
// datOwner -> resource::place

namespace rage
{
	template<typename T>
	class datOwner
	{
	protected:
		T* m_Pointer;

	public:
	
		datOwner()												// Automatic placing if in resource constructor
		{
			datResource* rsc = datResource::GetCurrent();
			if (rsc)
				*rsc >> m_Pointer;
			else
				m_Pointer = nullptr;
		}

		datOwner(T* ptr) { m_Pointer = ptr; }									// Initialize with pointer // Don't increment on initialization because rage uses default ref count 
		datOwner(const datResource& rsc) { rsc >> m_Pointer; }					// ReSharper disable once CppPossiblyUninitializedMember
		datOwner(datOwner&& other) noexcept { std::swap(m_Pointer, other.m_Pointer); };


		// Don't use! Reserved for resource compiler.
		// This makes identical smart pointer copy without accounting refs.
		void Snapshot(const datOwner& other, bool physical = false)
		{
			//pgSnapshotAllocator* allocator =
			//	physical ? pgRscCompiler::GetPhysicalAllocator() : pgRscCompiler::GetVirtualAllocator();

			//// We use snapshot function to resolve derived classes
			//constexpr bool hasSnapshotFn = requires(T t)
			//{
			//	T::Snapshot(allocator, &t);
			//};

			//if constexpr (hasSnapshotFn)
			//{
			//	m_Pointer = static_cast<T*>(T::Snapshot(allocator, other.m_Pointer));
			//	allocator->AddRef(m_Pointer);
			//}
			//else if constexpr (std::is_abstract_v<T>)
			//{
			//	AM_UNREACHABLE("Cannot be used on abstract classes");
			//}
			//else
			//{
			//	if (other.m_Pointer != nullptr)
			//	{
			//		allocator->AllocateRef(m_Pointer);
			//		new (m_Pointer) T(*other.m_Pointer);
			//	}
			//}
		}

		// Adds ref to internal pointer if compiling, otherwise doing nothing
		void AddCompilerRef(bool physical = false)
		{
			//pgRscCompiler* compiler = pgRscCompiler::GetCurrent();
			//if (!compiler)
			//	return;
			//if (physical)
			//	pgRscCompiler::GetPhysicalAllocator()->AddRef(this->m_Pointer);
			//else
			//	pgRscCompiler::GetVirtualAllocator()->AddRef(this->m_Pointer);
		}

		// Sets pointer value without deleting previous pointer
		// Used in rare cases... mostly to deal with legacy rage code
		// NOTE: Pointer has to be reset to old one before this smart pointer goes out of scope!
		void Set(T* ptr) { m_Pointer = ptr; }

		T* Get() const { return m_Pointer; }
		T& GetRef() { return *m_Pointer; }
		const T& GetRef() const { return *m_Pointer; }

		bool operator==(const T* ptr) const { return m_Pointer == ptr; }
		bool operator==(const datOwner& other) const { return m_Pointer == other.m_Pointer; }

		T* operator->() const { return m_Pointer; }
		T& operator*() { return *m_Pointer; }
		const T& operator*() const { return *m_Pointer; }

		// Implicit bool cast operator to make if(ptr) construction work
		operator bool() const { return m_Pointer != nullptr; }
		bool IsNull() const { return m_Pointer == nullptr; }

		// Used by 'weak' pointers that share single pointer with something else,
		// but still use fixup code
		void SuppressDelete(){ m_Pointer = nullptr; }
	};

	template <class T>
	struct datRef
	{
		T* m_Pointer;
		T& operator*() const { return (*m_Pointer); }
		T* operator->() const { return m_Pointer; }
		T*& operator=(T* _ptr) { return m_Pointer = _ptr; }

		T* Get() const { return m_Pointer; }
		T& GetRef() { return (*m_Pointer); }

		operator T*() { return m_Pointer; }
		operator bool() const { return m_Pointer != nullptr; }
	};
	
	template <class T>
	struct pgRef
	{
		T* m_Pointer;
		T& operator*() const { return (*m_Pointer); }
		T* operator->() const { return m_Pointer; }
		T*& operator=(T* _ptr) { return m_Pointer = _ptr; }

		T* Get() const { return m_Pointer; }
		T& GetRef() { return (*m_Pointer); }

		operator T* () { return m_Pointer; }
		operator bool() const { return m_Pointer != nullptr; }
	};

}