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
	using CyclesArrayType = std::array<tcCycle*, WEATHER_TC_FILES_COUNT>;
	using W_NamesArrayType = std::vector<std::string>;

	CyclesArrayType		m_CyclesArray = {{nullptr}};
	W_NamesArrayType	m_WeatherNames;
public:
	tcXmlParser			m_XmlParser;

						timeñycleHandler();
	std::string			GetCycleName(int index);
	const auto&			GetWeatherNamesVec() { return this->m_WeatherNames; }
	tcCycle*			GetCycle(int index)	 { return m_CyclesArray[index]; }
};



