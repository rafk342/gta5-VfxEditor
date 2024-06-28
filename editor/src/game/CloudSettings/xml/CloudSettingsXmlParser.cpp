#include "CloudSettingsXmlParser.h"

#include "app/logger.h"
#include <chrono>
#include <helpers/SimpleTimer.h>

//		naming here is a bit trash
//		I couldn't come up with a good names

///////////////////////////////////////////////////////////////////////////////////////////////////////
//									Import

void CloudSettingsXmlParser::ImportCloudKfData(const std::filesystem::path& path, CloudsHandler& CloudsHandler)
{
	std::unique_lock lock(mtx);

	size_t bits_size = 21;
	std::vector<int> ProbabilitiesVec(bits_size, 0);
	std::vector<std::string> SettingsNames;

	pugi::xml_document doc;
	pugi::xml_parse_result res = doc.load_file(path.c_str());

	if (!res) 
		return; 

	pugi::xml_node root = doc.first_child();
	if (static_cast<std::string>(root.name()) != "CloudSettingsMap") 
		return;
	
	auto& g_Map = (*CloudsHandler.gCloudsMap).CloudSettings;
	CloudsHandler.NamedCloudsSettingsVec.clear();
	g_Map.clear();
	

	for (auto& ItemNode : root.child("SettingsMap"))
	{
		CloudHatSettings TempSettingsItem;
		TempSettingsItem.probability_array.reserve(21);
		SettingsNames.push_back(ItemNode.child("Name").text().get());
		
		for (auto& ParamNode : ItemNode.child("Settings"))
		{
			if (static_cast<std::string>(ParamNode.name()) == "CloudList")
			{
				FillProbabilityVecFromStr(ProbabilitiesVec, ParamNode.child("mProbability").text().get());
				TempSettingsItem.probability_array.insert(0, ProbabilitiesVec.begin(), ProbabilitiesVec.end());
				TempSettingsItem.bits = ParamNode.child("mBits").text().as_uint();
				
				std::fill(ProbabilitiesVec.begin(), ProbabilitiesVec.end(), 0);	// fill with 0 for the next node 
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


void CloudSettingsXmlParser::LoadKeyframeData(pugi::xml_node& params_node, CloudHatSettings& settings)
{
	std::unordered_map<std::string, ptxKeyframe&> KeyframesMap =
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

	//				 time	->	values
	static std::map<float, rage::Vec4V> preMap;	// we have an ability to check what time should have which values
	static std::string node_name;
	preMap.clear();
	node_name.clear();

	node_name = static_cast<std::string>(params_node.name());
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

		auto temp = convert_str_to_float_arr(line, 5);
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


void CloudSettingsXmlParser::FillProbabilityVecFromStr(std::vector<int>& vec, const std::string& text)
{
	std::istringstream ss(text);
	int num;
	int idx = 0;
	while (ss >> num && idx < vec.size())
	{
		vec[idx] = num;
		idx++;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
//									Export


void CloudSettingsXmlParser::ExportCloudKfData(const std::filesystem::path& path, CloudsHandler& cloudsHandler)
{
	std::unique_lock lock(mtx);
	
	const std::vector<CloudSettingsNamed>& CloudsData = cloudsHandler.NamedCloudsSettingsVec;
	pugi::xml_document doc;
	pugi::xml_node decl = doc.append_child(pugi::node_declaration);
	decl.append_attribute("version") = "1.0";
	decl.append_attribute("encoding") = "UTF-8";


	pugi::xml_node CloudSettingsMap = doc.append_child("CloudSettingsMap");
	pugi::xml_node KeyframeTimes = CloudSettingsMap.append_child("KeyframeTimes");


	KeyframeTimes.append_attribute("content") = content_type::content_type_float_arr;
	KeyframeTimes.text() = GetTimeStr().c_str();
	

	pugi::xml_node SettingsMap = CloudSettingsMap.append_child("SettingsMap");
	

	for (auto& clouds : CloudsData)
	{
		pugi::xml_node Item = SettingsMap.append_child("Item");
		
		pugi::xml_node Name = Item.append_child("Name");
		Name.text() = clouds.name.c_str();

		pugi::xml_node Settings_node = Item.append_child("Settings");
		pugi::xml_node CloudList = Settings_node.append_child("CloudList");
		
		pugi::xml_node mProbability = CloudList.append_child("mProbability");
		mProbability.append_attribute("content") = content_type::content_type_int_arr;
		mProbability.text() = GetProbabilityText(clouds.CloudSettings->probability_array).c_str();

		pugi::xml_node mBits = CloudList.append_child("mBits");
		mBits.append_attribute("bits") = vfmt("{}", clouds.CloudSettings->bits.size());
		mBits.append_attribute("content") = content_type::content_type_int_arr;
		mBits.text() = vfmt("\n\t\t\t\t\t  0x{:08X}\n\t\t\t\t\t", *(clouds.CloudSettings->bits.data()));

		
		{
			AppendKeyframeData(Settings_node, "CloudColor",	clouds.CloudSettings->m_CloudColor.data);
			AppendKeyframeData(Settings_node, "CloudLightColor", clouds.CloudSettings->m_CloudLightColor.data);
			AppendKeyframeData(Settings_node, "CloudAmbientColor",clouds.CloudSettings->m_CloudAmbientColor.data);
			AppendKeyframeData(Settings_node, "CloudSkyColor",	clouds.CloudSettings->m_CloudSkyColor.data);
			AppendKeyframeData(Settings_node, "CloudBounceColor", clouds.CloudSettings->m_CloudBounceColor.data);
			AppendKeyframeData(Settings_node, "CloudEastColor",	clouds.CloudSettings->m_CloudEastColor.data);
			AppendKeyframeData(Settings_node, "CloudWestColor",	clouds.CloudSettings->m_CloudWestColor.data);
			AppendKeyframeData(Settings_node, "CloudScaleFillColors", clouds.CloudSettings->m_CloudScaleFillColors.data);
			AppendKeyframeData(Settings_node, "CloudDensityShift_Scale_ScatteringConst_Scale",	clouds.CloudSettings->m_CloudDensityShift_Scale_ScatteringConst_Scale.data);
			AppendKeyframeData(Settings_node, "CloudPiercingLightPower_Strength_NormalStrength_Thickness", clouds.CloudSettings->m_CloudPiercingLightPower_Strength_NormalStrength_Thickness.data);
			AppendKeyframeData(Settings_node, "CloudScaleDiffuseFillAmbient_WrapAmount", clouds.CloudSettings->m_CloudScaleDiffuseFillAmbient_WrapAmount.data);
		}
	}
	doc.save_file(path.c_str());
}


void CloudSettingsXmlParser::AppendKeyframeData(pugi::xml_node& settings_node, const char* param_name, atArray<ptxKeyframeEntry>& KeyframeEntries)
{
	pugi::xml_node KeyFramesName = settings_node.append_child(param_name);
	pugi::xml_node keyData = KeyFramesName.append_child("keyData");
	pugi::xml_node numKeyEntries = keyData.append_child("numKeyEntries");
	
	numKeyEntries.append_attribute("value") = vfmt("{}", KeyframeEntries.size());

	pugi::xml_node keyEntryData = keyData.append_child("keyEntryData");
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
		text += vfmt("\n\t\t\t\t\t\t  {:.6f}\t {:.6f}\t {:.6f}\t {:.6f}\t {:.6f}",
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
		text += vfmt("\n\t\t\t\t\t  {}", arr[i]);
	}
	text += "\n\t\t\t\t\t";
	return text;
}


std::string& CloudSettingsXmlParser::GetTimeStr()
{
	static std::string text;
	text.clear();
	for (size_t i = 0; i < time_arr.size() - 1; i++) {
		text += vfmt("\n\t  {:.6f}\t", time_arr[i]);
	}
	text += "\n\t";
	return text;
}



