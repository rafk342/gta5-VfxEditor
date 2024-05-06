#include "tñHandler.h"
#include <sstream>
#include <format>


timeñycleHandler::timeñycleHandler()
{
	NamesMap = {
		{rage::joaat("EXTRASUNNY")	, "EXTRASUNNY"	},
		{rage::joaat("CLEAR")		, "CLEAR"		},
		{rage::joaat("CLOUDS")		, "CLOUDS"		},
		{rage::joaat("SMOG")		, "SMOG"		},
		{rage::joaat("FOGGY")		, "FOGGY"		},
		{rage::joaat("OVERCAST")	, "OVERCAST"	},
		{rage::joaat("RAIN")		, "RAIN"		},
		{rage::joaat("THUNDER")		, "THUNDER"		},
		{rage::joaat("CLEARING")	, "CLEARING"	},
		{rage::joaat("NEUTRAL")		, "NEUTRAL"		},
		{rage::joaat("SNOW")		, "SNOW"		},
		{rage::joaat("BLIZZARD")	, "BLIZZARD"	},
		{rage::joaat("HALLOWEEN")	, "HALLOWEEN"	},
		{rage::joaat("SNOWLIGHT")	, "SNOWLIGHT"	},
		{rage::joaat("XMAS")		, "XMAS"		},
	};
	InitCyclesArr();
}

void timeñycleHandler::InitCyclesArr()
{
	weather_names.reserve(15);

	static void* tcMngr = gmAddress::Scan("48 C1 E0 03 41 80 E1 01 C6 44 24 20 00")
		.GetRef(22)
		.To<void*>();

	static auto fn = gmAddress::Scan("83 FA 10 73 ?? 8B C2 48 69 C0 E0 57 00 00")
		.ToFunc<tcCycle*(void*,u32)>();

	for (size_t i = 0; i < WEATHER_TC_FILES_COUNT; i++)
	{
		cyclesArray[i] = fn(tcMngr, i);
		weather_names.push_back(GetCycleName(i));
	}
}

const std::string timeñycleHandler::GetCycleName(int index)
{
	u32 n_hash = cyclesArray[index]->GetCycleNameHash();
	
	if (NamesMap.contains(n_hash)) {
		return NamesMap.at(n_hash);
	} else {
		return std::format("Unknown tcCycle name /hash : 0x{:08X}", n_hash);
	}
}

timeñycleHandler::~timeñycleHandler()
{
	for (size_t i = 0; i < cyclesArray.size(); i++)
	{
		cyclesArray[i] = nullptr;
	}
}
