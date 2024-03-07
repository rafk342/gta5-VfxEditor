#include "CloudSettingsXmlParser.h"

#include "app/logger.h"
#include <chrono>
#include <helpers/SimpleTimer.h>

//		naming here is a bit trash
//		I couldn't come up with a good names

///////////////////////////////////////////////////////////////////////////////////////////////////////
//									Import


void CloudSettingsXmlParser::ImportCloudKfData(const std::string& path, CloudsHandler& CloudsHandler)
{
	if (CloudsHandler.GetCloudSettingsVec().empty()) {
		return; 
	}

	int bits_size = CloudsHandler.GetCloudSettingsVec()[0].bits.size();
	
	std::vector<int> mProbabilityVec(bits_size, 0);
	std::string CloudsName, text;

	pugi::xml_document doc;
	pugi::xml_parse_result res = doc.load_file(path.c_str());

	if (!res) {
		return; 
	}

	pugi::xml_node root = doc.first_child();
	
	if (static_cast<std::string>(root.name()) != "CloudSettingsMap") {
		return;
	}

	pugi::xml_node settings_map_node = root.child("SettingsMap");

	for (auto& Item_node : settings_map_node)
	{
		CloudsName = Item_node.child("Name").text().get();
		auto* settings = CloudsHandler.FindCloudSettings(CloudsName.c_str());

		if (settings)
		{
			pugi::xml_node settings_node = Item_node.child("Settings");

			for (auto& param_node : settings_node)
			{
				if (static_cast<std::string>(param_node.name()) == "CloudList")
				{
					text = param_node.child("mProbability").text().get();
					FillProbabilityVecFromStr(mProbabilityVec, text);
					
					std::copy(mProbabilityVec.begin(), mProbabilityVec.begin() + bits_size, settings->CloudSettings->probability_array.begin());
					std::fill(mProbabilityVec.begin(), mProbabilityVec.end(), 0);	// fill with 0 for the next node 
					
					settings->bits = param_node.child("mBits").text().as_uint();
				}
				else
				{
					LoadKeyframeData(param_node, settings);
				}
			}
		}
		else
		{
			mlogger("Couldn't find " + CloudsName + " on import CloudKeyframes");
		}
	}
}


void CloudSettingsXmlParser::LoadKeyframeData(pugi::xml_node& param_node, CloudSettingsNamed* settings)
{
	//this can't be static
	std::unordered_map<std::string, ptxKeyframe&> KeyframesMap =
	{
		{"CloudColor",				settings->CloudSettings->m_CloudColor			},
		{"CloudLightColor",			settings->CloudSettings->m_CloudLightColor		},
		{"CloudAmbientColor",		settings->CloudSettings->m_CloudAmbientColor	},
		{"CloudSkyColor",			settings->CloudSettings->m_CloudSkyColor		},
		{"CloudBounceColor",		settings->CloudSettings->m_CloudBounceColor		},
		{"CloudEastColor",			settings->CloudSettings->m_CloudEastColor		},
		{"CloudWestColor",			settings->CloudSettings->m_CloudWestColor		},
		{"CloudScaleFillColors",	settings->CloudSettings->m_CloudScaleFillColors	},
		{"CloudDensityShift_Scale_ScatteringConst_Scale",				settings->CloudSettings->m_CloudDensityShift_Scale_ScatteringConst_Scale	},
		{"CloudPiercingLightPower_Strength_NormalStrength_Thickness",	settings->CloudSettings->m_CloudPiercingLightPower_Strength_NormalStrength_Thickness	},
		{"CloudScaleDiffuseFillAmbient_WrapAmount",						settings->CloudSettings->m_CloudScaleDiffuseFillAmbient_WrapAmount	},
	};

	//				 time	->	values
	static std::map<float, std::array<float, 4>> preMap;	// we have an ability to check what time should have which values
	static std::string	raw_text, node_name;
	preMap.clear();
	raw_text.clear();
	node_name.clear();


	node_name = static_cast<std::string>(param_node.name());
	if (!KeyframesMap.contains(node_name)){
		return;
	}

	pugi::xml_node keyData = param_node.child("keyData");
	pugi::xml_node keyEntryData = keyData.child("keyEntryData");
	raw_text = keyEntryData.text().get();

	FillPreMap(raw_text, preMap);
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
		if (strip_str(line).empty()) {
			continue;
		}

		temp = convert_str_to_float_arr(line, 5);
		preMap[temp[0]] = { temp[1], temp[2], temp[3], temp[4] };

		idx++;
	}
}


void CloudSettingsXmlParser::LoadKeyFrameDataToMem(ptxKeyframe& keyframe, std::map<float, std::array<float, 4>>& preMap)
{
	for (int i = 0; i < time_arr.size(); i++)
	{
		if (preMap.contains(time_arr[i]))
		{
			auto& values = preMap.at(time_arr[i]);

			for (int w = 0; w < 4; w++)
			{
				keyframe.data[i].vValue[w] = values[w];
			}
		}
		else
		{
			for (int w = 0; w < 4; w++)
			{
				keyframe.data[i].vValue[w] = 0;
			}
		}
	}
}


void CloudSettingsXmlParser::FillProbabilityVecFromStr(std::vector<int>& vec, std::string& text)
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


void CloudSettingsXmlParser::ExportCloudKfData(const std::string& path, const std::vector<CloudSettingsNamed>& CloudsData)
{
	static u32 __bits;

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
		Name.text() = clouds.str_name;

		pugi::xml_node Settings_node = Item.append_child("Settings");
		pugi::xml_node CloudList = Settings_node.append_child("CloudList");
		

		pugi::xml_node mProbability = CloudList.append_child("mProbability");
		mProbability.append_attribute("content") = content_type::content_type_int_arr;
		mProbability.text() = GetProbabilityText(clouds.CloudSettings->probability_array).c_str();


		pugi::xml_node mBits = CloudList.append_child("mBits");
		mBits.append_attribute("bits") = std::format("{}", clouds.bits.size()).c_str();
		mBits.append_attribute("content") = content_type::content_type_int_arr;
		__bits = clouds.bits.to_ulong();
		mBits.text() = std::format("\n\t\t\t\t\t  {}\n\t\t\t\t\t", GetStrHexFromU32(__bits)).c_str();

		
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
	
	numKeyEntries.append_attribute("value") = std::format("{}", keyframesData.GetSize()).c_str();

	pugi::xml_node keyEntryData = keyData.append_child("keyEntryData");
	keyEntryData.append_attribute("content") = content_type::content_type_float_arr; 
	
	keyEntryData.text() = GetKeyframesTextParams(keyframesData).c_str();
}


std::string& CloudSettingsXmlParser::GetKeyframesTextParams(atArray<ptxKeyframeEntry>& keyframesData)
{
	static std::string text;
	text.clear();

	for (size_t i = 0; i < keyframesData.GetSize(); i++)
	{
											 //time      r       g        b        a	
		text += std::format("\n\t\t\t\t\t\t  {:.6f}\t {:.6f}\t {:.6f}\t {:.6f}\t {:.6f}", 
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

	for (size_t i = 0; i < arr.GetSize(); i++)
	{
		text += std::format("\n\t\t\t\t\t  {}", arr[i]);
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
		text += std::format("\n\t  {:.6f}\t", time_arr[i]);
	}

	text += "\n\t";

	return text;
}



