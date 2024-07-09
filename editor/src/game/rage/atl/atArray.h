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
#undef max

template<class TValue, typename TSize = u16> 
class atArray
{
	static_assert(std::is_unsigned_v<TSize>, "SizeType must be unsigned");

	TValue* m_offset = nullptr;
	TSize m_size = 0;
	TSize m_capacity = 0;

	void VerifyBufferCanFitOrGrow(size_t requested_sz)
	{
		if (m_capacity == 0) {
			reserve(16);
		} else if (requested_sz >= m_capacity) {
			reserve(NEXT_POWER_OF_TWO_32(requested_sz));
		}
	}

	size_t get_alloc_sz(size_t count) { return count * sizeof(TValue); }

public:

	atArray() = default;

	atArray(TSize count, const TValue& v)
	{
		VerifyBufferCanFitOrGrow(count);
		for (size_t i = 0; i < count; i++) {
			push_back(v);
		}
	}

	atArray(TSize count)
	{
		VerifyBufferCanFitOrGrow(count);
		for (size_t i = 0; i < count; i++)
		{
			push_back({});
		}
	}

	atArray(std::initializer_list<TValue> il)
	{
		VerifyBufferCanFitOrGrow(il.size());
		for (auto it = il.begin(); it != il.end(); ++it) {
			push_back(std::move(*it));
		}
	}

	template <class Iter, std::enable_if_t<std::_Is_iterator_v<Iter>,int> = 0>
	atArray(Iter start, Iter end)
	{
		size_t sz = std::distance(start, end);
		VerifyBufferCanFitOrGrow(sz);
		for (; start != end; start++) {
			push_back(*start);
		}
	}
	
	atArray(std::vector<TValue>& vec) : atArray(vec.begin(), vec.end()) {}
	
	atArray(const atArray& other)
	{
		clear();
		VerifyBufferCanFitOrGrow(other.m_capacity);
		for (size_t i = 0; i < other.size(); i++) {
			push_back(other.m_offset[i]);
		}
	}

	atArray& operator=(const atArray& other)
	{
		if (this == &other)
			return *this;

		clear();
		VerifyBufferCanFitOrGrow(other.size());
		for (size_t i = 0; i < other.size(); ++i) {
			push_back(other.m_offset[i]);
		}
		return *this;
	}

	atArray(atArray&& other) noexcept
	{
		std::swap(m_offset, other.m_offset);
		std::swap(m_size, other.m_size);
		std::swap(m_capacity, other.m_capacity);
	}

	atArray& operator=(atArray&& other) noexcept
	{
		if (this == &other)
			return *this;

		std::swap(m_offset, other.m_offset);
		std::swap(m_size, other.m_size);
		std::swap(m_capacity, other.m_capacity);

		return *this;
	}

	TValue*         begin()                 { return m_offset; }
	TValue*         end()                   { return m_offset + m_size; }
	TValue&         front()                 { return *(begin); }
	TValue&         back()                  { return *(end() - 1); }
	TValue*         data()                  { return m_offset; }
	u16             capacity() const        { return m_capacity; }
	u16             size() const	        { return m_size; }
	bool            empty() const           { return (m_size == 0); }
	bool            contains(const TValue& v) const { return index_of(v).has_value(); }
	size_t          max_size() const        { return std::numeric_limits<TSize>::max();}

	TValue&         at(u16 offset)              { return (offset < m_size) ? m_offset[offset] : m_offset[0]; }
	TValue&         operator[](u16 idx)	        { return at(idx); }
	const TValue&   operator[](u16 idx) const   { return at(idx); }

	void reserve(size_t new_cap)
	{
		if (new_cap <= m_capacity)
			return;

		if (new_cap > max_size())
		{
			if (m_capacity < max_size()) {
				new_cap = max_size();
			} else {
				LogInfo("AtArray ::reserve({}) -> Capacity limit was reached; max_size : {}", new_cap, u64(max_size()));
				throw "";
			}
		}

		TValue* newOffset = reinterpret_cast<TValue*>(alloc_fn(get_alloc_sz(new_cap)));

		if (!m_offset)
		{
			m_offset = newOffset;
			m_capacity = new_cap;
			return;
		}
		if constexpr (std::is_nothrow_move_constructible_v<TValue> || !std::is_copy_constructible_v<TValue>) {
			std::uninitialized_move(begin(), end(), newOffset);
		} else {
			std::uninitialized_copy(begin(), end(), newOffset);
		}
		std::destroy(begin(), end());
		dealloc_fn(m_offset);
		m_offset = newOffset;
		m_capacity = new_cap;
	}

	TValue& push_back(const TValue& value)
	{
		VerifyBufferCanFitOrGrow(m_size);
		new (&m_offset[m_size]) TValue(value);
		m_size++;
		return back();
	}
	
	TValue& push_back(TValue&& value)
	{
		VerifyBufferCanFitOrGrow(m_size);
		new (&m_offset[m_size]) TValue(std::move(value));
		m_size++;
		return back();
	}

	void pop_back()
	{
		if (!empty()) {
			m_size -= 1;
			end()->~TValue();
		}
	}

	std::optional<s32> index_of(const TValue& v) const
	{
		for (size_t i = 0; i < m_size; i++)
		{
			if (m_offset[i] == v)
				return i;
		}
		return std::nullopt;
	}


	void insert(size_t _where, const TValue& value)
	{
		if (_where > m_size)
			throw std::out_of_range(std::format("AtArray ::insert()\nIdx : {} is out of range", _where));;

		if (_where == m_size)
		{
			push_back(value);
			return;
		}
		VerifyBufferCanFitOrGrow(m_size + 1);
		size_t move_sz = (m_size - _where) * sizeof(TValue);
		memmove(
			m_offset + _where + 1, //dst 
			m_offset + _where, //src
			move_sz ); //sz
		
		new (&m_offset[_where]) TValue(value);
		m_size++;
	}

	void insert(size_t _where, std::initializer_list<TValue> il)
	{
		if (_where > m_size)
			throw std::out_of_range(std::format("AtArray ::insert()\nIdx : {} is out of range", _where));
		
		VerifyBufferCanFitOrGrow(m_size + il.size());
		auto it_begin = il.begin();
		auto it_end = il.end();
		if (_where == m_size)
		{
			for (; it_begin != it_end; it_begin++) {
				push_back(*it_begin);
			}
			return;
		}
		size_t move_sz = (m_size - _where) * sizeof(TValue);	
		memmove(
			m_offset + _where + il.size(), //dst 
			m_offset + _where, //src
			move_sz ); //sz

		for (; it_begin != it_end; it_begin++)
		{
			new (&m_offset[_where++]) TValue(std::move(*it_begin));
		}
		m_size += il.size();
	}
	
	template <class Iter, std::enable_if_t<std::_Is_iterator_v<Iter>, int> = 0>
	void insert(size_t _where, Iter first, Iter last)
	{
		if (_where > m_size)
			throw std::out_of_range(std::format("AtArray ::insert()\nIdx : {} is out of range", _where));

		size_t count = std::distance(first, last);
		VerifyBufferCanFitOrGrow(m_size + count);
		if (_where == m_size)
		{
			for (; first != last; first++) {
				push_back(*first);
			}
			return;
		}
		size_t move_sz = (m_size - _where) * sizeof(TValue);
		memmove(
			m_offset + _where + count, // dst
			m_offset + _where,         // src
			move_sz);                  // size

		for (; first != last; first++) 
		{
			new (&m_offset[_where++]) TValue(*first);
		}
		m_size += count;
	}


	void remove_at(size_t idx)
	{
		if (idx >= m_size) 
			return;
		
		m_offset[idx].~TValue();
		size_t move_sz = (m_size - idx - 1) * sizeof(TValue);
		memmove(
			&m_offset[idx], // dst
			&m_offset[idx + 1], //src
			move_sz ); //sz

		m_size -= 1;
	}

	void remove_range(size_t left, size_t right)
	{
		bool RangeCanBeRemoved = right < m_size && left < right;
		if (!RangeCanBeRemoved)
			return;

		auto* first = &m_offset[left];
		auto* last = &m_offset[right];
		size_t count = std::distance(first, last + 1);
		std::destroy(first, last);
		size_t move_sz = (m_size - right - 1) * sizeof(TValue);
		memmove(
			first, // dst
			last + 1, //src
			move_sz); //sz
		m_size -= count;
	}

	void erase_v(const TValue& v)
	{
		if (!empty()) 
		{
			for (size_t i = m_size - 1; i >= 0; --i)
				if (v == m_offset[i])
					remove_at(i);
		}
	}

	void clear()
	{
		if (!empty())
		{
			std::destroy(begin(), end());
			m_size = 0;
		}
	}

	~atArray()
	{
		clear();
		if (m_offset)
		{
			dealloc_fn(m_offset);
			m_offset = nullptr;
			m_capacity = 0;
		}
	}
};


