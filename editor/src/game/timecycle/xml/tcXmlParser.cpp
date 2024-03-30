#include <format>
#include "tcXmlParser.h"


struct AttributePredicate
{
	const char* node_name;

	explicit AttributePredicate(const char* name) : node_name(name) {};

	bool operator()(pugi::xml_node attr) const
	{
		return std::strcmp(attr.name(), node_name) == 0;
	}
};

void tcXmlParser::load_tcData(const std::string& path, tcCycle* cycle_to_load)
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
	std::array<float, TC_TIME_SAMPLES> params_arr;

	int region_index = 0;

	for (auto region_node : cycle_node.children("region"))
	{
		for (size_t param_id = 0; param_id < TIMECYCLE_VAR_COUNT; param_id++)
		{
			AttributePredicate predicate(g_varInfos[param_id].name);
			pugi::xml_node param_node = region_node.find_child(predicate);
			
			if (param_node)
			{
				param_line = static_cast<std::string>(param_node.text().get());

				std::vector<float> tmp_vec = convert_str_to_float_arr(param_line, TC_TIME_SAMPLES);
				std::copy(tmp_vec.begin(), tmp_vec.begin() + TC_TIME_SAMPLES, params_arr.begin());

				for (size_t time = 0; time < TC_TIME_SAMPLES; time++)
				{
					cycle_to_load->SetKeyframeValue(static_cast<Regions>(region_index), param_id, time, params_arr[time]);
				}
			}
			else
			{
				for (size_t time = 0; time < TC_TIME_SAMPLES; time++)
				{
					cycle_to_load->SetKeyframeValue(static_cast<Regions>(region_index), param_id, time, 0.0f);
				}
			}
		}
		region_index++;
		
		if (region_index > 2)
			break;
	}
}


void tcXmlParser::export_tcData(const std::string& path, const tcCycle* cycle, const std::string& cycle_name)
{
	pugi::xml_document doc;

	pugi::xml_node decl = doc.append_child(pugi::node_declaration);
	decl.append_attribute("version") = "1.0";
	decl.append_attribute("encoding") = "UTF-8";

	pugi::xml_node timecycle_keyframe_data = doc.append_child("timecycle_keyframe_data");
	timecycle_keyframe_data.append_attribute("version") = "1.000000";

	pugi::xml_node cycle_node = timecycle_keyframe_data.append_child("cycle");

	cycle_node.append_attribute("name") = cycle_name.c_str();
	cycle_node.append_attribute("regions") = "2";


	for (u8 region_i = 0; region_i < 2; region_i++)
	{
		pugi::xml_node region_node = cycle_node.append_child("region");
		
		switch (region_i)
		{
		case(0):
			region_node.append_attribute("name") = "GLOBAL"; break;
		case(1):
			region_node.append_attribute("name") = "URBAN"; break;
		}

		for (size_t param_index = 0; param_index < TIMECYCLE_VAR_COUNT; param_index++)
		{
			region_node.append_child(g_varInfos[param_index].name).text() = getTcParamsLine(cycle, region_i, param_index).c_str();
		}
	}
	doc.save_file(path.c_str());
}



std::string& tcXmlParser::getTcParamsLine(const tcCycle* cycle, int region, int paramId)
{
	static std::string params_line;
	params_line = " ";

	for (int time = 0; time < TC_TIME_SAMPLES; time++)
	{
		params_line += std::format("{:.4f} ", cycle->GetKeyframeValue(static_cast<Regions>(region), paramId, time));
	}
	return params_line;
}

