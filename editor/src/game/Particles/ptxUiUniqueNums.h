#pragma once


inline size_t NextUniqueNum(bool reset = false)
{
	static size_t num = 1000;
	if (reset)
		num = 1000;
	return num++;
}