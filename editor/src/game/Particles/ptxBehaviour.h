#pragma once
#include <map>

#include "ImGui/imgui.h"
#include "common/types.h"
#include "ptxKeyframeProp.h"

#include "ptxUiUniqueNums.h"


namespace rage
{
	struct ptxBehaviour
	{
		virtual ~ptxBehaviour() {}

		virtual const char* GetName() = 0;
		virtual float GetOrder() = 0;
		virtual void SetDefaultData() {}
		virtual u32 GetTypeFilter() { return 3; }
		virtual bool NeedsToInit() = 0;
		virtual bool NeedsToUpdate() = 0;
		virtual bool NeedsToUpdateFinalize() = 0;
		virtual bool NeedsToDraw() = 0;
		virtual bool NeedsToRelease() = 0;
		virtual bool UpdateOnMainThreadOnly() { return false; }
		virtual void InitPoint(void*) {}
		virtual void UpdatePoint(void*) {}
		virtual void UpdateFinalize(void*) {}

		virtual void DrawPoints(u64 , u64, u64, u64, u64)  {}
		virtual void ReleasePoint(void*) {}
		virtual void SetKeyframeDefns() {}
		virtual void SetEvolvedKeyframeDefns(void*) {}

		//{
		//	static std::map<u32, const char*> names
		//	{
		//		{ rage::joaat("ptxd_Model") ,"ptxd_Model"},
		//		{ rage::joaat("ptxu_Acceleration") ,"ptxu_Acceleration"},
		//		{ rage::joaat("ptxd_Sprite"), "ptxd_Sprite"},
		//		{ rage::joaat("ptxd_Trail"), "ptxd_Trail"},
		//		{ rage::joaat("ptxu_Age"), "ptxu_Age"},
		//		{ rage::joaat("ptxu_AnimateTexture"), "ptxu_AnimateTexture"},
		//		{ rage::joaat("ptxu_Attractor"), "ptxu_Attractor"},
		//		{ rage::joaat("ptxu_Collision"), "ptxu_Collision"},
		//		{ rage::joaat("ptxu_Colour"), "ptxu_Colour"},
		//		{ rage::joaat("ptxu_MatrixWeight"), "ptxu_MatrixWeight"},
		//		{ rage::joaat("ptxu_Noise"), "ptxu_Noise"},
		//		{ rage::joaat("ptxu_Pause"), "ptxu_Pause"},
		//		{ rage::joaat("ptxu_Rotation"), "ptxu_Rotation"},
		//		{ rage::joaat("ptxu_Size"), "ptxu_Size"},
		//		{ rage::joaat("ptxu_Velocity"), "ptxu_Velocity"},
		//		{ rage::joaat("ptxu_Wind"), "ptxu_Wind"},
		//		{ rage::joaat("ptxu_Decal"), "ptxu_Decal"},
		//		{ rage::joaat("ptxu_DecalPool"), "ptxu_DecalPool"},
		//		{ rage::joaat("ptxu_FogVolume"), "ptxu_FogVolume"},
		//		{ rage::joaat("ptxu_Light"), "ptxu_Light"},
		//		{ rage::joaat("ptxu_Liquid"), "ptxu_Liquid"},
		//		{ rage::joaat("ptxu_River"), "ptxu_River"},
		//		{ rage::joaat("ptxu_ZCull"), "ptxu_ZCull"},
		//		{ rage::joaat("ptxu_Dampening"), "ptxu_Dampening"}
		//	}
		//}

		enum class DerivedType : u32
		{
			ptxd_Model            = rage::joaat("ptxd_Model"),
			ptxu_Acceleration     = rage::joaat("ptxu_Acceleration"),
			ptxd_Sprite	          = rage::joaat("ptxd_Sprite"),
			ptxd_Trail            = rage::joaat("ptxd_Trail"),
			ptxu_Age              = rage::joaat("ptxu_Age"),
			ptxu_AnimateTexture   = rage::joaat("ptxu_AnimateTexture"),
			ptxu_Attractor        = rage::joaat("ptxu_Attractor"),
			ptxu_Collision        = rage::joaat("ptxu_Collision"),
			ptxu_Colour           = rage::joaat("ptxu_Colour"),
			ptxu_MatrixWeight     = rage::joaat("ptxu_MatrixWeight"),
			ptxu_Noise            = rage::joaat("ptxu_Noise"),
			ptxu_Pause            = rage::joaat("ptxu_Pause"),
			ptxu_Rotation         = rage::joaat("ptxu_Rotation"),
			ptxu_Size             = rage::joaat("ptxu_Size"),
			ptxu_Velocity         = rage::joaat("ptxu_Velocity"),
			ptxu_Wind             = rage::joaat("ptxu_Wind"),
			ptxu_Decal            = rage::joaat("ptxu_Decal"),
			ptxu_DecalPool        = rage::joaat("ptxu_DecalPool"),
			ptxu_FogVolume        = rage::joaat("ptxu_FogVolume"),
			ptxu_Light            = rage::joaat("ptxu_Light"),
			ptxu_Liquid           = rage::joaat("ptxu_Liquid"),
			ptxu_River            = rage::joaat("ptxu_River"),
			ptxu_ZCull            = rage::joaat("ptxu_ZCull"),
			ptxu_Dampening        = rage::joaat("ptxu_Dampening"),
		};

		void UiWidgets(size_t EntryIdx);


		u32 m_hashName;
		u8 m_pad1[4];
		ptxKeyframePropList m_keyframePropList;			
		u32 m_accumCPUUpdateTicks;						
		u8 m_pad2[12];
	};

	struct ptxd_Model : public ptxBehaviour
	{
		virtual const char* GetName() override { return "ptxd_Model"; }

		u32 m_colourControlShaderId;
		float m_cameraShrink;
		float m_shadowCastIntensity;
		bool m_disableDraw;
		u8 pad[3];
	};

	struct ptxu_Acceleration : public ptxBehaviour
	{
		virtual const char* GetName() override { return "ptxu_Acceleration"; }

		ptxKeyframeProp m_xyzMinKFP;
		ptxKeyframeProp m_xyzMaxKFP;
		float* m_pGlobalData_NOTUSED;
		int m_referenceSpace;
		bool m_isAffectedByZoom;		
		bool m_enableGravity;			
		u8 pad[6];
	};

	struct ptxd_Sprite : public ptxBehaviour
	{
		virtual const char* GetName() override { return "ptxd_Sprite"; }

		Vec3V m_alignAxis;
		int m_alignmentMode;

		float m_flipChanceU;
		float m_flipChanceV;

		float m_nearClipDist;
		float m_farClipDist;

		float m_projectionDepth;
		float m_shadowCastIntensity;

		bool m_isScreenSpace;
		bool m_isHiRes;
		bool m_nearClip;
		bool m_farClip;
		bool m_uvClip;

		bool m_disableDraw;

		u8 pad[14];
	};

	struct ptxd_Trail : public ptxBehaviour
	{
		virtual const char* GetName() override { return "ptxd_Trail"; }

		ptxKeyframeProp m_texInfoKFP;

		Vec3V m_alignAxis;

		int m_alignmentMode;

		int m_tessellationU;
		int m_tessellationV;

		float m_smoothnessX;
		float m_smoothnessY;

		float m_projectionDepth;
		float m_shadowCastIntensity;

		bool m_flipU;
		bool m_flipV;
		bool m_wrapTextureOverParticleLife;
		bool m_disableDraw;
	};

	struct ptxu_Age : public ptxBehaviour
	{
		virtual const char* GetName() override { return "ptxu_Age"; }
	};

	struct ptxu_AnimateTexture : public ptxBehaviour
	{
		virtual const char* GetName() override { return "ptxu_AnimateTexture"; }

		ptxKeyframeProp m_animRateKFP;

		int m_keyframeMode;

		int m_lastFrameId;
		int m_loopMode;

		bool m_isRandomised;
		bool m_isScaledOverParticleLife;
		bool m_isHeldOnLastFrame;
		bool m_doFrameBlending;
	};

	struct ptxu_Attractor : public ptxBehaviour
	{
		virtual const char* GetName() override { return "ptxu_Attractor"; }

		ptxKeyframeProp m_strengthKFP;
	};

	struct ptxu_Collision : public ptxBehaviour
	{
		virtual const char* GetName() override { return "ptxu_Collision"; }

		ptxKeyframeProp m_bouncinessKFP;
		ptxKeyframeProp m_bounceDirVarKFP;

		// non keyframe data
		float m_radiusMult;
		float m_restSpeed;
		int m_colnChance;
		int m_killChance;

		// non parsable data
		bool m_debugDraw;

		u8 m_pad[3];

		float m_overrideMinRadius;
		u8 m_pad2[8];
	};

	struct ptxu_Colour : public ptxBehaviour
	{
		virtual const char* GetName() override { return "ptxu_Colour"; }

		ptxKeyframeProp m_rgbaMinKFP;
		ptxKeyframeProp m_rgbaMaxKFP;
		ptxKeyframeProp m_emissiveIntensityKFP;

		// non keyframe data
		int m_keyframeMode;

		bool m_rgbaMaxEnable;
		bool m_rgbaProportional;
		bool m_rgbCanTint;

		u8 m_pad[9];
	};

	struct ptxu_MatrixWeight : public ptxBehaviour
	{
		virtual const char* GetName() override { return "ptxu_MatrixWeight"; }

		ptxKeyframeProp m_mtxWeightKFP;
		
		int m_referenceSpace;
		u8 m_pad[12];
	};

	struct ptxu_Noise : public ptxBehaviour
	{
		virtual const char* GetName() override { return "ptxu_Noise"; }

		ptxKeyframeProp m_posNoiseMinKFP;
		ptxKeyframeProp m_posNoiseMaxKFP;
		ptxKeyframeProp m_velNoiseMinKFP;
		ptxKeyframeProp m_velNoiseMaxKFP;

		int m_referenceSpace;
		bool m_keepConstantSpeed;

		u8 m_pad[11];
	};

	struct ptxu_Pause : public ptxBehaviour
	{
		virtual const char* GetName() override { return "ptxu_Pause"; }

		float m_pauseLifeRatio;
		float m_unpauseTime;
		float m_unpauseEffectDist;
		float m_unpauseCamDist;
	};

	struct ptxu_Rotation : public ptxBehaviour
	{
		virtual const char* GetName() override { return "ptxu_Rotation"; }

		ptxKeyframeProp m_initialAngleMinKFP;
		ptxKeyframeProp m_initialAngleMaxKFP;
		ptxKeyframeProp m_angleMinKFP;
		ptxKeyframeProp m_angleMaxKFP;

		int m_initRotationMode;
		int m_updateRotationMode;

		bool m_accumulateAngle;
		bool m_rotateAngleAxes;
		bool m_rotateInitAngleAxes;

		char m_pad[1];

		float m_speedFadeThresh;
	};

	struct ptxu_Size : public ptxBehaviour
	{
		virtual const char* GetName() override { return "ptxu_Size"; }

		ptxKeyframeProp m_whdMinKFP;
		ptxKeyframeProp m_whdMaxKFP;
		ptxKeyframeProp m_tblrScalarKFP;
		ptxKeyframeProp m_tblrVelScalarKFP;	

		int m_keyframeMode;
		bool m_isProportional;				

		u8 m_pad[11];
	};

	struct ptxu_Velocity : public ptxBehaviour
	{
		virtual const char* GetName() override { return "ptxu_Velocity"; }
	};

	struct ptxu_Wind : public ptxBehaviour
	{
		virtual const char* GetName() override { return "ptxu_Wind"; }

		ptxKeyframeProp m_influenceKFP;

		void* m_pGlobalData_NOTUSED;
		void* m_pWindEval_NOTUSED;

		float m_highLodRange;
		float m_lowLodRange;

		int	m_highLodDisturbanceMode;
		int	m_lowLodDisturbanceMode;
		bool m_ignoreMtxWeight;

		u8 m_pad[7];
	};

	struct ptxu_Decal : public ptxBehaviour
	{
		virtual const char* GetName() override { return "ptxu_Decal"; }

		ptxKeyframeProp m_dimensionsKFP;
		ptxKeyframeProp m_alphaKFP;

		int	m_decalId;
		float m_velocityThresh;
		float m_totalLife;
		float m_fadeInTime;
		float m_uvMultStart;
		float m_uvMultEnd;
		float m_uvMultTime;
		float m_duplicateRejectDist;
		bool m_flipU;
		bool m_flipV;
		bool m_proportionalSize;
		bool m_useComplexColn;
		float m_projectionDepth;
		float m_distanceScale;
		bool m_isDirectional;

		u8 m_pad[3];
	};

	struct ptxu_DecalPool : public ptxBehaviour
	{
		virtual const char* GetName() override { return "ptxu_DecalPool"; }

		float m_velocityThresh;
		int m_liquidType;
		int m_decalId;
		float m_startSize;
		float m_endSize;
		float m_growthRate;
		
		u8 pad[8];
	};

	struct ptxu_FogVolume : public ptxBehaviour
	{
		virtual const char* GetName() override { return "ptxu_FogVolume"; }

		ptxKeyframeProp m_rgbTintMinKFP;
		ptxKeyframeProp m_rgbTintMaxKFP;
		ptxKeyframeProp m_densityRangeKFP;
		ptxKeyframeProp m_scaleMinKFP;
		ptxKeyframeProp m_scaleMaxKFP;
		ptxKeyframeProp m_rotationMinKFP;
		ptxKeyframeProp m_rotationMaxKFP;

		float m_falloff;
		float m_hdrMult;
		int m_lightingType;
		bool m_colourTintFromParticle;
		bool m_sortWithParticles;
		bool m_useGroundFogColour;
		bool m_useEffectEvoValues;
	};

	struct ptxu_Light : public ptxBehaviour
	{
		virtual const char* GetName() override { return "ptxu_Light"; }

		//  (light specific)
		ptxKeyframeProp m_rgbMinKFP;
		ptxKeyframeProp m_rgbMaxKFP;
		ptxKeyframeProp m_intensityKFP;
		ptxKeyframeProp m_rangeKFP;

		// (corona specific)
		ptxKeyframeProp m_coronaRgbMinKFP;
		ptxKeyframeProp m_coronaRgbMaxKFP;
		ptxKeyframeProp m_coronaIntensityKFP;
		ptxKeyframeProp m_coronaSizeKFP;
		ptxKeyframeProp m_coronaFlareKFP;

		float m_coronaZBias;
		bool m_coronaUseLightCol;

		bool m_colourFromParticle;
		bool m_colourPerFrame;
		bool m_intensityPerFrame;
		bool m_rangePerFrame;
		bool m_castsShadows;
		bool m_coronaNotInReflection;
		bool m_coronaOnlyInReflection;

		int m_lightType;
	};
	
	struct ptxu_Liquid : public ptxBehaviour
	{
		virtual const char* GetName() override { return "ptxu_Liquid"; }

		float m_velocityThresh;
		int m_liquidType;
		float m_poolStartSize;
		float m_poolEndSize;
		float m_poolGrowthRate;
		float m_trailWidthMin;
		float m_trailWidthMax;
		u8 pad[4];
	};

	struct ptxu_River : public ptxBehaviour
	{
		virtual const char* GetName() override { return "ptxu_River"; }

		float m_velocityMult;
		float m_influence;
		u8 pad[8];
	};

	struct ptxu_ZCull : public ptxBehaviour
	{
		virtual const char* GetName() override { return "ptxu_ZCull"; }

		ptxKeyframeProp m_heightKFP;
		ptxKeyframeProp m_fadeDistKFP;

		float* m_pGlobalData_NOTUSED;

		int m_mode;
		int m_referenceSpace;

		u8 m_pad[4];
	};

	struct ptxu_Dampening : public ptxBehaviour
	{
		virtual const char* GetName() override { return "ptxu_Dampening"; }

		ptxKeyframeProp m_xyzMinKFP;
		ptxKeyframeProp m_xyzMaxKFP;
		
		void* m_pGlobalData_NOTUSED;

		int m_referenceSpace;
		bool m_enableAirResistance;	
		
		u8 pad[7];
	};
}
