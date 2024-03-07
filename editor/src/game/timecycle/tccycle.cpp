#include "tccycle.h"


float tcCycle::GetKeyframeValue(int region, int id, int timeSampleIndex) const 
{
	return GetFloat32_FromFloat16(m_keyframes[region][timeSampleIndex].tc_vars[id]);
}

void tcCycle::SetKeyframeValue(int region, int id, int timeSampleIndex, float val)
{
	this->m_keyframes[region][timeSampleIndex].tc_vars[id] = GetFloat16_FromFloat32(val);
}

u32 tcCycle::GetCycleNameHash()
{
	return m_hash;
}

