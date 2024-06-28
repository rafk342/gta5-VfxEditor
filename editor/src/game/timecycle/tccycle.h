#pragma once

#include "app/common/types.h"
#include "app/memory/address.h"
#include "app/crypto/float16.h"
#include <string>

#include "tcData.h"

enum Regions
{
	GLOBAL = 0,
	URBAN = 1,
};

struct tcKeyframe
{
	u16 tc_vars[432];
};

class tcCycle
{
	tcKeyframe	m_keyframes[REGIONS_COUNT][TC_TIME_SAMPLES];
	u32			m_hash;
public:
	float		GetKeyframeValue(Regions region, int id, int timeSampleIndex) const;
	void		SetKeyframeValue(Regions region, int id, int timeSampleIndex, float val);
	u32			GetCycleNameHash();
};

