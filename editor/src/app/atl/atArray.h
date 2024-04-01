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

	TValue* begin()
	{
		return &m_offset[0];
	}

	TValue* end()
	{
		return &m_offset[m_size];
	}
	
	TValue* data() const
	{
		return m_offset;
	}

	inline u16 GetCapacity() const
	{
		return m_capacity;
	}

	inline u16 GetSize() const
	{
		return m_size;
	}

	TValue& Get(u16 offset)
	{
		if (offset >= m_size || offset < 0)
		{
			return m_offset[0];
		}

		return m_offset[offset];
	}

	TValue& operator[](u16 idx)
	{
		if (idx >= m_size)
		{
			return m_offset[0];
		}
		return m_offset[idx];
	}

	void push_back(const TValue& value)
	{
		set(m_size, value);
	}
	
	void push_back(TValue&& value)
	{
		set(m_size, std::move(value));
	}

	void set(u16 offset, TValue&& value)
	{
		if (offset >= m_capacity)
		{
			reserve(offset + 1);
		}
		if (offset >= m_size)
		{
			m_size = offset + 1;
		}

		m_offset[offset] = value;
	}

	void reserve(u16 newSize)
	{
		if (m_capacity >= newSize)
			return;

		TValue* newOffset = reinterpret_cast<TValue*>(tlsContext::get()->m_allocator->Allocate(newSize * sizeof(TValue), 16, 0));

		// initialize the new entries
		std::uninitialized_fill(newOffset, newOffset + newSize, TValue());
		//mlogger("for loop new (newOffset + i) TValue(std::move(m_offset[i]));");

		//for (u16 i = 0; i < m_size; ++i) {
		//	mlogger(std::format("i : {}", i));
		//	new (newOffset + i) TValue(std::move(m_offset[i]));
		//}
		//mlogger("for loop new (newOffset + i) TValue(std::move(m_offset[i])); done ");

		if (m_offset)
		{
			std::copy(m_offset, m_offset + m_size, newOffset);
			tlsContext::get()->m_allocator->Free(m_offset);
		}

		m_offset = newOffset;
		m_capacity = newSize;
	}
	
	void pop_back()
	{
		if (m_size > 0)
		{
			m_size--;
			if (m_size == 0)
			{
				clear();
			}
		}
	}

	void clear()
	{
		m_capacity = 0;
		m_size = 0;

		if (m_offset)
		{
			tlsContext::get()->m_allocator->Free(m_offset);
			m_offset = nullptr;
		}
	}

	~atArray()
	{
		for (size_t i = 0; i < m_size; ++i) {
			m_offset[i].~TValue();
		}

		clear();
	}
};
