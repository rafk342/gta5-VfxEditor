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
	u16 m_BitWordsCount = 0; // size in 32-bit words    ; 32 -> 1; 64 -> 2
	u16 m_BitsCount = 0;     // size in bits
public:

	u16 GetNumBitWords() const { return m_BitWordsCount; }
	u16 size() const { return m_BitsCount; }

	atBitSet(u16 bits_count)
	{
		if (bits_count == 0)
			return;
		
		m_BitsCount = ALIGN_32(bits_count);
		m_BitWordsCount = m_BitsCount / (sizeof(u32) * 8);

		u16 alloc_sz = m_BitWordsCount * sizeof(u32);
		m_bits = static_cast<u32*>(alloc_fn(alloc_sz));
		memset(m_bits, 0, alloc_sz);
	}
	
	atBitSet(const atBitSet& other)
		: m_BitWordsCount(other.m_BitWordsCount)
		, m_BitsCount(other.m_BitsCount)
	{
		if (m_bits)
		{
			dealloc_fn(m_bits);
			m_bits = nullptr;
		}
		if (other.m_bits)
		{
			u16 sz = other.m_BitWordsCount * sizeof(u32);
			m_bits = static_cast<u32*>(alloc_fn(sz));
			memcpy(m_bits, other.m_bits, sz);
		}
	}

	atBitSet(atBitSet&& other) noexcept
	{
		std::swap(m_BitWordsCount, other.m_BitWordsCount);
		std::swap(m_BitsCount, other.m_BitsCount);
		std::swap(m_bits, other.m_bits);
	}
	
	atBitSet& operator=(atBitSet&& other) noexcept
	{
		if (this == &other)
			return *this;
		
		std::swap(m_BitWordsCount, other.m_BitWordsCount);
		std::swap(m_BitsCount, other.m_BitsCount);
		std::swap(m_bits, other.m_bits);

		return *this;
	}
	
	atBitSet& operator=(const atBitSet& other)
	{
		if (this == &other)
			return *this;

		m_BitWordsCount = other.m_BitWordsCount;
		m_BitsCount = other.m_BitsCount;

		if (m_bits)
		{
			dealloc_fn(m_bits);
			m_bits = nullptr;
		}
		if (other.m_bits)
		{
			u16 sz = other.m_BitWordsCount * sizeof(u32);
			m_bits = static_cast<u32*>(alloc_fn(sz));
			memcpy(m_bits, other.m_bits, sz);
		}
		return *this;
	}

	void operator= (u32 value)
	{
		if (m_bits)
			*m_bits = value;
	}

	~atBitSet()
	{
		if (m_bits)
		{
			dealloc_fn(m_bits);
			m_bits = nullptr;
			m_BitWordsCount = 0;
			m_BitsCount = 0;
		}
	}

};
