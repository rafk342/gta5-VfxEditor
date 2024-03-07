#pragma once

#include <format>
#include "common/types.h"


template <class counterType>
class atBitSet
{
	u32* m_Bits = nullptr;
	counterType m_Size;		//NumBitWords
	counterType m_BitSize;	//bits count

public:

	u32* getRawPtr() { return m_Bits; }
	void setRawPtrData(u32 data) { *m_Bits = data; }
	int GetNumBits() const { return m_BitSize; }
	int GetSize() const { return m_Size; }
};
