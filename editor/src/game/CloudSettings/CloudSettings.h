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
#include "atl/atArray.h"
#include "atl/atHashMap.h"
#include "atl/atBitSet.h"
#include "helpers/helpers.h"


struct ptxKeyframeEntry
{
	float vTime[4];
	float vValue[4];
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
	u8 pad[16];	
public:
	atArray<int>  probability_array;
	atBitSet<u16> bits;

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
};


struct CloudHatFragContainer
{
private:
	char pad01[64];
public:
	char Name[64];		//we need just names from here
private:
	char pad02[336];
};


struct CloudSettingsNamed
{
	u32					hash_name = 0;
	const char*			str_name = nullptr;
	CloudHatSettings*	CloudSettings = nullptr;
	std::bitset<21>		bits;

	CloudSettingsNamed(u32 hash, const char* name, CloudHatSettings* settings, u32 bits) :
		hash_name(hash),
		str_name(name),
		CloudSettings(settings),
		bits(bits)
	{}

};



class CloudsHandler
{
	// game related things
	struct gCloudSettingsMap
	{
	private:
		u8 pad[48];
	public:
		atHashMap<CloudHatSettings>				CloudSettings;
		atArray<float>							TimeData;
	};

	atArray<CloudHatFragContainer>*				gCloudHatNames;
	gCloudSettingsMap*							gCloudsMap = nullptr;
	u8*											gCloudsMngr;				//might be useful to keep it here

	// for our usage	
	std::vector<CloudSettingsNamed>				CloudsSettingsVec;
	std::vector<std::pair<u32, std::string>>	CloudNames;

public:

										CloudsHandler();
	CloudSettingsNamed*					FindCloudSettings(u32 hash);
	CloudSettingsNamed*					FindCloudSettings(const char* name);
	std::vector<CloudSettingsNamed>&	GetCloudSettingsVec() { return this->CloudsSettingsVec; }
	atArray<CloudHatFragContainer>&		GetCloudHatNamesArray() { return *(this->gCloudHatNames); }
	u8*									Get_raw_gCloudsMngrPtr() { return this->gCloudsMngr; }
	u16									GetNewRandomCloudhatIndex();

	void								RequestClearCloudHat();
	void								RequestCloudHat(const char* name, float time);
	int									GetActiveCloudhatIndex();
};
