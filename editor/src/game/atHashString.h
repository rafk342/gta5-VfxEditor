#pragma once

#include "common/types.h"
#include "memory/address.h"

namespace rage
{
	//In most cases it is useless, in the final version they use atHashValue, it doesn't save string
	inline const char* atHashString_TryGetCStr(u32 hash, u32 _namespace = 1)
	{
		static auto fn = gmAddress::Scan("85 D2 75 04 33 C0 EB 61").GetAt(-0x18).ToFunc<const char*(u32, u32)>();
		return fn(_namespace, hash);
	}
}

