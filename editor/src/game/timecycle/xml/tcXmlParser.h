#pragma once

#include <string>
#include <array>
#include <vector>
#include <sstream>

#include "pugixml/pugixml.hpp"

#include "game/timecycle/tccycle.h"
#include "game/timecycle/tcData.h"
#include "app/helpers/helpers.h"
#include "pugixml/pugixml.hpp"
#include "../tcdef.h"


class tcXmlParser
{
	std::string& getTcParamsLine(const tcCycle* cycle, int region, int paramId);
public:
	void load_tcData(const std::string& path, tcCycle* cycle_to_load);
	void export_tcData(const std::string& path, const tcCycle* cycle, const std::string& cycle_name);
};

