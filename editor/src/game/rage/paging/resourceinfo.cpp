#include "resourceinfo.h"

#include "resourcechunk.h"
#include "resourcemap.h"
//#include "compiler/packer.h"

u8 rage::datResourceInfo::GetMaxChunkCountInBucket(u32 bucket)
{
	static constexpr u8 chunkMaxCounts[] = { 1, 3, 15, 63, 127, 1, 1, 1, 1 };
	return chunkMaxCounts[bucket];
}


u8 rage::datResourceInfo::GetSizeShift(u32 data)
{
	return (data & 0xF) + PG_MIN_SIZE_SHIFT;
}

u8 rage::datResourceInfo::GetChunkCount(u32 data)
{
	return (u8)(
		(data >> 4 & 0x1) +
		(data >> 5 & 0x3) +
		(data >> 7 & 0xF) +
		(data >> 11 & 0x3F) +
		(data >> 17 & 0x7F) +
		(data >> 24 & 0x1) +
		(data >> 25 & 0x1) +
		(data >> 26 & 0x1) +
		(data >> 27 & 0x1));
}

u8 rage::datResourceInfo::GetVirtualChunkCount() const
{
	return GetChunkCount(VirtualData);
}

u8 rage::datResourceInfo::GetPhysicalChunkCount() const
{
	return GetChunkCount(PhysicalData);
}

u32 rage::datResourceInfo::EncodeChunks(const datPackedChunks& pack) ////////////////////////////////////////////////////////////////////////////////////////
{
	// Bit layout is described in header
	
	u32 data = 0;
	//data |= pack.SizeShift - PG_MIN_SIZE_SHIFT & 0xF; 
	//data |= (pack.BucketCounts[0] & 0x1) << 4;
	//data |= (pack.BucketCounts[1] & 0x3) << 5;
	//data |= (pack.BucketCounts[2] & 0xF) << 7;
	//data |= (pack.BucketCounts[3] & 0x3F) << 11;
	//data |= (pack.BucketCounts[4] & 0x7F) << 17;
	//data |= (pack.BucketCounts[5] & 0x1) << 24;
	//data |= (pack.BucketCounts[6] & 0x1) << 25;
	//data |= (pack.BucketCounts[7] & 0x1) << 26;
	//data |= (pack.BucketCounts[8] & 0x1) << 27;

	return data;
}

u8 rage::datResourceInfo::GenerateChunks(u32 data, u64 baseSize, datResourceMap& map, u8 startChunk, u64 baseAddr)
{
	u32 chunkCounts[PG_MAX_BUCKETS]
	{
		data >> 4 & 0x1,
		data >> 5 & 0x3,
		data >> 7 & 0xF,
		data >> 11 & 0x3F,
		data >> 17 & 0x7F,
		data >> 24 & 0x1,
		data >> 25 & 0x1,
		data >> 26 & 0x1,
		data >> 27 & 0x1,
	};

	u64 chunkSize = baseSize << GetSizeShift(data);
	for (u32 count : chunkCounts)
	{
		for (u32 i = 0; i < count; i++)
		{
			map.Chunks[startChunk] = datResourceChunk(baseAddr, 0, chunkSize);
			baseAddr += chunkSize;
			startChunk++;
		}
		chunkSize >>= 1; // Chunks are ordered descending by size
	}
	return startChunk;
}

void rage::datResourceInfo::GenerateMap(datResourceMap& map) const
{
	map.VirtualChunkCount = GetVirtualChunkCount();
	map.PhysicalChunkCount = GetPhysicalChunkCount();

	map.MainChunkIndex = 0;
	map.MainChunk = nullptr;

	u8 vChunkCount = GenerateChunks(VirtualData, PG_MIN_CHUNK_SIZE, map, 0, PG_VIRTUAL_MASK);
	u8 pChunkCount = GenerateChunks(PhysicalData, PG_MIN_CHUNK_SIZE, map, vChunkCount, PG_PHYSICAL_MASK);
}

u32 rage::datResourceInfo::ComputeSize(u32 data) const
{
	u32 chunkCounts[PG_MAX_BUCKETS]
	{
		data >> 4 & 0x1,
		data >> 5 & 0x3,
		data >> 7 & 0xF,
		data >> 11 & 0x3F,
		data >> 17 & 0x7F,
		data >> 24 & 0x1,
		data >> 25 & 0x1,
		data >> 26 & 0x1,
		data >> 27 & 0x1,
	};

	u32 totalSize = 0;
	u64 chunkSize = PG_MIN_CHUNK_SIZE << GetSizeShift(data);
	for (u32 count : chunkCounts)
	{
		for (u32 i = 0; i < count; i++)
			totalSize += chunkSize;
		chunkSize >>= 1; // Chunks are ordered descending by size
	}
	return totalSize;
}


u32 rage::datResourceInfo::GetChunkSize(u32 blockSize)
{
	if (blockSize == 0)
		return 0;

	if (blockSize < PG_MIN_CHUNK_SIZE)
		return PG_MIN_CHUNK_SIZE;

	return ALIGN_POWER_OF_TWO_64(blockSize);
}


u8 rage::datResourceInfo::GetChunkSizeShift(u32 chunkSize)
{
	return BitScanR64(chunkSize) - BitScanR64(PG_MIN_CHUNK_SIZE);
}


void rage::datResourceInfo::SetVersion(int version)
{
	VirtualData &= ~VERSION_MASK;
	PhysicalData &= ~VERSION_MASK;
	VirtualData |= (version >> 4 & 0xF) << VERSION_SHIFT; // Bits 4-8 in virtual
	PhysicalData |= (version & 0xF) << VERSION_SHIFT; // Bits 0-4 in physical
}

int rage::datResourceInfo::GetVersion() const
{
	return (int)(((VirtualData >> VERSION_SHIFT) & 0xF) << 4) | ((PhysicalData >> VERSION_SHIFT) & 0xF);
}