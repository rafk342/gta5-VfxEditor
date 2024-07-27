#pragma once
#include "rage/atl/atArray.h"
#include "rage/paging/ref.h"
#include "ptxEvolution.h"
#include "rage/atl/color32.h"

namespace rage
{

	enum ptxEventType
	{
		PTXEVENT_TYPE_EMITTER = 0,
		PTXEVENT_TYPE_NUM
	};

	struct ptxEvent
	{
		virtual ~ptxEvent() {};
	
		int m_index;												// the index of the event 
		ptxEventType m_type;										// the event type (emitter or effect)
		float m_startRatio;											// the ratio of the effect's current duration when this event starts
		float m_endRatio;											// the ratio of the effect's current duration when this event ends
		datOwner<ptxEvolutionList> m_pEvolutionList;				// pointer to the list of evolutions on the event
		float m_distSqrToCamera;									// space to store the distance from the event to the camera (temp storage for sorting when drawing)
		u64 pad;
	};


	struct ptxEventEmitter : public ptxEvent
	{
		const char* m_emitterRuleName;											// the name of the emitter rule
		const char* m_particleRuleName;											// the name of the particle rule
		/*datRef ptxEmitterRule*/  void* m_pEmitterRule;									// pointer to the emitter rule
		/*datRef ptxParticleRule*/ void* m_pParticleRule;								// pointer to the effect rule

		float m_playbackRateScalarMin;											// scalars and tints that get applied to the emitter
		float m_playbackRateScalarMax;
		float m_zoomScalarMin;
		float m_zoomScalarMax;
		Color32 m_colourTintMin;
		Color32 m_colourTintMax;
	};

	struct ptxTimeLine
	{
		virtual ~ptxTimeLine() {};
		atArray<datOwner<ptxEvent>> m_events;
	};

}