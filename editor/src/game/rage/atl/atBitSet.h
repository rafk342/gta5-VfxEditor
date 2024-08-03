#pragma once

#include <format>
#include "common/types.h"
#include "atArray.h"
#include "helpers/align.h"

#undef min
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
		m_BitWordsCount = ALIGN_32(bits_count) / (sizeof(u32) * 8); // 64 / 32 = 2 (u32)
		
		u16 alloc_sz = m_BitWordsCount * sizeof(u32); // 2 * 4 = 8 bytes
		m_bits = static_cast<u32*>(rage_malloc(alloc_sz));
		memset(m_bits, 0, alloc_sz);
	}

	atBitSet(const atBitSet& other)
	{
		if (m_bits)
		{
			rage_free(m_bits);
			m_bits = nullptr;
			m_BitWordsCount = 0;
			m_BitsCount = 0;
		}
		if (other.m_bits)
		{
			m_BitWordsCount = other.m_BitWordsCount;
			m_BitsCount = other.m_BitsCount;

			u16 sz = other.m_BitWordsCount * sizeof(u32);
			m_bits = static_cast<u32*>(rage_malloc(sz));
			memcpy(m_bits, other.m_bits, sz);
		}
	}

	atBitSet(atBitSet&& other) noexcept
	{
		std::swap(m_BitWordsCount, other.m_BitWordsCount);
		std::swap(m_BitsCount, other.m_BitsCount);
		std::swap(m_bits, other.m_bits);
	}

	atBitSet& operator= (atBitSet&& other) noexcept
	{
		if (this == &other)
			return *this;

		std::swap(m_BitWordsCount, other.m_BitWordsCount);
		std::swap(m_BitsCount, other.m_BitsCount);
		std::swap(m_bits, other.m_bits);

		return *this;
	}

	atBitSet& operator= (const atBitSet& other)
	{
		if (this == &other)
			return *this;

		if (m_bits)
		{
			rage_free(m_bits);
			m_bits = nullptr;
			m_BitWordsCount = 0;
			m_BitsCount = 0;
		}
		if (other.m_bits)
		{
			m_BitWordsCount = other.m_BitWordsCount;
			m_BitsCount = other.m_BitsCount;

			u16 sz = other.m_BitWordsCount * sizeof(u32);
			m_bits = static_cast<u32*>(rage_malloc(sz));
			memcpy(m_bits, other.m_bits, sz);
		}
		return *this;
	}

	atBitSet& operator= (u32 value)
	{
		if (m_bits)
			*m_bits = value;
		return *this;
	}

	void reset()
	{
		memset(m_bits, 0, m_BitWordsCount * sizeof(u32));
	}
	
	bool test(u16 bit) const
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

	void resize(u16 NewBitsCount)
	{
		if (NewBitsCount == m_BitsCount)
			return;

		if (NewBitsCount == 0)
		{
			if (m_bits)
			{
				rage_free(m_bits);
				m_bits = nullptr;
			}
			m_BitWordsCount = 0;
			m_BitsCount = 0;
		}
		else
		{
			u16 NewBitWordsCount = ALIGN_32(NewBitsCount) / (sizeof(u32) * 8);
			size_t alloc_sz = NewBitWordsCount * sizeof(u32);
			u32* new_bits = static_cast<u32*>(rage_malloc(alloc_sz));
			memset(new_bits, 0, alloc_sz);

			if (m_bits)
			{
				size_t cpy_sz = std::min(m_BitWordsCount, NewBitWordsCount) * sizeof(u32);
				memcpy(new_bits, m_bits, cpy_sz);
				rage_free(m_bits);
			}

			m_bits = new_bits;
			m_BitsCount = NewBitsCount;
			m_BitWordsCount = NewBitWordsCount;
		}
	}

	const char* to_string() const
	{
		static char buff[0x400];
		//memset(buff, 0, m_BitsCount);
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
			rage_free(m_bits);
			m_bits = nullptr;
			m_BitWordsCount = 0;
			m_BitsCount = 0;
		}
	}
};


template<int MaxBits, typename T>
class atFixedBitSet
{
	static_assert(MaxBits <= sizeof(T) * 8);
	T m_Value = 0;

public:

	atFixedBitSet() = default;
	atFixedBitSet(T value) : m_Value(value) {}

	size_t    size() const    { return MaxBits; }
	T*        data()          { return &m_Value; }
	void      reset()         { m_Value = 0; }
	operator  T() const       { return m_Value; }

	atFixedBitSet& operator= (T value)
	{
		m_Value = value;
		return *this;
	}

	atFixedBitSet& operator= (const atFixedBitSet& other)
	{
		m_Value = other.m_Value;
		return *this;
	}

	void set(T bit, bool on = true)
	{
		if (bit > MaxBits)
			return;

		T mask = 1 << bit;
		m_Value &= ~mask;
		if (on) 
			m_Value |= mask;
	}

	bool test(T bit) const
	{
		if (bit > MaxBits)
			return false;

		T mask = 1 << bit;
		return m_Value & mask;
	}
};
