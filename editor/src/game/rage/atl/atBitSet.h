#pragma once

#include <format>
#include "common/types.h"
#include "atArray.h"
#include "helpers/align.h"


class atBitSet
{
public:
	u32* m_bits = nullptr;
private:
	u16 m_capacity = 0;
	u16 m_size = 0;
public:

	atBitSet(u16 bits_count)
	{
		u16 alloc_sz = ALIGN_32(bits_count) / sizeof(u32) / 2;
		m_bits = static_cast<u32*>(alloc_fn(alloc_sz));
		m_size = bits_count;
		m_capacity = ALIGN_32(bits_count);
		memset(m_bits, 0, alloc_sz);
	}
	
	~atBitSet()
	{
		dealloc_fn(m_bits);
		m_bits = nullptr;
		m_capacity = 0;
		m_size = 0;
	}

	atBitSet(const atBitSet& other)
		: m_capacity(other.m_capacity)
		, m_size(other.m_size)
	{
		u16 size = ALIGN_32(other.m_capacity) / sizeof(u32) / 2;
		m_bits = static_cast<u32*>(alloc_fn(size));
		memcpy(m_bits, other.m_bits, size);
	}

	void operator= (u32 value)
	{
		if (m_bits)
			*m_bits = value;
	}

	u16 capacity() const { return m_capacity; }
	u16 size() const { return m_size; }
};
