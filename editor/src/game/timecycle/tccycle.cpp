#include "tccycle.h"


float tcCycle::GetKeyframeValue(Regions region, int id, int timeSampleIndex) const
{
	return GetFloat32_FromFloat16(m_keyframes[static_cast<bool>(region)][timeSampleIndex].tc_vars[id]);
}

void tcCycle::SetKeyframeValue(Regions region, int id, int timeSampleIndex, float val)
{
	this->m_keyframes[static_cast<bool>(region)][timeSampleIndex].tc_vars[id] = GetFloat16_FromFloat32(val);
}

u32 tcCycle::GetCycleNameHash()
{
	return m_hash;
}

