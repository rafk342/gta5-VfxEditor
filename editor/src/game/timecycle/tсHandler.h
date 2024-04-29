#pragma once

#include <map>
#include <iostream>
#include <string>
#include <vector>
#include <array>

#include "tcdef.h"
#include "tccycle.h"
#include "xml/tcXmlParser.h"
#include "tcdef.h"
#include "crypto/joaat.h"

class timeñycleHandler
{
	std::array<tcCycle*, WEATHER_TC_FILES_COUNT> cyclesArray = {{nullptr}};
	std::vector<std::string>			weather_names;
	std::map<u32, std::string>			NamesMap;

	tcCycle*							LoadCycle(u32 cycleIndex);
	void								InitCyclesArr();

public:
										timeñycleHandler();
										~timeñycleHandler();
	const std::string					GetCycleName(int index);
	const std::vector<std::string>&		GetWeatherNamesVec() { return this->weather_names; }
	tcCycle*							GetCycle(int index) { return cyclesArray[index]; }

	tcXmlParser							xmlParser;
};



