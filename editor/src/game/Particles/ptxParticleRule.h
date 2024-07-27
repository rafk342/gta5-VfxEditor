#pragma once

#include "rage/math/vec.h"
#include "rage/atl/atArray.h"

#include "rage/paging/ref.h"
#include "rage/paging/base.h"

#include "ptxTimeline.h"
#include "ptxKeyframeProp.h"
#include "ptxEffectSpawner.h"
#include "ptxRenderState.h"
#include "ptxBehaviour.h"
#include "ptxBiasLink.h"
#include "ptxShader.h"
#include "ptxDrawable.h"

namespace rage
{
	struct ptxParticleRule : public pgBaseRefCounted
	{
		void* m_pUIData;

		ptxEffectSpawner m_effectSpawnerAtRatio;
		ptxEffectSpawner m_effectSpawnerOnColn;
		ptxRenderState m_renderState;
		
		float m_fileVersion;
		int m_texFrameIdMin;
		int m_texFrameIdMax;

		const char* m_name;

		atArray<datRef<ptxBehaviour>> m_allBehaviours;											// would be datOwner<ptxBehaviour> but there is intentionally no Place function in ptxBehaviour - which breaks the template.
		atArray<datRef<ptxBehaviour>> m_initBehaviours;
		atArray<datRef<ptxBehaviour>> m_updateBehaviours;
		atArray<datRef<ptxBehaviour>> m_updateFinalizeBehaviours;
		atArray<datRef<ptxBehaviour>> m_drawBehaviours;
		atArray<datRef<ptxBehaviour>> m_releaseBehaviours;

		atArray<ptxBiasLink> m_biasLinks;
		void* m_pPointPool;
		u8 pad[16];
		
		ptxShaderInst m_shaderInst;
		atArray<ptxDrawable> m_drawables;
		
		u8 m_sortType; 																			// ptxSortType
		u8 m_drawType; 																			// ptxDrawType

		u8 m_flags;
		
		u8 m_runtimeFlags;	//this is bitset
		
		void* m_pLastEvoList_NOTUSED;
		void* m_pWindBehaviour;
		u8 pad1[8];
	};
}