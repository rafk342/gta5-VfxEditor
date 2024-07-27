#pragma once
#include "rage/math/vec.h"

#include "rage/paging/ref.h"
#include "rage/paging/base.h"

#include "ptxKeyframeProp.h"

#include "ptxDomain.h"


namespace rage
{
	struct ptxEmitterRule : public pgBaseRefCounted
	{
		float m_fileVersion;
		const char* m_name;
		void* m_pLastEvoList_NOTUSED;
		void* m_pUIData;

		ptxDomainObj m_creationDomainObj;									// the emitter rule's creation domain (defines where points get created - i.e. sets their initial position)
		ptxDomainObj m_targetDomainObj;										// the emitter rule's target domain (defines where points initially travel towards - i.e. sets their initial velocity)
		ptxDomainObj m_attractorDomainObj;									// the emitter rule's attractor domain (defines an area where particle with an attractor behaviour will be attracted towards)

		u8 m_pad[12];

		ptxKeyframeProp m_spawnRateOverTimeKFP;								// keyframeable rate at which the emitter spawns points over time
		ptxKeyframeProp m_spawnRateOverDistKFP;								// keyframeable rate at which the emitter spawns points over distance
		ptxKeyframeProp m_particleLifeKFP;									// keyframeable	life of the emitted points
		ptxKeyframeProp m_playbackRateScalarKFP;							// keyframeable playback rate scalar of the emitted points
		ptxKeyframeProp m_speedScalarKFP;									// keyframeable speed scalar of the emitted points
		ptxKeyframeProp m_sizeScalarKFP;									// keyframeable size scalar of the emitted points
		ptxKeyframeProp m_accnScalarKFP;									// keyframeable acceleration scalar of the emitted points
		ptxKeyframeProp m_dampeningScalarKFP;								// keyframeable dampening scalar of the emitted points 
		ptxKeyframeProp m_matrixWeightScalarKFP;							// keyframeable matrix weight scalar of the emitted points
		ptxKeyframeProp m_inheritVelocityKFP;								// keyframeable amount of velocity that the emitted points inherit from the emitter

		ptxKeyframePropList m_keyframePropList;								// list of keyframeable properties

		bool m_isOneShot;													// whether this emitter spawns all of its points in its first frame of life
		
		u8 m_pad2[7];
	};

}