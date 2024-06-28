#pragma once

#include <format>
#include "common/types.h"
#include "atArray.h"
#include "helpers/align.h"


class atBitSet
{
	u32* m_bits = nullptr;
	u16 m_BitWordsCount = 0; // size in 32-bit words
	u16 m_BitsCount = 0;     // size in bits
public:

	u16 GetNumBitWords() const { return m_BitWordsCount; }
	u16 size() const { return m_BitsCount; }
	u32* data() { return m_bits; }

	atBitSet(u16 bits_count)
	{
		if (bits_count == 0)
			return;

		m_BitsCount = bits_count;
		m_BitWordsCount = ALIGN_32(bits_count) / (sizeof(u32) * 8); // 64 / 32 = 2 (u32 nums)
		
		u16 alloc_sz = m_BitWordsCount * sizeof(u32); // 2 * 4 = 8 bytes
		m_bits = static_cast<u32*>(alloc_fn(alloc_sz));
		memset(m_bits, 0, alloc_sz);
	}

	atBitSet(const atBitSet& other)
	{
		if (m_bits)
		{
			dealloc_fn(m_bits);
			m_bits = nullptr;
			m_BitWordsCount = 0;
			m_BitsCount = 0;
		}
		if (other.m_bits)
		{
			m_BitWordsCount = other.m_BitWordsCount;
			m_BitsCount = other.m_BitsCount;

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

		if (m_bits)
		{
			dealloc_fn(m_bits);
			m_bits = nullptr;
			m_BitWordsCount = 0;
			m_BitsCount = 0;
		}
		if (other.m_bits)
		{
			m_BitWordsCount = other.m_BitWordsCount;
			m_BitsCount = other.m_BitsCount;

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

	void reset()
	{
		memset(m_bits, 0, m_BitWordsCount * sizeof(u32));
	}
	
	bool test(u16 bit)
	{
		if (bit >= m_BitsCount) 
			return false;

		u16 wordIndex = bit / 32;
		u16 bitIndex = bit % 32;
		return (m_bits[wordIndex] & (1 << bitIndex)) != 0;
	}

	void set(u16 bit, bool value = true)
	{
		if (bit >= m_BitsCount)
			return;

		u16 wordIndex = bit / 32;
		u16 bitIndex = bit % 32;
		u32 mask = 1 << bitIndex;
		m_bits[wordIndex] &= (~mask);
		if (value)
			m_bits[wordIndex] |= mask;
	}

	const char* to_string()
	{
		static char buff[0x400];
		memset(buff, 0, std::size(buff));
		for (u16 i = 0; i < m_BitsCount; i++)
		{
			buff[m_BitsCount - i - 1] = test(i) ? '1' : '0';
		}
		buff[m_BitsCount] = '\0';
		return buff;
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

