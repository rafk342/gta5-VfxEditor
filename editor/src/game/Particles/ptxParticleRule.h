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

enum PtFxDrawListId_e
{
	PTFX_DRAWLIST_QUALITY_LOW = 0,
	PTFX_DRAWLIST_QUALITY_MEDIUM,
	PTFX_DRAWLIST_QUALITY_HIGH,

	NUM_PTFX_DRAWLISTS
};

enum ptfxRenderFlags
{
	PTFX_RF_MEDRES_DOWNSAMPLE = 1,
	PTFX_RF_LORES_DOWNSAMPLE = 2,
};
enum ptxEffectFlags
{
	PTXEFFECTFLAG_KEEP_RESERVED	= (0),
	PTXEFFECTFLAG_MANUAL_DRAW	= (1),
	PTXEFFECTFLAG_USER1			= (2),
	PTXEFFECTFLAG_USER2			= (3),
	PTXEFFECTFLAG_USER3			= (4),
	PTXEFFECTFLAG_USER4			= (5),
	PTXEFFECTFLAG_USER5			= (6),
	PTXEFFECTFLAG_USER6			= (7),
	PTXEFFECTFLAG_USER7			= (8),
	PTXEFFECTFLAG_USER8			= (9),
	PTXEFFECTFLAG_USER9			= (10),
	PTXEFFECTFLAG_USER10		= (11),
	PTXEFFECTFLAG_USER11		= (12)		
};

#define PTFX_RESERVED_ATTACHED								(PTXEFFECTFLAG_USER1)
#define	PTFX_RESERVED_REGISTERED							(PTXEFFECTFLAG_USER2)
#define PTFX_RESERVED_SORTED								(PTXEFFECTFLAG_USER3)
#define PTFX_EFFECTINST_CONTAINS_REFRACT_RULE				(PTXEFFECTFLAG_USER4)
#define PTFX_EFFECTINST_CONTAINS_WATER_ABOVE_RULE			(PTXEFFECTFLAG_USER5)
#define PTFX_EFFECTINST_CONTAINS_WATER_BELOW_RULE			(PTXEFFECTFLAG_USER6)
#define PTFX_EFFECTINST_CONTAINS_WATER_BOTH_RULE			(PTXEFFECTFLAG_USER5 | PTXEFFECTFLAG_USER6)
#define PTFX_EFFECTINST_FORCE_VEHICLE_INTERIOR				(PTXEFFECTFLAG_USER7)
#define PTFX_EFFECTINST_DONT_REMOVE							(PTXEFFECTFLAG_USER8)
#define PTFX_EFFECTINST_RENDER_ONLY_FIRST_PERSON_VIEW		(PTXEFFECTFLAG_USER9)
#define PTFX_EFFECTINST_RENDER_ONLY_NON_FIRST_PERSON_VIEW	(PTXEFFECTFLAG_USER10)
#define PTFX_EFFECTINST_VEHICLE_WHEEL_ATTACHED_TRAILING		(PTXEFFECTFLAG_USER11) //TODO: This flag should be removed and replaced with a vfx system enum


enum ptfxGameCustomFlags
{
	PTFX_CONTAINS_REFRACTION				= 0,
	PTFX_WATERSURFACE_RENDER_ABOVE_WATER	= 1,
	PTFX_WATERSURFACE_RENDER_BELOW_WATER	= 2,
	PTFX_DRAW_SHADOW_CULLED					= 3,
	PTFX_IS_VEHICLE_INTERIOR				= 4,
};

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
		
		u8 m_runtimeFlags;
		
		void* m_pLastEvoList_NOTUSED;
		void* m_pWindBehaviour;
		u8 pad1[8];
	};
}