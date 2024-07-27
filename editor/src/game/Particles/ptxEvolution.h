#pragma once
#include "rage/atl/atArray.h"
#include "CloudSettings/CloudSettings.h"

namespace rage
{
	struct ptxEvolvedKeyframe
	{
		ptxKeyframe	m_keyframe;															// the evolved keyframe data 
		int m_evolutionIdx;																// the index of the evolution
		bool m_isLodEvolution;															// the index of any lod evolution
		u8 m_pad[11];
	};

	struct ptxEvolvedKeyframeProp
	{
		atArray<ptxEvolvedKeyframe> m_evolvedKeyframes;				// array of this property's evolved keyframes
		u32 m_propertyId;											// the id of the property we're keyframing (usually set a hash of the class and variable name)
		u8 m_blendMode;												// how the base and evolved keyframes are blended to produce the final keyframe
		u8 pad[3];
	};

	struct ptxEvolution
	{
		const char* m_name;
		u32 m_hash;
		float m_overrideValue;															// the override value (from the editor)
		bool m_isOverriden;																// whether the evolution value is overriden (from the editor)
		u8 pad[3];
	};

	struct ptxEvolutionList
	{
		atArray<ptxEvolution> m_evolutions;												// array of evolutions
		atArray<ptxEvolvedKeyframeProp> m_evolvedKeyframeProps;							// array of evolved keyframed properties
		//atStringMap<datRef<ptxEvolvedKeyframeProp> > m_evolvedKeyframePropMap;			// map of evolved keyframed properties
		u8 pad[24];
	};
}