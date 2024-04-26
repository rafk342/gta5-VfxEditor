#pragma once
#include <format>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <filesystem>

#include "pugixml/pugixml.hpp"
#include "helpers/helpers.h"
#include "VfxLightningSettings/VfxLightningSettings.h"

class VfxLightningsXmlParser
{

public:

	void AppendCloudBurstCommonSettingsXmlNodes(pugi::xml_node& parent, CloudBurstCommonSettings& settings);
	void AppendKeyData(pugi::xml_node& parent, ptxKeyframe& keyframe);
	std::string& GetKeyframesTextParams(ptxKeyframe& keyframe);

	void LoadCloudBurstCommonSettings(CloudBurstCommonSettings& settings, const pugi::xml_node& node);
	void LoadKeyframeData(const pugi::xml_node& keyData, ptxKeyframe& keyframe);

	void mImportLightningData(const std::filesystem::path& path, gVfxLightningSettings* settings);
	void mExportLightningData(const std::filesystem::path& path, gVfxLightningSettings* settings);

};

