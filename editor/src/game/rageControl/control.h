#pragma once

#include "app/memory/address.h"
#include "app/common/types.h"

class GameInput
{
public:
	static void DisableAllControlsThisFrame()
	{
		static auto fn = gmAddress::Scan("40 53 48 83 EC 20 33 DB 85 C9 75 09").ToFunc<void*(int)>();
		fn(0);
	}
};

