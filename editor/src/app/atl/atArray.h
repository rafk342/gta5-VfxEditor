#pragma once
#include "common/types.h"


template<class TValue>
class atArray
{
	TValue* m_offset = nullptr;
	u16 m_size;
	u16 m_capacity;

public:

	atArray()
	{
		m_offset = (TValue*)0;
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

	inline u16 GetCapacity()
	{
		return m_capacity;
	}

	inline u16 GetSize()
	{
		return m_size;
	}

	TValue& Get(u16 offset)
	{
		if (offset >= m_size)
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
	~atArray()
	{
		m_offset = nullptr;
	}
};
