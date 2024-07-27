#pragma once
#include "rage/atl/atArray.h"


namespace rage
{
	struct ptxBiasLink
	{
		char m_name[64];
		atArray<int> m_keyframePropIds;
		u8 m_randIndex;
		char m_pad[3];
	};

}