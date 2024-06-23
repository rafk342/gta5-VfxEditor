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
	std::vector<int> mProbabilityVec(bits_size, 0);
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
				FillProbabilityVecFromStr(mProbabilityVec, ParamNode.child("mProbability").text().get());
				TempSettingsItem.probability_array.insert(0, mProbabilityVec.begin(), mProbabilityVec.end());
				TempSettingsItem.bits = ParamNode.child("mBits").text().as_uint();
				
				std::fill(mProbabilityVec.begin(), mProbabilityVec.end(), 0);	// fill with 0 for the next node 
			}
			else
			{
				LoadKeyframeData(ParamNode, &TempSettingsItem);
			}
		}
		g_Map.insert(SettingsNames.back().c_str(), TempSettingsItem);
	}


	for (auto [hash, settings_ptr] : g_Map.toVec())
	{
		auto it = std::find_if(SettingsNames.begin(), SettingsNames.end(), [hash](std::string& name) { return rage::joaat(name.c_str()) == hash; });

		if (it != SettingsNames.end()) {
			CloudsHandler.NamedCloudsSettingsVec.push_back({ hash,*it,settings_ptr });
		} else {
			CloudsHandler.NamedCloudsSettingsVec.push_back({ hash,std::format("Unknown /hash : 0x{:08X}",hash),settings_ptr });
		}
	}
}


void CloudSettingsXmlParser::LoadKeyframeData(pugi::xml_node& param_node, CloudHatSettings* settings)
{
	//this can't be static
	std::unordered_map<std::string, ptxKeyframe&> KeyframesMap =
	{
		{"CloudColor", settings->m_CloudColor},
		{"CloudLightColor",	settings->m_CloudLightColor	},
		{"CloudAmbientColor", settings->m_CloudAmbientColor	},
		{"CloudSkyColor", settings->m_CloudSkyColor	},
		{"CloudBounceColor", settings->m_CloudBounceColor},
		{"CloudEastColor", settings->m_CloudEastColor},
		{"CloudWestColor", settings->m_CloudWestColor},
		{"CloudScaleFillColors", settings->m_CloudScaleFillColors},
		{"CloudDensityShift_Scale_ScatteringConst_Scale", settings->m_CloudDensityShift_Scale_ScatteringConst_Scale	},
		{"CloudPiercingLightPower_Strength_NormalStrength_Thickness",settings->m_CloudPiercingLightPower_Strength_NormalStrength_Thickness	},
		{"CloudScaleDiffuseFillAmbient_WrapAmount",	settings->m_CloudScaleDiffuseFillAmbient_WrapAmount	},
	};

	//				 time	->	values
	static std::map<float, std::array<float, 4>> preMap;	// we have an ability to check what time should have which values
	static std::string node_name;
	preMap.clear();
	node_name.clear();


	node_name = static_cast<std::string>(param_node.name());
	if (!KeyframesMap.contains(node_name))
		return;

	pugi::xml_node keyData = param_node.child("keyData");
	pugi::xml_node keyEntryData = keyData.child("keyEntryData");
	
	FillPreMap(keyEntryData.text().get(), preMap);
	LoadKeyFrameDataToMem(KeyframesMap.at(node_name), preMap);
}


void CloudSettingsXmlParser::FillPreMap(const std::string& raw_text, std::map<float, std::array<float, 4>>& preMap)
{
	static std::string line;
	static std::vector<float> temp;
	line.clear();
	temp.clear();

	std::istringstream iss(raw_text);
	
	int	idx = 0;
	while (std::getline(iss, line, '\n') && idx < 15)
	{
		if (strip_str(line).empty()) 
			continue;

		temp = convert_str_to_float_arr(line, 5);
		preMap[temp[0]] = { temp[1], temp[2], temp[3], temp[4] };

		idx++;
	}
}


void CloudSettingsXmlParser::LoadKeyFrameDataToMem(ptxKeyframe& keyframe, std::map<float, std::array<float, 4>>& preMap)
{
	atArray<ptxKeyframeEntry>& kf_entries = keyframe.data;

	for (int i = 0; i < time_arr.size(); i++)
	{
		if (preMap.contains(time_arr[i]))
		{
			std::array<float, 4>& data = preMap.at(time_arr[i]);
			kf_entries.push_back(ptxKeyframeEntry(time_arr[i], { data[0],data[1],data[2],data[3] }));
		}
		else
		{
			kf_entries.push_back(ptxKeyframeEntry(time_arr[i], { 0,0,0,0 }));
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


void CloudSettingsXmlParser::ExportCloudKfData(const std::filesystem::path& path, const std::vector<CloudSettingsNamed>& CloudsData)
{
	std::unique_lock lock(mtx);

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
		Name.text() = clouds.str_name.c_str();

		pugi::xml_node Settings_node = Item.append_child("Settings");
		pugi::xml_node CloudList = Settings_node.append_child("CloudList");
		

		pugi::xml_node mProbability = CloudList.append_child("mProbability");
		mProbability.append_attribute("content") = content_type::content_type_int_arr;
		mProbability.text() = GetProbabilityText(clouds.CloudSettings->probability_array).c_str();


		pugi::xml_node mBits = CloudList.append_child("mBits");
		mBits.append_attribute("bits") = vfmt("{}", clouds.bits.size());
		mBits.append_attribute("content") = content_type::content_type_int_arr;
		mBits.text() = vfmt("\n\t\t\t\t\t  0x{:08X}\n\t\t\t\t\t", clouds.bits.to_ulong());

		
		{
			AddKeyframeData(Settings_node, "CloudColor",			clouds.CloudSettings->m_CloudColor.data);
			AddKeyframeData(Settings_node, "CloudLightColor",		clouds.CloudSettings->m_CloudLightColor.data);
			AddKeyframeData(Settings_node, "CloudAmbientColor",		clouds.CloudSettings->m_CloudAmbientColor.data);
			AddKeyframeData(Settings_node, "CloudSkyColor",			clouds.CloudSettings->m_CloudSkyColor.data);
			AddKeyframeData(Settings_node, "CloudBounceColor",		clouds.CloudSettings->m_CloudBounceColor.data);
			AddKeyframeData(Settings_node, "CloudEastColor",		clouds.CloudSettings->m_CloudEastColor.data);
			AddKeyframeData(Settings_node, "CloudWestColor",		clouds.CloudSettings->m_CloudWestColor.data);
			AddKeyframeData(Settings_node, "CloudScaleFillColors",	clouds.CloudSettings->m_CloudScaleFillColors.data);
			AddKeyframeData(Settings_node, "CloudDensityShift_Scale_ScatteringConst_Scale",				clouds.CloudSettings->m_CloudDensityShift_Scale_ScatteringConst_Scale.data);
			AddKeyframeData(Settings_node, "CloudPiercingLightPower_Strength_NormalStrength_Thickness", clouds.CloudSettings->m_CloudPiercingLightPower_Strength_NormalStrength_Thickness.data);
			AddKeyframeData(Settings_node, "CloudScaleDiffuseFillAmbient_WrapAmount",					clouds.CloudSettings->m_CloudScaleDiffuseFillAmbient_WrapAmount.data);
		}
	}
	doc.save_file(path.c_str());
}


void CloudSettingsXmlParser::AddKeyframeData(pugi::xml_node& settings_node, const char* param_name, atArray<ptxKeyframeEntry>& keyframesData)
{
	pugi::xml_node KeyFramesName = settings_node.append_child(param_name);
	pugi::xml_node keyData = KeyFramesName.append_child("keyData");
	pugi::xml_node numKeyEntries = keyData.append_child("numKeyEntries");
	
	numKeyEntries.append_attribute("value") = std::format("{}", keyframesData.size()).c_str();

	pugi::xml_node keyEntryData = keyData.append_child("keyEntryData");
	keyEntryData.append_attribute("content") = content_type::content_type_float_arr; 
	
	keyEntryData.text() = GetKeyframesTextParams(keyframesData).c_str();
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

	for (size_t i = 0; i < arr.size(); i++)
	{
		text += vfmt("\n\t\t\t\t\t  {}", arr[i]);
	}

	text += "\n\t\t\t\t\t";
	return text;
}


std::string& CloudSettingsXmlParser::GetTimeStr()
{
	static std::string text;
	text.clear();

	for (size_t i = 0; i < time_arr.size() - 1; i++)
	{
		text += vfmt("\n\t  {:.6f}\t", time_arr[i]);
	}

	text += "\n\t";

	return text;
}



