#pragma once

#include "app/common/types.h"
#include "app/memory/address.h"
#include "app/crypto/float16.h"
#include <string>
#include "tcdef.h"

#include "tcdef.h"
#include "tcData.h"


struct __declspec(align(32)) tcKeyframe
{
	u16 tc_vars[432];
};

class tcCycle
{
	tcKeyframe	m_keyframes[REGIONS_COUNT][TC_TIME_SAMPLES];
	u32			m_hash;
public:
	float		GetKeyframeValue(int region, int id, int timeSampleIndex) const;
	void		SetKeyframeValue(int region, int id, int timeSampleIndex, float val);
	u32			GetCycleNameHash();
};

enum Regions
{
	GLOBAL = 0,
	URBAN
};
