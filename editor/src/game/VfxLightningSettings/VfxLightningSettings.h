#pragma once

#include "memory/address.h"
#include "common/types.h"
#include "CloudSettings/CloudSettings.h"
#include "atl/color32.h"

namespace
{
	class lightningTimeCycleMods
	{
		u8 pad02[8];
	public:
		u32 tcLightningDirectionalBurst;
		u32 tcLightningCloudBurst;
		u32 tcLightningStrikeOnly;
		u32 tcLightningDirectionalBurstWithStrike;
	};


	class DirectionalBurstSettings
	{
		u8 pad03[8];
	public:
		float BurstIntensityMin;
		float BurstIntensityMax;
		float BurstDurationMin;
		float BurstDurationMax;
		int   BurstSeqCount;
		float RootOrbitXVariance;
		float RootOrbitYVarianceMin;
		float RootOrbitYVarianceMax;
		float BurstSeqOrbitXVariance;
		float BurstSeqOrbitYVariance;

	};


	class CloudBurstCommonSettings
	{
		u8 pad[8];
	public:
		float BurstDurationMin;
		float BurstDurationMax;
		int   BurstSeqCount;
		float RootOrbitXVariance;
		float RootOrbitYVarianceMin;
		float RootOrbitYVarianceMax;
		float LocalOrbitXVariance;
		float LocalOrbitYVariance;
		float BurstSeqOrbitXVariance;
		float BurstSeqOrbitYVariance;
		float DelayMin;
		float DelayMax;
		float SizeMin;
		float SizeMax;
	};


	class CloudBurstSettings
	{
		u8 pad04[8];
	public:
		float BurstIntensityMin;
		float BurstIntensityMax;
		float LightSourceExponentFalloff;
		float LightDeltaPos;
		float LightDistance;
		Color32 LightColor;

		CloudBurstCommonSettings m_CloudBurstCommonSettings;
	};


	class ZigZagSplitPoint
	{
		u8 pad[8];
	public:
		float	m_FractionMin;
		float	m_FractionMax;
		float	m_DeviationDecay;
		float	m_DeviationRightVariance;
	};


	class ForkPoint
	{
		u8 pad[8];
	public:
		float	m_DeviationRightVariance;
		float	m_DeviationForwardMin;
		float	m_DeviationForwardMax;
		float	m_LengthMin;
		float	m_LengthMax;
		float	m_LengthDecay;
	};


	class LightningKeyFrames
	{
		u8 pad[8];
	public:
		ptxKeyframe LightningMainIntensity;
		ptxKeyframe LightningBranchIntensity;
	};


	class StrikeVariationSettings
	{
		u8 pad[8];
	public:
		float	DurationMin;
		float	DurationMax;
		float	AnimationTimeMin;
		float	AnimationTimeMax;
		float	EndPointOffsetXVariance;

		u8		SubdivisionPatternMask;

		ZigZagSplitPoint mZigZagSplitPoint;
		ZigZagSplitPoint mForkZigZagSplitPoint;

		ForkPoint mForkPoint;

		LightningKeyFrames mKeyFrameData;
	};


	class StrikeSettings
	{
		u8 pad[8];
	public:
		float	IntensityMin;
		float	IntensityMax;
		float	IntensityMinClamp;

		float	WidthMin;
		float	WidthMax;
		float	WidthMinClamp;

		float	IntensityFlickerMin;
		float	IntensityFlickerMax;
		float	IntensityFlickerFrequency;

		float	IntensityLevelDecayMin;
		float	IntensityLevelDecayMax;
		float	WidthLevelDecayMin;
		float	WidthLevelDecayMax;

		float	NoiseTexScale;
		float	NoiseAmplitude;
		float	NoiseAmpMinDistLerp;
		float	NoiseAmpMaxDistLerp;

		u8		SubdivisionCount;

		float	OrbitDirXVariance;
		float	OrbitDirYVarianceMin;
		float	OrbitDirYVarianceMax;
		float	MaxDistanceForExposureReset;
		float	AngularWidthFactor;
		float	MaxHeightForStrike;
		float	CoronaIntensityMult;
		float	BaseCoronaSize;
		Color32	BaseColor;
		Color32	FogColor;
		float	CloudLightIntensityMult;
		float	CloudLightDeltaPos;
		float	CloudLightRadius;
		float	CloudLightFallOffExponent;
		float	MaxDistanceForBurst;
		float	BurstThresholdIntensity;
		float	LightningFadeWidth;
		float	LightningFadeWidthFalloffExp;

		StrikeVariationSettings m_Variations[3];
		CloudBurstCommonSettings m_CloudBurstCommonSettings;
	};


	class VfxLightningSettings
	{
		u8 pad01[8];
	public:
		int lightningOccurranceChance;
		float lightningShakeIntensity;

		lightningTimeCycleMods		m_lightningTimeCycleMods;
		DirectionalBurstSettings	m_DirectionalBurstSettings;
		CloudBurstSettings			m_CloudBurstSettings;
		StrikeSettings				m_StrikeSettings;
	};
}


class VfxLightningOwner
{
public:
	VfxLightningOwner();
	VfxLightningSettings* mVfxLightningSettings;
};


