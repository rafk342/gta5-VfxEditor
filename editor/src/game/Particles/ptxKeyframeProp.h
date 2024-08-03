#pragma once
#include <map>
#include "ptxEvolution.h"
#include "rage/dat/datBase.h"
#include "rage/paging/ref.h"

namespace rage
{
	struct ptxKeyframeProp : public datBase
	{
		ptxEvolvedKeyframeProp* m_pEvolvedKeyframeProp[12]{};
		u32 m_propertyId;	// hash
		bool m_invertBiasLink;
		char m_randIndex;
		u8 m_pad[2];
		ptxKeyframe m_keyframe;
	};

	struct ptxKeyframePropList
	{
		atArray<datRef<ptxKeyframeProp>> m_pKeyframeProps;
	};
}