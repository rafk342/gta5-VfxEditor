#pragma once
#include <cstdint>
#include <initializer_list>
#include <vector>
#include <type_traits>

#include "common/types.h"
#include "helpers/bits.h"
#include "rage/base/tlsContext.h"
#include "logger.h"
#include "helpers/align.h"

#define rage_alloc true

#if rage_alloc
#define alloc_fn(sz) 	rage::tlsContext::get()->m_allocator->Allocate(sz, 16, 0)
#define dealloc_fn(ptr) rage::tlsContext::get()->m_allocator->Free(ptr);
#else
#define alloc_fn(sz) malloc(sz)
#define dealloc_fn(ptr) free(ptr)
#endif

template<class TValue, bool reserve_with_directly_memcpy_for_this_type = false>
class atArray
{
	TValue* m_offset = nullptr;
	u16 m_size = 0;
	u16 m_capacity = 0;

	void manage_capacity(size_t sz)
	{
		if (m_capacity == 0)
			reserve(16);

		else if (sz >= m_capacity)
			reserve(NEXT_POWER_OF_TWO_32(sz));
	}

public:

	atArray() = default;

	atArray(u16 count, const TValue& v)
	{
		manage_capacity(count);
		for (size_t i = 0; i < count; i++) {
			push_back(v);
		}
	}

	atArray(u16 count)
	{
		manage_capacity(count);
	}

	atArray(std::initializer_list<TValue> il)
	{
		manage_capacity(il.size());
		for (auto it = il.begin(); it != il.end(); ++it) {
			push_back(*it);
		}
	}

	template<typename Iter>
	atArray(Iter start, Iter end)
	{
		static_assert(std::is_same_v<typename std::iterator_traits<Iter>::value_type, TValue>, "Invalid iterator type");

		size_t sz = std::distance(start, end);
		manage_capacity(sz);

		while (start != end)
		{
			push_back(*start);
			start++;
		}
	}
	
	atArray(std::vector<TValue>& vec) : atArray(vec.begin(), vec.end()) {}
	
	TValue*         begin()                 { return m_offset; }
	TValue*         end()                   { return m_offset + m_size; }
	TValue*         back()                  { return m_size == 0 ? nullptr : end() - 1; }
	TValue*         data()                  { return m_offset; }
	u16             GetCapacity() const     { return m_capacity; }
	u16             GetSize() const	        { return m_size; }
	bool            empty() const           { return (m_size == 0); }

	TValue& Get(u16 offset) { return (offset < m_size) ? m_offset[offset] : m_offset[0]; }

	TValue&	      operator[](u16 idx)       { return Get(idx); }
	const TValue& operator[](u16 idx) const { return Get(idx); }

	atArray& operator=(const atArray& other)
	{
		if (this == &other)
			return *this;

		clear();
		manage_capacity(other.GetSize());

		for (size_t i = 0; i < other.GetSize(); ++i) {
			push_back(other.m_offset[i]);
		}
		return *this;
	}

	atArray& operator=(atArray&& other) noexcept 
	{
		if (this == &other)
			return *this;

		if (m_offset)
		{
			clear();
			dealloc_fn(m_offset);
			m_offset = nullptr;
		}
		
		m_size = 0;
		m_capacity = 0;

		std::swap(m_offset, other.m_offset);
		std::swap(m_size, other.m_size);
		std::swap(m_capacity, other.m_capacity);

		return *this;
	}

	atArray(const atArray& other)
	{
		clear();
		manage_capacity(other.m_capacity);

		for (size_t i = 0; i < other.GetSize(); i++)
		{
			push_back(other.m_offset[i]);
		}
	}

	atArray(atArray&& other) noexcept
	{
		if (m_offset)
		{
			clear();
			dealloc_fn(m_offset);
			m_offset = nullptr;
		}
		
		m_size = 0;
		m_capacity = 0;

		std::swap(m_offset, other.m_offset);
		std::swap(m_size, other.m_size);
		std::swap(m_capacity, other.m_capacity);
	}

	void reserve(size_t new_cap)
	{
		if (new_cap <= m_capacity)
			return;

		size_t cpySz = 0;
		size_t alloc_sz = new_cap * sizeof(TValue);
		TValue* newOffset = reinterpret_cast<TValue*>(alloc_fn(alloc_sz));

		memset(newOffset, 0, alloc_sz);

		if (!m_offset)
		{
			m_offset = newOffset;
			m_capacity = new_cap;
			return;
		}

		if constexpr (reserve_with_directly_memcpy_for_this_type)
		{
			cpySz = m_size * sizeof(TValue);
			memcpy_s(newOffset, alloc_sz, m_offset, cpySz);
			memset(m_offset, 0, cpySz);
			dealloc_fn(m_offset);

		} else {

			for (size_t i = 0; i < m_size; ++i) {
				new (&newOffset[i]) TValue(std::move(m_offset[i]));
			}
			std::destroy(begin(), end());
			memset(m_offset, 0, m_capacity * sizeof(TValue));
			dealloc_fn(m_offset);
		}

		m_offset = newOffset;
		m_capacity = new_cap;
	}

	TValue& push_back(const TValue& value)
	{
		manage_capacity(m_size);
	
		new (&m_offset[m_size]) TValue(value);
		m_size++;
		return *back();
	}
	
	TValue& push_back(TValue&& value)
	{
		manage_capacity(m_size);
	
		new (&m_offset[m_size]) TValue(std::move(value));
		m_size++;
		return *back();
	}

	std::optional<s32> IndexOf(const TValue& v) const
	{
		for (size_t i = 0; i < m_size; i++)
		{
			if (m_offset[i] == v)
				return i;
		}
		return std::nullopt;
	}

	bool Contains(const TValue& v) const { return IndexOf(v).has_value(); }

	void pop_back()
	{
		if (empty())
			return;

		m_size -= 1;
		end()->~TValue();
	}

	void RemoveAt(u16 idx)
	{
		if (idx >= m_size) {
			return;
		}
		m_offset[idx].~TValue();
		memset(&m_offset[idx], 0, sizeof(TValue));

		size_t move_size = (m_size - idx - 1) * sizeof(TValue);
		memmove_s(&m_offset[idx], move_size,
			&m_offset[idx + 1], move_size);

		m_size -= 1;
		memset(end(), 0, sizeof(TValue));
	}

	void erase(const TValue& v)
	{
		if (empty())
			return;

		for (size_t i = m_size - 1; i >= 0; --i)
			if (v == m_offset[i])
				RemoveAt(i);
	}

	void clear()
	{
		if (empty())
			return;

		std::destroy(begin(), end());
		m_size = 0;
	}

	~atArray()
	{
		clear();

		if (m_offset)
		{
			std::memset(m_offset, 0, m_capacity * sizeof(TValue));
			dealloc_fn(m_offset);
			m_offset = nullptr;
			m_capacity = 0;
		}
	}
};

