#pragma once

#include "rage/atl/atHashMap.h"
#include "rage/math/vec.h"

#include "rage/paging/ref.h"
#include "rage/paging/base.h"

#include "ptxTimeline.h"
#include "ptxKeyframeProp.h"


namespace rage
{
	class ptxFxList;

	struct alignas(16) ptxEffectRule : public pgBaseRefCounted
	{
		float m_fileVersion;								// the effect rule's file version
		u8 pad0[4];

		ConstString m_name;									// the name of the effect rule
		ptxFxList* m_Self_FxList;								// pointer to the effect list where we have loaded from

		// timeline and evolution data
		ptxTimeLine m_timeline;							// the effect rule's timeline
		datOwner<ptxEvolutionList> m_pEvolutionList;		// pointer to the list of evolutions on the effect

		// effect setup
		int m_numLoops;										// the number of times the timeline should loop back to the start
		bool m_sortEventsByDistance;						// whether the timeline events are sorted by distance to the camera or not
		u8 m_drawListId;									// the index of the draw list that this effect belongs to
		bool m_isShortLived;								// whether the effect is short lived (so it's ambient scale lookup can be optimised)
		bool m_hasNoShadows;								// whether the effect has no shadows
		u8 pad1[8];

		rage::Vector4 m_vRandOffsetPos;						// random offset position applied to the effect every loop
		float m_preUpdateTime;								// the amount of time that the effect should be updated for when first started
		float m_preUpdateTimeInterval;						// the interval for each update for the effect for when first started
		float m_durationMin;								// minimum duration of the effect
		float m_durationMax;								// maximum duration of the effect
		float m_playbackRateScalarMin;						// minimum playback rate scalar of the effect
		float m_playbackRateScalarMax;						// maximum playback rate scalar of the effect

		u8 m_viewportCullingMode;							// the viewport culling mode
		bool m_renderWhenViewportCulled;					// whether to render this effect when it is viewport culled
		bool m_updateWhenViewportCulled;					// whether to update this effect when it is viewport culled	
		bool m_emitWhenViewportCulled;						// whether this effect emits particles when it is viewport culled
		u8 m_distanceCullingMode;							// the distance culling mode
		bool m_renderWhenDistanceCulled;					// whether to render this effect when it is distance culled	
		bool m_updateWhenDistanceCulled;					// whether to update this effect when it is distance culled		
		bool m_emitWhenDistanceCulled;						// whether this effect emits particles when it is distance culled
		rage::Vector4 m_viewportCullingSphereOffset;		// the offset of the viewport culling sphere
		float m_viewportCullingSphereRadius;				// the radius of the viewport culling sphere
		float m_distanceCullingFadeDist;					// the distance at which the effect starts to fade out
		float m_distanceCullingCullDist;					// the distance at which the effect is totally culled	

		float m_lodEvoDistMin;								// the distance at which the lod evolution kicks in 
		float m_lodEvoDistMax;								// the distance at which the lod evolution is at max

		float m_colnRange;									// the range to query for collision
		float m_colnProbeDist;								// the distance to probe for collision	

		u8 m_colnType;										// the type of collision that this effect has	
		bool m_shareEntityColn;								// whether the collision is registered on the entity (so it can be shared with other effects)
		bool m_onlyUseBVHColn;								// whether the collision only uses bvh bounds

		u8 m_gameFlags;										// per game flags that can be tuned via the editor

		// keyframes
		ptxKeyframeProp m_colourTintMinKFP;					// keyframeable minimum colour tint of the effect
		ptxKeyframeProp m_colourTintMaxKFP;					// keyframeable maximum colour tint of the effect
		ptxKeyframeProp m_zoomScalarKFP;					// keyframeable size scalar of the effect
		ptxKeyframeProp m_dataSphereKFP;					// keyframeable data sphere attached to the effect
		ptxKeyframeProp m_dataCapsuleKFP; 					// keyframeable capsule sphere attached to the effect  
		ptxKeyframePropList m_keyframePropList;				// list of keyframeable properties

		bool m_colourTintMaxEnable;							// whether the max colour tint keyframe is enabled
		bool m_useDataVolume;								// whether this effect uses a data volume
		u8 m_dataVolumeType;								// the type of data volume

		u8 m_pad3[1];
		u32 m_numActiveInstances;							// number of active instances of this effect rule

		float m_zoomableComponentScalar;					// editor only zoom level used when rescaling zoomable components (MN - needs renamed to m_zoomableComponentScalar) 

		u8 m_pad5[20];
	};


}