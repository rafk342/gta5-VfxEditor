#include "CloudSettingsXmlParser.h"

#include "app/logger.h"
#include <chrono>
#include <helpers/Timer.h>


///////////////////////////////////////////////////////////////////////////////////////////////////////
//									Import

void CloudSettingsXmlParser::ImportCloudKfData(const std::filesystem::path& path, CloudsHandler& CloudsHandler)
{	
	pugi::xml_document doc;
	pugi::xml_parse_result res = doc.load_file(path.c_str());
	if (!res)
		return;

	pugi::xml_node root = doc.first_child();
	if (std::string_view(root.name()) != "CloudSettingsMap")
		return;
	
	auto& g_Map = (*CloudsHandler.gCloudsMap).CloudSettings; 
	g_Map.clear();
	std::vector<std::string> SettingsNames;
	

	for (auto& ItemNode : root.child("SettingsMap").children())
	{
		SettingsNames.push_back(ItemNode.child("Name").text().as_string());
		CloudHatSettings TempSettingsItem;
		
		for (auto& ParamNode : ItemNode.child("Settings").children())
		{
			if (std::string_view(ParamNode.name()) == "CloudList")
			{
				fillBits(TempSettingsItem, ParamNode);
				fillProbabilitiesArray(TempSettingsItem, ParamNode);
			}
			else
			{
				LoadKeyframeData(ParamNode, TempSettingsItem);
			}
		}
		g_Map.insert(SettingsNames.back().c_str(), TempSettingsItem);
	}
	CloudsHandler.fillCloudSettingsNamedVec(SettingsNames);
}


void CloudSettingsXmlParser::fillBits(CloudHatSettings& settings, pugi::xml_node& ParamNode)
{
	pugi::xml_node BitsNode = ParamNode.child("mBits");
	u32 bits_count = BitsNode.attribute("bits").as_uint();
	settings.m_Bits.resize(bits_count);
	settings.m_Bits = BitsNode.text().as_uint();
}


void CloudSettingsXmlParser::LoadKeyframeData(pugi::xml_node& params_node, CloudHatSettings& settings)
{
	std::unordered_map<std::string_view, ptxKeyframe&> KeyframesMap =
	{
		{"CloudColor", settings.m_CloudColor},
		{"CloudLightColor",	settings.m_CloudLightColor	},
		{"CloudAmbientColor", settings.m_CloudAmbientColor	},
		{"CloudSkyColor", settings.m_CloudSkyColor	},
		{"CloudBounceColor", settings.m_CloudBounceColor},
		{"CloudEastColor", settings.m_CloudEastColor},
		{"CloudWestColor", settings.m_CloudWestColor},
		{"CloudScaleFillColors", settings.m_CloudScaleFillColors},
		{"CloudDensityShift_Scale_ScatteringConst_Scale", settings.m_CloudDensityShift_Scale_ScatteringConst_Scale	},
		{"CloudPiercingLightPower_Strength_NormalStrength_Thickness",settings.m_CloudPiercingLightPower_Strength_NormalStrength_Thickness	},
		{"CloudScaleDiffuseFillAmbient_WrapAmount",	settings.m_CloudScaleDiffuseFillAmbient_WrapAmount	},
	};

	//		 time	->	values
	std::map<float, rage::Vec4V> preMap; // there's an ability to check what time should have which values
	std::string_view node_name = params_node.name();
	if (!KeyframesMap.contains(node_name))
		return;

	pugi::xml_node keyData_Node = params_node.child("keyData");
	pugi::xml_node keyEntryData_Node = keyData_Node.child("keyEntryData");
	
	FillPreMap(keyEntryData_Node.text().get(), preMap);
	LoadKeyFrameDataToMem(KeyframesMap.at(node_name), preMap);
}


void CloudSettingsXmlParser::FillPreMap(const std::string& raw_text, std::map<float, rage::Vec4V>& preMap)
{
	static std::string line;
	int	idx = 0;
	line.clear();

	std::istringstream iss(raw_text);
	while (std::getline(iss, line, '\n') && idx < 15)
	{
		if (strip_str(line).empty()) 
			continue;

		std::array temp = ConvertStrToArray<float, 5>(line.c_str());
		preMap[temp[0]] = { temp[1], temp[2], temp[3], temp[4] };

		idx++;
	}
}


void CloudSettingsXmlParser::LoadKeyFrameDataToMem(ptxKeyframe& keyframe, std::map<float, rage::Vec4V>& preMap)
{
	for (int i = 0; i < time_arr.size(); i++)
	{
		if (preMap.contains(time_arr[i])) {
			keyframe.data.push_back(ptxKeyframeEntry(time_arr[i], preMap.at(time_arr[i])));
		} else {
			keyframe.data.push_back(ptxKeyframeEntry(time_arr[i], { 0,0,0,0 }));
		}
	}
}


void CloudSettingsXmlParser::fillProbabilitiesArray(CloudHatSettings& settings, pugi::xml_node& ParamNode)
{
	u32 bits_count = settings.m_Bits.size();
	auto& arr = settings.m_ProbabilitiesArray;
	arr.reserve(bits_count);

	std::istringstream iss(ParamNode.child("mProbability").text().as_string());
	int num = 0;
	for (size_t i = 0; iss >> num && i < bits_count; i++)
	{
		arr.push_back(num);
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
//									Export


void CloudSettingsXmlParser::ExportCloudKfData(const std::filesystem::path& path, CloudsHandler& cloudsHandler)
{	
	const std::vector<CloudSettingsNamed>& SettingsArray = cloudsHandler.NamedCloudsSettingsVec;
	pugi::xml_document doc;
	pugi::xml_node decl = doc.append_child(pugi::node_declaration);
	decl.append_attribute("version") = "1.0";
	decl.append_attribute("encoding") = "UTF-8";


	pugi::xml_node CloudSettingsMap = doc.append_child("CloudSettingsMap");
	pugi::xml_node KeyframeTimes = CloudSettingsMap.append_child("KeyframeTimes");


	KeyframeTimes.append_attribute("content") = content_type::content_type_float_arr;
	KeyframeTimes.text() = GetTimeStr().c_str();
	

	pugi::xml_node SettingsMap = CloudSettingsMap.append_child("SettingsMap");
	

	for (auto& Item : SettingsArray)
	{
		pugi::xml_node ItemNode = SettingsMap.append_child("Item");
		
		pugi::xml_node Name = ItemNode.append_child("Name");
		Name.text() = Item.name.c_str();

		pugi::xml_node Settings_node = ItemNode.append_child("Settings");
		pugi::xml_node CloudList = Settings_node.append_child("CloudList");
		
		pugi::xml_node mProbability = CloudList.append_child("mProbability");
		mProbability.append_attribute("content") = content_type::content_type_int_arr;
		mProbability.text() = GetProbabilityText(Item.CloudSettings->m_ProbabilitiesArray).c_str();

		pugi::xml_node mBits = CloudList.append_child("mBits");
		mBits.append_attribute("bits") = vfmt("{}", Item.CloudSettings->m_Bits.size());
		mBits.append_attribute("content") = content_type::content_type_int_arr;
		mBits.text() = vfmt("\n\t\t\t\t\t\t 0x{:08X}\n\t\t\t\t\t", *(Item.CloudSettings->m_Bits.data()));

		{
			AppendKeyframeData(Settings_node, "CloudColor",	Item.CloudSettings->m_CloudColor.data);
			AppendKeyframeData(Settings_node, "CloudLightColor", Item.CloudSettings->m_CloudLightColor.data);
			AppendKeyframeData(Settings_node, "CloudAmbientColor",Item.CloudSettings->m_CloudAmbientColor.data);
			AppendKeyframeData(Settings_node, "CloudSkyColor",	Item.CloudSettings->m_CloudSkyColor.data);
			AppendKeyframeData(Settings_node, "CloudBounceColor", Item.CloudSettings->m_CloudBounceColor.data);
			AppendKeyframeData(Settings_node, "CloudEastColor",	Item.CloudSettings->m_CloudEastColor.data);
			AppendKeyframeData(Settings_node, "CloudWestColor",	Item.CloudSettings->m_CloudWestColor.data);
			AppendKeyframeData(Settings_node, "CloudScaleFillColors", Item.CloudSettings->m_CloudScaleFillColors.data);
			AppendKeyframeData(Settings_node, "CloudDensityShift_Scale_ScatteringConst_Scale",	Item.CloudSettings->m_CloudDensityShift_Scale_ScatteringConst_Scale.data);
			AppendKeyframeData(Settings_node, "CloudPiercingLightPower_Strength_NormalStrength_Thickness", Item.CloudSettings->m_CloudPiercingLightPower_Strength_NormalStrength_Thickness.data);
			AppendKeyframeData(Settings_node, "CloudScaleDiffuseFillAmbient_WrapAmount", Item.CloudSettings->m_CloudScaleDiffuseFillAmbient_WrapAmount.data);
		}
	}
	doc.save_file(path.c_str());
}


void CloudSettingsXmlParser::AppendKeyframeData(pugi::xml_node& settings_node, const char* param_name, atArray<ptxKeyframeEntry>& KeyframeEntries)
{
	pugi::xml_node KeyFramesName = settings_node.append_child(param_name);
	pugi::xml_node keyData = KeyFramesName.append_child("keyData");
	pugi::xml_node numKeyEntries = keyData.append_child("numKeyEntries");
	pugi::xml_node keyEntryData = keyData.append_child("keyEntryData");
	
	numKeyEntries.append_attribute("value") = vfmt("{}", KeyframeEntries.size());
	keyEntryData.append_attribute("content") = content_type::content_type_float_arr; 
	keyEntryData.text() = GetKeyframesTextParams(KeyframeEntries).c_str();
}


std::string& CloudSettingsXmlParser::GetKeyframesTextParams(atArray<ptxKeyframeEntry>& keyframesData)
{
	static std::string text;
	text.clear();
	for (size_t i = 0; i < keyframesData.size(); i++)
	{
		                               //time      r       g        b        a	
		text += vfmt("\n\t\t\t\t\t\t\t{:.6f}\t {:.6f}\t {:.6f}\t {:.6f}\t {:.6f}",
			keyframesData[i].vTime[0],
			keyframesData[i].vValue[0], 
			keyframesData[i].vValue[1],
			keyframesData[i].vValue[2],
			keyframesData[i].vValue[3] );
	}
	text += "\n\t\t\t\t\t\t";
	return text;
}


std::string& CloudSettingsXmlParser::GetProbabilityText(atArray<int>& arr)
{
	static std::string text;
	text.clear();
	for (size_t i = 0; i < arr.size(); i++) {
		text += vfmt("\n\t\t\t\t\t\t{}", arr[i]);
	}
	text += "\n\t\t\t\t\t";
	return text;
}


std::string& CloudSettingsXmlParser::GetTimeStr()
{
	static std::string text;
	text.clear();
	for (size_t i = 0; i < time_arr.size() - 1; i++) {
		text += vfmt("\n\t\t{:.6f}", time_arr[i]);
	}
	text += "\n\t";
	return text;
}



