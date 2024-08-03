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
	u8 pad[16]{};
};
