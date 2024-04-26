#pragma once

#include <iostream>
#include <vector>
#include <array>
#include <sstream>
#include <format>
#include <unordered_map>
#include <map>
#include <filesystem>

#include "pugixml/pugixml.hpp"
#include "CloudSettings/CloudSettings.h"
#include "helpers/helpers.h"


class CloudSettingsXmlParser
{
	std::array<float, 15> time_arr = { 0, 4, 5 ,6 ,7 ,10 ,12 ,16 ,17 ,18 ,19 ,20 ,21 ,22 ,24 };
	
	struct content_type
	{
		static constexpr const char* content_type_float_arr = "float_array";
		static constexpr const char* content_type_int_arr = "int_array";
	};
	
	//import
	void FillProbabilityVecFromStr(std::vector<int>& vec, std::string& text);
	void LoadKeyframeData(pugi::xml_node& param_node, CloudSettingsNamed* settings);
	void FillPreMap(const std::string& raw_text, std::map<float, std::array<float, 4>>& preMap);
	void LoadKeyFrameDataToMem(ptxKeyframe& keyframe, std::map<float, std::array<float, 4>>& preMap);

	//export
	std::string&	GetTimeStr();
	std::string&	GetProbabilityText(atArray<int>& arr);
	void			AddKeyframeData(pugi::xml_node& settings_node,const char* param_name ,atArray<ptxKeyframeEntry>& keyframesData);
	std::string&	GetKeyframesTextParams(atArray<ptxKeyframeEntry>& keyframesData);

public:

	void ImportCloudKfData(const std::filesystem::path& path, CloudsHandler& CloudsHandler);
	void ExportCloudKfData(const std::filesystem::path& path, const std::vector<CloudSettingsNamed>& CloudsData);

};
