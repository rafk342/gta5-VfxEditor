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


#define RageAllocUsage true
namespace rage
{
	inline void* _RageMalloc(size_t size) { return rage::tlsContext::get()->m_allocator->Allocate(size, 16, 0); }
	inline void _RageFree(void* block){ sysMemAllocator* _allocator = rage::tlsContext::get()->m_allocator; if (_allocator) { _allocator->Free(block); } }
}
#if RageAllocUsage
#define rage_malloc(size) rage::_RageMalloc(size)
#define rage_free(block) rage::_RageFree(block)
#else
#define rage_malloc(size) malloc(size)
#define rage_free(block) free(block)
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
		if (requested_sz >= m_capacity) 
		{
			size_t sz = NEXT_POWER_OF_TWO_32(requested_sz);
			reserve(sz);
		}
	}

	size_t get_alloc_sz(size_t count) { return count * sizeof(TValue); }

public:

	atArray() = default;

	atArray(TSize count)
	{
		resize(count);
	}

	atArray(TSize count, const TValue& v)
	{
		VerifyBufferCanFitOrGrow(count);
		for (size_t i = 0; i < count; i++) {
			push_back(v);
		}
	}

	atArray(std::initializer_list<TValue> il)
	{
		VerifyBufferCanFitOrGrow(il.size());
		std::uninitialized_copy(il.begin(), il.end(), begin());
		m_size = il.size();
	}

	template <class Iter, std::enable_if_t<std::_Is_iterator_v<Iter>, int> = 0>
	atArray(Iter start, Iter end)
	{
		size_t sz = std::distance(start, end);
		VerifyBufferCanFitOrGrow(sz);
		std::uninitialized_copy(start, end, begin());
		m_size = sz;
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

	TValue*				begin()					    { return m_offset; }
	TValue*				end()                       { return m_offset + m_size; }
	TValue&				front()                     { return *(begin); }
	TValue&				back()                      { return *(end() - 1); }
	TValue*				data()                      { return m_offset; }
	u16					capacity() const            { return m_capacity; }
	u16					size() const	            { return m_size; }
	bool				empty() const               { return (m_size == 0); }
	bool				contains(const TValue& v) const { return index_of(v).has_value(); }
	size_t				max_size() const            { return std::numeric_limits<TSize>::max();}

	TValue&				at(u16 offset)              { return (offset < m_size) ? m_offset[offset] : m_offset[0]; }
	TValue&				operator[](u16 idx)	        { return at(idx); }
	const TValue&		operator[](u16 idx) const   { return at(idx); }

	void reserve(size_t new_cap)
	{
		if (new_cap <= m_capacity)
			return;

		if (new_cap > max_size())
		{
			if (m_capacity < max_size()) {
				new_cap = max_size();
			} else {
				LogInfo("atArray ::reserve({}) -> Capacity limit was reached; max_size : {}", new_cap, u64(max_size()));
				throw "";
			}
		}

		TValue* newOffset = reinterpret_cast<TValue*>(rage_malloc(get_alloc_sz(new_cap)));

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
		rage_free(m_offset);
		m_offset = newOffset;
		m_capacity = new_cap;
	}

	template <typename... Args>
	TValue& emplace_back(Args&&... args)
	{
		VerifyBufferCanFitOrGrow(m_size);
		new (&m_offset[m_size]) TValue(std::forward<Args>(args)...);
		m_size++;
		return back();
	}

	TValue& push_back(const TValue& value)
	{
		return emplace_back(value);
	}

	TValue& push_back(TValue&& value)
	{
		return emplace_back(std::move(value));
	}

	void pop_back()
	{
		if (!empty()) {
			m_size -= 1;
			std::destroy_at(end());
		}
	}

	void resize(TSize new_sz)
	{
		if (m_size == new_sz)
			return;

		if (new_sz < m_size)
		{
			std::destroy(m_offset + new_sz, end());
		}
		else
		{
			VerifyBufferCanFitOrGrow(new_sz);
			std::uninitialized_fill(m_offset + m_size, m_offset + new_sz, TValue());
		}
		m_size = new_sz;
	}

	void insert(size_t _where, const TValue& value)
	{
		if (_where > m_size)
			throw std::out_of_range(std::format("AtArray ::insert()\nIdx : {} is out of range", _where));

		VerifyBufferCanFitOrGrow(m_size + 1);
		if (_where == m_size)
		{
			emplace_back(value);
			return;
		}
		
		auto* pos = m_offset + _where;
		auto* _end = end();

		std::construct_at(_end, TValue());
		if constexpr (std::is_nothrow_move_constructible_v<TValue>) {
			std::move_backward(pos, _end, _end + 1);
		} else {
			std::copy_backward(pos, _end, _end + 1);
		}
		std::destroy_at(pos);
		std::construct_at(pos, value);
		//(*pos) = const_cast<TValue&>(value);
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

		auto* pos = m_offset + _where;
		auto* _end = end();
		auto* next_end = _end + il.size();
		
		std::uninitialized_fill(_end, next_end, TValue());
		if constexpr (std::is_nothrow_move_constructible_v<TValue>) {
			std::move_backward(pos, _end, next_end);
		} else {
			std::copy_backward(pos, _end, next_end);
		}
		
		std::destroy(pos, pos + il.size());
		for (; it_begin != it_end; it_begin++, pos++)
		{
			std::construct_at(pos, *it_begin);
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
			for (auto it = first; it != last; ++it) 
			{
				push_back(*it);
			}
			return;
		}

		auto* pos = m_offset + _where;
		auto* curr_end = end();
		auto* new_end = end() + count;
		
		std::uninitialized_fill(curr_end, new_end, TValue());
		
		if constexpr (std::is_nothrow_move_constructible_v<TValue>) {
			std::move_backward(pos, curr_end, new_end);
		} else {
			std::copy_backward(pos, curr_end, new_end);
		}
		
		std::destroy(pos, pos + count);
		for (auto it = first; it != last; ++it, ++pos) 
		{
			std::construct_at(pos, *it);
		}
		m_size += count;
	}

	void remove_at(size_t idx)
	{
		if (idx >= m_size)
			return;
		
		auto* _where = m_offset + idx;
		
		if (_where == &back())
		{
			pop_back();
			return;
		}

		std::destroy_at(_where);
		if constexpr (std::is_nothrow_move_assignable_v<TValue>) {
			std::move(_where + 1, end(), _where);
		} else {
			std::copy(_where + 1, end(), _where);
		}
	
		m_size -= 1;
		std::destroy_at(end());	// object here is still valid
	}
	
	//void remove_range(size_t left, size_t right)
	//{
	//	bool RangeCanBeRemoved = right < m_size && left < right;
	//	if (!RangeCanBeRemoved)
	//		return;

	//	auto* first = &m_offset[left];
	//	auto* last = &m_offset[right];
	//	size_t count = std::distance(first, last + 1);
	//	std::destroy(first, last);
	//	size_t move_sz = (m_size - right - 1) * sizeof(TValue);
	//	memmove(
	//		first, // dst
	//		last + 1, //src
	//		move_sz); //sz
	//	m_size -= count;
	//}

	void erase_v(const TValue& v)
	{
		if (empty())
			return;

		for (size_t i = m_size - 1; i >= 0; --i) {
			if (v == m_offset[i]) {
				remove_at(i);
			}
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
			rage_free(m_offset);
			m_offset = nullptr;
			m_capacity = 0;
		}
	}
};


