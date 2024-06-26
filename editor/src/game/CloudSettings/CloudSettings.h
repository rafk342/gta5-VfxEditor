#pragma once
#include <iostream>
#include <unordered_map>
#include <bitset>
#include <map>

#include "common/types.h"
#include "memory/address.h"
#include "logger.h"
#include "crypto/joaat.h"
#include "compiler/compiler.h"
#include "rage/atl/atArray.h"
#include "rage/atl/atHashMap.h"
#include "rage/atl/atBitSet.h"
#include "helpers/helpers.h"
#include "rage/math/vecv.h"

struct ptxKeyframeEntry
{
	float vTime[4]{};
	float vValue[4]{};

	ptxKeyframeEntry(float time, rage::Vec4V values)
	{
		vTime[0] = time;
		for (int i = 0; i < 4; ++i) {
			vValue[i] = values[i];
		}
	}
};


class ptxKeyframe
{
public:
	atArray<ptxKeyframeEntry> data;
private:
	u8 pad[16];
};


struct CloudHatSettings				
{
private:
	u8 pad[16]{};
public:
	atArray<int> probability_array;
	atBitSet bits;

	ptxKeyframe m_CloudColor;
	ptxKeyframe m_CloudLightColor;
	ptxKeyframe m_CloudAmbientColor;
	ptxKeyframe m_CloudSkyColor;
	ptxKeyframe m_CloudBounceColor;
	ptxKeyframe m_CloudEastColor;
	ptxKeyframe m_CloudWestColor;
	ptxKeyframe m_CloudScaleFillColors;
	ptxKeyframe m_CloudDensityShift_Scale_ScatteringConst_Scale;
	ptxKeyframe m_CloudPiercingLightPower_Strength_NormalStrength_Thickness;
	ptxKeyframe m_CloudScaleDiffuseFillAmbient_WrapAmount;

	CloudHatSettings() : bits(21) {};
};



struct CloudSettingsNamed
{
	u32					hash = 0;
	std::string			name;
	CloudHatSettings*	CloudSettings = nullptr;

	CloudSettingsNamed(u32 hash, std::string name, CloudHatSettings* settings) 
		: hash(hash)
		, name(std::move(name))
		, CloudSettings(settings)
	{ }
};


class CloudsHandler
{
	friend class CloudSettingsXmlParser;
	struct gCloudSettingsMap
	{
	private:
		u8 pad[48];
	public:
		atMap<const char*, CloudHatSettings> CloudSettings;
		atArray<float> TimeData;
	};

	gCloudSettingsMap*				gCloudsMap = nullptr;
	void*							gCloudsMngr;
	std::vector<const char*>		gCloudHatNames;
	std::vector<CloudSettingsNamed>	NamedCloudsSettingsVec;
	
public: 

	CloudsHandler();

	auto& GetCloudSettingsVec() { return this->NamedCloudsSettingsVec; }
	auto& GetCloudHatNamesArray() { return this->gCloudHatNames; }
	
	void  RequestClearCloudHat();
	void  RequestCloudHat(const char* name, float time);
	u16	  GetNewRandomCloudhatIndex();
	int	  GetActiveCloudhatIndex();

	void fillCloudSettingsNamedVec(const auto& NamesContainer)
	{
		NamedCloudsSettingsVec.clear();
		for (auto [hash, settings_ptr] : gCloudsMap->CloudSettings.toVec())
		{
			auto it = std::find_if(NamesContainer.begin(), NamesContainer.end(), [hash](const std::string& name) { return rage::joaat(name.c_str()) == hash; });
			if (it != NamesContainer.end()) {
				NamedCloudsSettingsVec.push_back({ hash,*it,settings_ptr });
			} else {
				NamedCloudsSettingsVec.push_back({ hash,std::format("Unknown /hash : 0x{:08X}",hash),settings_ptr });
			}
		}
	}
};

