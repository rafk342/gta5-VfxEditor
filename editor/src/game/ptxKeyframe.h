#pragma once
#include <iostream>
#include "stringzilla/include.h"
#include "rage/math/vec.h"
#include "rage/atl/atArray.h"
#include "helpers/helpers.h"

#include "imgui.h"

enum ptxKeyframeType
{
	PTX_KF_FLOAT,
	PTX_KF_FLOAT2,
	PTX_KF_FLOAT3,
	PTX_KF_FLOAT4,
	PTX_KF_COL3,
	PTX_KF_COL4,
};


struct ptxKeyframeEntry
{
	float vTime[4]{};
	float vValue[4]{};

	ptxKeyframeEntry() = default;
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







//struct ptxKeyframeDefn
//{
//	const char*		m_Name;
//	std::array<const char*, 4> m_Labels;
//	ptxKeyframeType m_Type;
//	rage::Vector4	m_DefaulValue = {};
//	rage::Vector3	m_MIN_MAX_DELTA_vTime = {};
//	rage::Vector3	m_MIN_MAX_DELTA_vValue = {};
//
//	ptxKeyframeDefn(
//		const char* name,
//		ptxKeyframeType ValueType,
//		rage::Vector4 DefaultValue = {},
//		rage::Vector3 MIN_MAX_DELTA_X = {FLT_MIN,FLT_MAX,0.01f},
//		rage::Vector3 MIN_MAX_DELTA_Y = {FLT_MIN,FLT_MAX,0.01f},
//		const char* lb0 = "unnamed",
//		const char* lb1 = "unnamed",
//		const char* lb2 = "unnamed",
//		const char* lb3 = "unnamed"
//	)
//		: m_Name(name)
//		, m_Labels { lb0, lb1, lb2, lb3 }
//		, m_Type(ValueType)
//		, m_DefaulValue(DefaultValue)
//		, m_MIN_MAX_DELTA_vTime(MIN_MAX_DELTA_X)
//		, m_MIN_MAX_DELTA_vValue(MIN_MAX_DELTA_Y)
//	{ }
//};

//
//template<size_t TimeValuesInUse, typename... Args> 
//inline void PtxKeyframeTableTreeNode(ptxKeyframe& ptxKf, const ptxKeyframeDefn& defn, Args... ImArgs) 
//{
//	switch (defn.m_Type)
//	{
//	case PTX_KF_FLOAT:
//
//		break;
//	case PTX_KF_FLOAT2:
//
//		break;
//	case PTX_KF_FLOAT3:
//
//		break;
//	case PTX_KF_FLOAT4:
//
//		break;
//	case PTX_KF_COL3:
//
//		break;
//	case PTX_KF_COL4:
//
//		break;
//	default:
//		break;
//	}
//};
//
