#pragma once

#include "paging.h"
#include "resourcemap.h"
#include "common/types.h"


namespace rage
{
	class pgBase;
	struct datResource;

	/**
	 * \brief Holds data for deserializing resource.
	 */
	struct datResource
	{
		datResourceMap* Map;
		datResource* PreviousResource;

		ConstString				DebugName;
		datResourceSortedChunk	SrcChunks[PG_MAX_CHUNKS];	// Chunks from file
		datResourceSortedChunk	DestChunks[PG_MAX_CHUNKS];	// Chunks allocated on game heap
		u8						ChunkCount;					// Virtual + Physical

	private:
		static inline thread_local datResource* tl_CurrentResource = nullptr;
	public:
		datResource(datResourceMap& map, ConstString name = "<unknown>");
		~datResource();

		static datResource* GetCurrent() { return tl_CurrentResource; }						// Gets current active resource in this thread, if there's any.
		static bool	IsBuilding() { return GetCurrent() != nullptr; }						// Whether resource is building in current thread or not.
		datResourceChunk* GetChunk(const datResourceSortedChunk* sortedChunk) const;		// Gets corresponding resource chunk from it's sorted version.
		const datResourceSortedChunk* Find(const datResourceSortedChunk* chunks, u64 address) const;		// Searches for chunk that contains given address.
		bool ContainsThisAddress(u64 address) const;										// Checks whether given heap address belongs to this resource.
		u64	GetFixup(u64 resourceOffset) const;												// Gets offset for given resource offset to map it into corresponding address in game heap.


		
		template<typename T>
		void Fixup(T& obj) const		// Automatically adds offset to passed pointer reference
		{
			if (!obj)
				return;

			obj = (T)((u64)obj + GetFixup((u64)obj));
		}

		
		template<typename T>
		void FixupAndPlace(T*& obj) const		// Fixes up and calls place function
		{
			if (!obj)
				return;

			Fixup(obj);
			Place(obj);
		}

		template<typename T>
		void Place(T* obj) const							// Calls T::Place function if defined, otherwise ::PlaceDefault.
		{
			constexpr bool hasPlaceFn = requires(T t, datResource rsc)
			{
				T::Place(rsc, &t);
			};

			// We allow resources to implement their own place function,
			// example of how it is used can be seen in phBound::Place factory
			if constexpr (hasPlaceFn)
			{
				T::Place(*this, obj);
			}
			else
			{
				PlaceDefault(obj);
			}
		}

		template<typename T>
		void PlaceDefault(T* obj) const					// Default place function that just calls resource constructor
		{
			// Key thing to note here is that we're new-placing struct on de-serialized memory from resource file
			const datResource& rsc = *this;
			void* addr = (void*)obj;
			new (addr) T(rsc);
		}

		// Automatically decides whether to just fixup pointer or also perform placement (if datResource constructor exists)
		template<typename T>
		void operator>>(T*& obj) const
		{
			constexpr bool hasResourceConstructor = requires(T t, datResource rsc)
			{
				new (&t) T(rsc); // Check if T(const datResource& rsc) constructor exists
			};

			constexpr bool hasPlaceFn = requires(T t, datResource rsc)
			{
				T::Place(rsc, &t);
			};

			if constexpr (hasResourceConstructor || hasPlaceFn)
			{
				FixupAndPlace(obj);
			}
			else
			{
				Fixup(obj);
			}
		}
	};

	template<typename T>
	concept pgCanDeserialize = requires(T t, datResource r) { r >> t; };

	template<typename T>
	constexpr bool pgHasRscConstructor = requires(T t, datResource rsc)
	{
		new (&t) T(rsc); // Check if T(const datResource& rsc) constructor exists
	};
}



