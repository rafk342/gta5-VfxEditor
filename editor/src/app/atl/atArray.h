#pragma once
#include "common/types.h"
#include <cstdint>
#include <initializer_list>
#include <vector>

#include "base/tlsContext.h"
#include "logger.h"

namespace
{
	using namespace rage;
	inline u8 BitScanR32(u32 value)
	{
		unsigned long maxPageBit;
		_BitScanReverse(&maxPageBit, value);
		return static_cast<u8>(maxPageBit);
	}
#define IS_POWER_OF_TWO(value) ((value) != 0 && ((value) & ((value) - 1)) == 0)
#define ALIGN_POWER_OF_TWO_32(value) (u32)(IS_POWER_OF_TWO((value)) ? (value) : 1u << (BitScanR32((value)) + 1))
#define NEXT_POWER_OF_TWO_32(value) ((value) < 2 ? 2 : ALIGN_POWER_OF_TWO_32((value) + 1))
}



template<class TValue>
class atArray
{
	TValue* m_offset = nullptr;
	u16 m_size;
	u16 m_capacity;

public:

	atArray() :
		m_offset(nullptr),
		m_size(0),
		m_capacity(0)
	{
	}

	TValue*			begin()					{ return m_offset; }
	TValue*			end()					{ return m_offset + m_size; }
	TValue*			data() const			{ return m_offset; }
	u16				GetCapacity() const		{ return m_capacity; }
	u16				GetSize() const			{ return m_size; }
	bool			empty()	const			{ return (m_size == 0); }

	TValue& Get(u16 offset) {
		return (offset < m_size) ? m_offset[offset] : m_offset[0];
	}

	TValue&		  operator[](u16 idx)		{ return Get(idx); }
	const TValue& operator[](u16 idx) const	{ return Get(idx); }

	void push_back(const TValue& value)
	{ 
		if (m_capacity == 0)
			reserve(16);

		else if (m_size >= m_capacity)
			reserve(NEXT_POWER_OF_TWO_32(m_capacity));

		new (&m_offset[m_size]) TValue(std::move(value));
		m_size++;
	}

	void reserve(u16 new_cap, bool directly_memcpy = true)
	{
		if (new_cap <= m_capacity)
			return;
		size_t cpySz = 0;
		size_t alloc_sz = new_cap * sizeof(TValue);

		TValue* newOffset = reinterpret_cast<TValue*>(tlsContext::get()->m_allocator->Allocate(alloc_sz, 16, 0));
		memset(newOffset, 0, alloc_sz);

		if (!m_offset)
		{
			m_offset = newOffset;
			m_capacity = new_cap;
			return;
		}
		
		switch (directly_memcpy)
		{
		case true:
			cpySz = m_size * sizeof(TValue);
			memcpy_s(newOffset, alloc_sz, m_offset, cpySz);
			memset(m_offset, 0, cpySz);
			tlsContext::get()->m_allocator->Free(m_offset);
			break;

		case false:
			for (size_t i = 0; i < m_size; ++i) {
				new (&newOffset[i]) TValue(std::move(m_offset[i]));
			}
			std::destroy(begin(), end());
			memset(m_offset, 0, m_size * sizeof(TValue));
			tlsContext::get()->m_allocator->Free(m_offset);
			break;

		}
		m_offset = newOffset;
		m_capacity = new_cap;
	}
	
	void pop_back()
	{
		if (m_size == 0)
			return;
		
		m_size -= 1;
		m_offset[m_size].~TValue();
	}

	// returns -1 if value was not found
	size_t IndexOf(const TValue& v) const
	{
		for (size_t i = 0; i < m_size; i++) 
		{
			if (m_offset[i] == v)
				return i;
		}
		return -1;
	}
	
	void RemoveAt(u16 idx)
	{
		if (idx >= m_size) {
			mlogger("trying to remove elem at idx >= size");
			return;
		}
		m_offset[idx].~TValue();
		memset(&m_offset[idx], 0, sizeof(TValue));
		
		size_t move_size = (m_size - idx - 1) * sizeof(TValue);
		memmove_s(&m_offset[idx], move_size, &m_offset[idx + 1], move_size);
		memset(&m_offset[m_size - 1], 0, sizeof(TValue));
		
		m_size -= 1;
	}

	void clear()
	{
		std::destroy(begin(), end());
		m_size = 0;
	}

	~atArray()
	{
		clear();

		if (m_offset)
		{
			tlsContext::get()->m_allocator->Free(m_offset);
			m_offset = nullptr;
		}
	}
};
