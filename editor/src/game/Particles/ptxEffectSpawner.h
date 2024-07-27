#pragma once

#include "rage/math/vec.h"

#include "rage/paging/ref.h"
#include "rage/paging/base.h"

#include "ptxTimeline.h"
#include "ptxKeyframeProp.h"
#include "rage/paging/ref.h"
#include "rage/atl/atBitSet.h"
#include "ptxEffectRule.h"

namespace rage
{
	struct ptxSpawnedEffectScalars
	{
		float m_durationScalar;
		float m_playbackRateScalar;
		Color32 m_colourTintScalar;
		float m_zoomScalar;
		atFixedBitSet<5, u32> m_flags;
		u8 pad[12];
	};


	struct ptxEffectSpawner : public pgBase
	{
		u8 pad[8];
		ptxSpawnedEffectScalars m_spawnedEffectScalarsMin;				// min spawned effect scalar settings
		ptxSpawnedEffectScalars m_spawnedEffectScalarsMax;				// max spawned effect scalar settings
		datRef<ptxEffectRule> m_pEffectRule;	//datRef						/ pointer to the effect rule that gets spawned
		const char* m_name;											// the name of the effect rule that gets spawned - rsTODO: get rid of this (and others like it) if we do a 2 pass dependency check then load when loading xml data
		float m_triggerInfo;											// trigger info (either the ratio through the point life when the effect gets spawned or the velocity threshold of an effect that gets spawned on collision)
		bool m_inheritsPointLife;										// whether the spawned effect inherits the life of the point
		bool m_tracksPointPos;											// whether the spawned effect tracks the position of the point
		bool m_tracksPointDir;											// whether the spawned effect tracks the direction of the point
		bool m_tracksPointNegDir;										// whether the spawned effect tracks the negative direction of the point
	};
}