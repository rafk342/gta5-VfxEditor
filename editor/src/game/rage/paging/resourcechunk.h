#pragma once
#include "common/types.h"
#include "resourceinfo.h"



namespace rage
{
	/**
	 *  Represents mapping between resource (file) offset and game memory address.
	 */
	struct datResourceChunk
	{
		u64 SrcAddr;								// Address of chunk in file
		u64 DestAddr;								// Address of chunk allocated in heap
		u64 Size;									// Even though it's defined as 64 bits, maximum allowed size (100MB) won't exceed 32 bits.
		
		pVoid GetAllocatedAddress() const;
		u64 GetFixup() const;						// Gets offset that maps resource address to corresponding address allocated in game heap.
		u8 GetSizeShift() const;
	};

	/**
	 *  Same as datResourceChunk but used in sorted list for quick binary search.
	 */
	struct datResourceSortedChunk
	{
		static constexpr u64 SORTED_CHUNK_SIZE_MASK = 0x00FF'FFFF'FFFF'FFFF;
		static constexpr u64 SORTED_CHUNK_INDEX_SHIFT = 56;	// 8 highest bits
		static constexpr u64 SORTED_CHUNK_INDEX_SIZE = 0xFF;

		u64 Address = 0;											// Start address of this chunk
		u64 IndexAndSize = 0;

		datResourceSortedChunk() = default;
		datResourceSortedChunk(u64 address, u64 size, u8 index);

		u8 GetChunkIndex() const;									// Corresponding chunk in datResourceMap.Chunks
		u64 ContainsThisAddress(u64 addr) const;					// Gets whether given address is within address range of this chunk.
		u64 GetSize() const;
		u64 GetEndAddress() const;
	};
}
