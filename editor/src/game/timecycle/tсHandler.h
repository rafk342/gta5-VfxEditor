#pragma once

#include <map>
#include <iostream>
#include <string>
#include <vector>
#include <array>

#include "app/common/types.h"
#include "app/memory/address.h"
#include "crypto/joaat.h"
#include "app/crypto/float16.h"

#include "tcData.h"


struct tcKeyframe
{
	u16 tc_vars[432];
};

class tcCycle
{
	tcKeyframe	m_keyframes[TC_REGIONS_COUNT][TC_TIME_SAMPLES];
	u32			m_hash;
	u8			pad[28];
public:
	__forceinline float GetKeyframeValue(Regions region, int id, int timeSampleIndex) const { return GetFloat32_FromFloat16(m_keyframes[static_cast<bool>(region)][timeSampleIndex].tc_vars[id]); };
	__forceinline void SetKeyframeValue(Regions region, int id, int timeSampleIndex, float val) { this->m_keyframes[static_cast<bool>(region)][timeSampleIndex].tc_vars[id] = GetFloat16_FromFloat32(val);};
	__forceinline u32 GetCycleNameHash() { return m_hash; };
};
static_assert(sizeof(tcCycle) == 0x57e0);


class timeñycleHandler
{
	using CyclesArray_t = std::array<tcCycle*, WEATHER_TC_FILES_COUNT>;
	using StringsArray_t = std::vector<std::string>;

	CyclesArray_t	m_CyclesArray = {{nullptr}};
	StringsArray_t	m_WeatherNames;

	//struct
	//{
	//	struct gVarInfo
	//	{
	//		u32			VarId;
	//		u8			pad[4];
	//		const char* name;
	//		float		DefaultValue;
	//		tcVarType_e	varType;
	//		u8			pad1[8];
	//	};
	//	static_assert(sizeof(gVarInfo) == 32);
	//	u32 NumVars;
	//	gVarInfo* gVarInfosArray;
	//
	//} m_TcConfig;

public:

	timeñycleHandler();
	std::string	GetCycleName(int index);
	const auto&	GetWeatherNamesVec() { return this->m_WeatherNames; }
	tcCycle* GetCycle(int index)	 { return m_CyclesArray[index]; }
};

