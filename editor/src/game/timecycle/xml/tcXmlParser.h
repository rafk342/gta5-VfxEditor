#pragma once

#include <string>
#include <array>
#include <vector>
#include <sstream>
#include <filesystem>

#include "pugixml/pugixml.hpp"

#include "game/timecycle/tccycle.h"
#include "game/timecycle/tcData.h"
#include "app/helpers/helpers.h"
#include "pugixml/pugixml.hpp"


class tcXmlParser
{
	std::string& getTcParamsLine(const tcCycle* cycle, Regions region, int paramId);
public:
	void load_tcData(const std::filesystem::path& path, tcCycle* cycle_to_load);
	void export_tcData(const std::filesystem::path& path, const tcCycle* cycle, std::string cycle_name);
};

