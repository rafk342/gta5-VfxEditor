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
}


template<class TValue>
class atArray
{
	TValue* m_offset = nullptr;
	u16 m_size;
	u16 m_capacity;

public:

	atArray()
	{
		m_offset = nullptr;
		m_size = 0;
		m_capacity = 0;
	}

	TValue*			begin()					{ return &m_offset[0]; }
	TValue*			end()					{ return &m_offset[m_size]; }
	TValue*			data() const			{ return m_offset; }
	inline u16		GetCapacity() const		{ return m_capacity; }
	inline u16		GetSize() const			{ return m_size; }
	bool			empty()	const			{ return (m_size == 0); }

	TValue& Get(u16 offset)
	{
		if (offset >= m_size || offset < 0)
		{
			return m_offset[0];
		}
		return m_offset[offset];
	}

	TValue& operator[](u16 idx)				{ return Get(idx); }
	const TValue& operator[](u16 idx) const	{ return Get(idx); }

	void	push_back(const TValue& value)	{ set(m_size, value); }
	void	push_back(TValue&& value)		{ set(m_size, std::move(value)); }

private:
	void set(u16 offset, TValue&& value)
	{
		if (offset >= m_capacity)
		{
			if (m_capacity == 0)
				reserve(16);
			else
				reserve(offset * 2);
		}
		if (offset >= m_size)
		{
			m_size = offset + 1;
		}
		m_offset[offset] = value;
	}
public:

	void reserve(u16 new_cap)
	{
		if (m_capacity >= new_cap)
			return;

		TValue* newOffset = reinterpret_cast<TValue*>(tlsContext::get()->m_allocator->Allocate(new_cap * sizeof(TValue), 16, 0));

		// initialize the new entries
		std::uninitialized_fill(newOffset, newOffset + new_cap, TValue());

		//for (u16 i = 0; i < m_size; ++i) {
		//	new (newOffset + i) TValue(std::move(m_offset[i]));
		//}

		if (m_offset)
		{
			std::copy(m_offset, m_offset + m_size, newOffset);
			tlsContext::get()->m_allocator->Free(m_offset);
		}

		m_offset = newOffset;
		m_capacity = new_cap;
	}
	
	void pop_back()
	{
		m_offset[m_size].~TValue();

		if (m_size > 0)
			--m_size;
	}

	void clear()
	{
		for (size_t i = 0; i < m_size; ++i) {
			m_offset[i].~TValue();
		}
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
