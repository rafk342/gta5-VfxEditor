#pragma once
#include "atArray.h"


template<class TData,class TKey>
class atBinaryMap
{
	struct Pair
	{
		TKey key;
		TData data;
	};
public:

	bool sorted;
	atArray<Pair> Data;
};
