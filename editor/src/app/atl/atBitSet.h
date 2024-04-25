#pragma once

#include <format>
#include "common/types.h"


template <typename counterType>
class atBitSet
{
	u32* m_Bits = nullptr;
	counterType m_Size;
	counterType m_BitSize;

public:
	u32* getRawPtr() { return m_Bits; }
	void setRawPtrData(u32 data) { *m_Bits = data; }
	counterType GetNumBits() const { return m_BitSize; }
	counterType GetSize() const { return m_Size; }
};
