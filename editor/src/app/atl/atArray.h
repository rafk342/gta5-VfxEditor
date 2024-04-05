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
	u16 m_size = 0;
	u16 m_capacity = 0;

public:

	atArray() :
		m_offset(nullptr),
		m_size(0),
		m_capacity(0)
	{
	}

	atArray(u16 count, const TValue& v) :
		m_offset(nullptr),
		m_size(0),
		m_capacity(0)
	{
		reserve(count);
		for (size_t i = 0; i < count; i++){
			push_back(v);
		}
	}

	atArray(u16 count)
	{
		reserve(count);
	}

	atArray(std::initializer_list<TValue> il)
	{
		reserve(il.size());
		for (auto it = il.begin(); it != il.end(); ++it) {
			push_back(*it);
		}
	}
	
	template<typename Iter>
	atArray(Iter start, Iter end)
	{
		u16 size = end - start;
		reserve(size);

		for (u16 i = 0; start < end; ++start, ++i) 
			m_offset[i] = *start;

		m_size = size;
	}

	atArray(std::vector<TValue>& vec) : atArray(vec.begin(), vec.end()) {}
	

	TValue*         begin()                 { return m_offset; }
	TValue*         end()                   { return m_offset + m_size; }
	TValue&			Last()					{ return *(end() - 1); }
	TValue*         data() const            { return m_offset; }
	u16             GetCapacity() const	    { return m_capacity; }
	u16	            GetSize() const	        { return m_size; }
	bool            empty() const           { return (m_size == 0); }

	TValue& Get(u16 offset) { return (offset < m_size) ? m_offset[offset] : m_offset[0]; }

	TValue&	      operator[](u16 idx)       { return Get(idx); }
	const TValue& operator[](u16 idx) const { return Get(idx); }

	// if we are sure that directly copying the objects memory in case of reallocation is completely safe - we can specify it.
	// we are not calling destructors and constructors in this case, so it should perform faster, I guess.
	TValue& push_back(const TValue& value, bool reserve_with_directly_memcpy = false)
	{ 
		if (m_capacity == 0)
			reserve(16);

		else if (m_size >= m_capacity)
			reserve(NEXT_POWER_OF_TWO_32(m_capacity), reserve_with_directly_memcpy);

		new (&m_offset[m_size]) TValue(std::move(value));
		m_size++;
		return Last();
	}

	void reserve(u16 new_cap, bool directly_memcpy = false)
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
		end().~TValue();
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
	
	bool Contains(const TValue& v)
	{
		return (IndexOf(v) != -1);
	}

	void RemoveAt(u16 idx)
	{
		if (idx >= m_size) {
			mlogger("trying to remove elem at idx >= size");
			assert(idx <= m_size && "trying to remove elem at idx >= size");
			return;
		}
		m_offset[idx].~TValue();
		memset(&m_offset[idx], 0, sizeof(TValue));
		
		size_t move_size = (m_size - idx - 1) * sizeof(TValue);
		memmove_s(&m_offset[idx], move_size, 
				  &m_offset[idx + 1], move_size);

		memset(&m_offset[m_size - 1], 0, sizeof(TValue));
		
		m_size -= 1;
	}

	void erase(const TValue& v)
	{
		for (size_t i = m_size - 1; i >= 0; --i) 
			if (v == m_offset[i]) 
				RemoveAt(i);
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
			std::memset(m_offset, 0, m_capacity);
			tlsContext::get()->m_allocator->Free(m_offset);
			m_offset = nullptr;
			m_capacity = 0;
		}
	}
};
