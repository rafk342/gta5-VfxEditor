#include <format>
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
}


struct AttributePredicate
{
	const char* node_name;

	explicit AttributePredicate(const char* name) : node_name(name) {};

	bool operator()(pugi::xml_node attr) const
	{
		return std::strcmp(attr.name(), node_name) == 0;
	}
};

void tcXmlParser::load_tcData(const std::filesystem::path& path, tcCycle* cycle_to_load)
{
	if (cycle_to_load == nullptr) 
		return;

	pugi::xml_document doc;
	pugi::xml_parse_result res = doc.load_file(path.c_str());
	
	if(!res) 
		return;

	pugi::xml_node root = doc.first_child();
	
	if (!(static_cast<std::string>(root.name()) == "timecycle_keyframe_data")) 
		return;

	//std::string attr_cycle_name = root.child("cycle").attribute("name").value();	//doesn't matter here
	pugi::xml_node cycle_node = root.child("cycle");
	std::string param_line;

	int region_index = 0;

	for (auto region_node : cycle_node.children("region"))
	{
		for (size_t v_idx = 0; v_idx < TIMECYCLE_VAR_COUNT; v_idx++)
		{
			AttributePredicate predicate(g_varInfos[v_idx].name);
			pugi::xml_node param_node = region_node.find_child(predicate);
			
			if (param_node)
			{
				param_line = static_cast<std::string>(param_node.text().get());
				std::vector<float> tmp_vec = convert_str_to_float_arr(param_line, TC_TIME_SAMPLES);

				for (size_t time = 0; time < TC_TIME_SAMPLES; time++)
				{
					cycle_to_load->SetKeyframeValue(static_cast<Regions>(region_index), v_idx, time, tmp_vec[time]);
				}
			}
			else
			{
				for (size_t time = 0; time < TC_TIME_SAMPLES; time++)
				{
					cycle_to_load->SetKeyframeValue(static_cast<Regions>(region_index), v_idx, time, 0.0f);
				}
			}
		}
		region_index++;
		
		if (region_index >= 2)
			break;
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
	cycle_node.append_attribute("regions") = "2";


	for (u8 region_idx = 0; region_idx < 2; region_idx++)
	{
		pugi::xml_node region_node = cycle_node.append_child("region");
		
		switch (region_idx)
		{
		case(0):
			region_node.append_attribute("name") = "GLOBAL";
			break;
		case(1):
			region_node.append_attribute("name") = "URBAN"; 
			break;
		}

		for (size_t param_index = 0; param_index < TIMECYCLE_VAR_COUNT; param_index++)
		{
			region_node.append_child(g_varInfos[param_index].name).text() = getTcParamsLine(cycle, static_cast<Regions>(region_idx), param_index).c_str();
		}
	}
	doc.save_file(path.c_str());
}



std::string& tcXmlParser::getTcParamsLine(const tcCycle* cycle, Regions region, int paramId)
{
	static std::string params_line;
	params_line = " ";

	for (int time = 0; time < TC_TIME_SAMPLES; time++)
	{
		params_line += std::format("{:.4f} ", cycle->GetKeyframeValue(region, paramId, time));
	}
	return params_line;
}

