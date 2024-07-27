#pragma once

#include "common/types.h"
#include "rage/base/atRTTI.h"

namespace rage
{
	struct datResourceMap;

	/**
	 * \brief Base class for every paged resource.
	 * \n For more details on how this system works, see paging.h;
	 */
	class pgBase
	{
		DEFINE_RAGE_RTTI(pgBase);

		/**
		 * \brief Mini copy of datResourceMap.
		 * \n Purpose: We have to store allocated chunks to de-allocate them when object is destroyed.
		 * \n Full datResourceMap contains redundant information for deallocation so this is a 'mini' version of it.
		 */
		struct Map
		{
			static constexpr u64 MAP_SIZE_SHIFT_MASK = 0xF;				// Lowest 8 bits
			static constexpr u64 MAP_ADDRESS_MASK = ~0xF;				// Highest 56 bits

			u32* MetaData;												// According to Companion. Purpose is unknown.

			u8 VirtualChunkCount;
			u8 PhysicalChunkCount;
			u8 MainChunkIndex;
			bool bIsDynamic;

			u64 AddressAndShift[128];								// In comparison with datResource, contains only destination (allocated) addresses.

			bool IsCompiled() const;								// Whether this map was built from compiled resource.
			void GenerateFromMap(const datResourceMap& map);		// Generates mini map from original one.
			void RegenerateMap(datResourceMap& map) const;			// Recreates resource map from this mini map.
		};

		Map* m_Map;

		void MakeDefragmentable(const datResourceMap& map) const;	// Builds mini map (consumes less memory) // We need this map to destruct and defragment (done via re-allocating resource) paged object
		void FreeMemory(const datResourceMap& map) const;			// Performs de-allocation of all memory chunks that are owned by this resource.
																	// NOTE: This includes main chunk or 'this'; Which means: after this function is invoked,
																	// accessing 'this' will cause undefined behaviour.
	public:													
		
		pgBase();
		virtual ~pgBase();

		pgBase(const pgBase& other) = delete;	//Todo				// Compiler constructor.
		void Destroy() const;										// De-allocates resource memory.
		bool HasMap() const;										// Whether this paged object has internal resource map,	// which means that this resource is compiled and was built from file.
		void RegenerateMap(datResourceMap& map) const;				// Recreates resource map of this resource. If resource has no map, nothing is done.

#if APP_BUILD_2699_16_RELEASE_NO_OPT							// Unused in final version (overriden only once in grcTexture)
		virtual ConstString GetDebugName() { return ""; }		// They're present only for virtual table completeness.
#endif
		virtual u32 GetHandleIndex() const { return 0; }
		virtual void SetHandleIndex(u32 index) { }
	};


	class pgBaseRefCounted : public pgBase
	{
	public:
		u32 m_RefCount;
	};
}

