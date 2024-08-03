#pragma once
#include "atArray.h"


template<class TKey, class TData>
class atBinaryMap
{
	struct Pair
	{
		TKey key;
		TData data;
	};
public:

	bool sorted;
	u8 pad[7];
	atArray<Pair> Data;
};
