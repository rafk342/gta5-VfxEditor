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
	void fillBits(CloudHatSettings& settings, pugi::xml_node& ParamNode);
	void fillProbabilitiesArray(CloudHatSettings& settings, pugi::xml_node& ParamNode);
	//std::vector<int> GetProbabilitiesVecFromStr(const char* text, u32 bits_count);
	void LoadKeyframeData(pugi::xml_node& params_node, CloudHatSettings& settings);
	void FillPreMap(const std::string& raw_text, std::map<float, rage::Vec4V>& preMap);
	void LoadKeyFrameDataToMem(ptxKeyframe& keyframe, std::map<float, rage::Vec4V>& preMap);

	//export
	std::string&	GetTimeStr();
	std::string&	GetProbabilityText(atArray<int>& arr);
	void			AppendKeyframeData(pugi::xml_node& settings_node,const char* param_name ,atArray<ptxKeyframeEntry>& keyframesData);
	std::string&	GetKeyframesTextParams(atArray<ptxKeyframeEntry>& keyframesData);

public:

	void ImportCloudKfData(const std::filesystem::path& path, CloudsHandler& CloudsHandler);
	void ExportCloudKfData(const std::filesystem::path& path, CloudsHandler& cloudsHandler);

};
