#include <format>
#include <map>
#include "tcXmlParser.h"


namespace
{
	std::unordered_map<std::string, std::string> fileNames =
	{
		{"w_extrasunny.xml"		, "EXTRASUNNY"	},
		{"w_clear.xml"			, "CLEAR"		},
		{"w_clouds.xml"			, "CLOUDS"		},
		{"w_smog.xml"			, "SMOG"		},
		{"w_foggy.xml"			, "FOGGY"		},
		{"w_overcast.xml"		, "OVERCAST"	},
		{"w_rain.xml"			, "RAIN"		},
		{"w_thunder.xml"		, "THUNDER"		},
		{"w_clearing.xml"		, "CLEARING"	},
		{"w_neutral.xml"		, "NEUTRAL"		},
		{"w_snow.xml"			, "SNOW"		},
		{"w_blizzard.xml"		, "BLIZZARD"	},
		{"w_halloween.xml"		, "HALLOWEEN"	},
		{"w_snowlight.xml"		, "SNOWLIGHT"	},
		{"w_xmas.xml"			, "XMAS"		},
	};

	std::map<std::string_view, Regions> RegionNamesMap
	{
		{ "GLOBAL", GLOBAL },
		{ "URBAN",  URBAN }
	};
}


void tcXmlParser::load_tcData(const std::filesystem::path& path, tcCycle* cycle_to_load)
{
	if (!cycle_to_load) 
		return;
	
	pugi::xml_document doc;
	pugi::xml_parse_result res = doc.load_file(path.c_str());
	if (!res) 
		return;

	pugi::xml_node root = doc.first_child();
	if (std::string_view(root.name()) != "timecycle_keyframe_data")
		return;

	for (auto region_node : root.child("cycle").children("region"))
	{
		std::string_view region_name_string = region_node.attribute("name").as_string();
		if (!RegionNamesMap.contains(region_name_string)) 
			continue;
		Regions RegionIndex = RegionNamesMap.at(region_name_string);

		for (size_t v_idx = 0; v_idx < TCVAR_NUM; v_idx++)
		{
			pugi::xml_node params_node = region_node.find_child([&v_idx](pugi::xml_node node) { return std::string_view(node.name()) == g_varInfos[v_idx].name; });
			if (params_node)
			{
				std::array temp = ConvertStrToArray<float, TC_TIME_SAMPLES>(params_node.text().as_string());
				for (size_t time = 0; time < TC_TIME_SAMPLES; time++)
				{
					cycle_to_load->SetKeyframeValue(RegionIndex, v_idx, time, temp[time]);
				}
			}
			else
			{
				for (size_t time = 0; time < TC_TIME_SAMPLES; time++)
				{
					cycle_to_load->SetKeyframeValue(RegionIndex, v_idx, time, 0.0f);
				}
			}
		}
	}
}


void tcXmlParser::export_tcData(const std::filesystem::path& path, const tcCycle* cycle, std::string cycle_name)
{
	if (fileNames.contains(path.filename().string()))
		cycle_name = fileNames.at(path.filename().string());
	
	pugi::xml_document doc;
	pugi::xml_node decl = doc.append_child(pugi::node_declaration);
	decl.append_attribute("version") = "1.0";
	decl.append_attribute("encoding") = "UTF-8";

	pugi::xml_node timecycle_keyframe_data = doc.append_child("timecycle_keyframe_data");
	timecycle_keyframe_data.append_attribute("version") = "1.000000";
	pugi::xml_node cycle_node = timecycle_keyframe_data.append_child("cycle");

	cycle_node.append_attribute("name") = cycle_name.c_str();
	cycle_node.append_attribute("regions") = TC_REGIONS_COUNT;

	AppendRegionNode(cycle, cycle_node, "GLOBAL");
	AppendRegionNode(cycle, cycle_node, "URBAN");

	doc.save_file(path.c_str());
}


void tcXmlParser::AppendRegionNode(const tcCycle* cycle, pugi::xml_node& cycle_node, const char* RegionName)
{
	pugi::xml_node region_node = cycle_node.append_child("region");
	region_node.append_attribute("name") = RegionName;

	for (size_t param_index = 0; param_index < TCVAR_NUM; param_index++)
	{
		region_node.append_child(g_varInfos[param_index].name).text() = getTcParamsLine(cycle, RegionNamesMap.at(RegionName), param_index).c_str();
	}
}

std::string& tcXmlParser::getTcParamsLine(const tcCycle* cycle, Regions region, int paramId)
{
	static std::string params_line;
	params_line = " ";

	for (size_t time = 0; time < TC_TIME_SAMPLES; time++)
	{
		params_line += vfmt("{:.4f} ", cycle->GetKeyframeValue(region, paramId, time));
	}
	return params_line;
}

