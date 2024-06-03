#pragma once

#include <iostream>

#include "memory/address.h"
#include "rage/atl/atArray.h"
#include "rage/atl/color32.h"

struct CFlareFX
{
	float	m_fDistFromLight;
	float	m_fSize;
	float	m_fWidthRotate;
	float	m_fScrollSpeed;
	float	m_fCurrentScroll;
	float	m_fDistanceInnerOffset;
	float	m_fIntensityScale;
	float	m_fIntensityFade;
	float	m_fAnimorphicScaleFactorU;
	float	m_fAnimorphicScaleFactorV;
	Color32 m_color;
	float	m_fTextureColorDesaturate;
	float	m_fGradientMultiplier;
	u8		m_nTexture;
	u8		m_nSubGroup;
	bool	m_bAnimorphicBehavesLikeArtefact;
	bool	m_bAlignRotationToSun;
	float	m_fPositionOffsetU;

	CFlareFX()
		: m_fDistFromLight(0.0f)
		, m_fSize(0.1f)
		, m_fWidthRotate(0.01f)
		, m_fScrollSpeed(0.15f)
		, m_fCurrentScroll(0.0f)
		, m_fDistanceInnerOffset(0.01f)
		, m_fIntensityScale(100.f)
		, m_fIntensityFade(1.0f)
		, m_fAnimorphicScaleFactorU(0.0f)
		, m_fAnimorphicScaleFactorV(0.0f)
		, m_color(Color32(1.0f,1.0f,1.0f))
		, m_fTextureColorDesaturate(0.0f)
		, m_fGradientMultiplier(1.0f)
		, m_nTexture(0)
		, m_nSubGroup(1)
		, m_bAnimorphicBehavesLikeArtefact(false)
		, m_fPositionOffsetU(0.0f)
	{ }
};


struct CLensFlareSettings
{
	float m_fSunVisibilityFactor;
	s32	  m_iSunVisibilityAlphaClip;
	float m_fSunFogFactor;
	float m_fChromaticTexUSize;
	float m_fExposureScaleFactor;
	float m_fExposureIntensityFactor;
	float m_fExposureRotationFactor;
	float m_fChromaticFadeFactor;
	float m_fArtefactDistanceFadeFactor;
	float m_fArtefactRotationFactor;
	float m_fArtefactScaleFactor;
	float m_fArtefactGradientFactor;
	float m_fCoronaDistanceAdditionalSize;
	float m_fMinExposureScale;
	float m_fMaxExposureScale;
	float m_fMinExposureIntensity;
	float m_fMaxExposureIntensity;
	
	u8 pad[4];
	
	atArray<CFlareFX> m_arrFlareFX;
};



class LensFlareHandler
{
	class gCLensFlare
	{
		u8 pad01[0x78];
	public:
		CLensFlareSettings m_Settings[3];
		u8 m_index;
	private:
		u8 pad02[7];
	};

public:

	gCLensFlare* m_CLensFlare = nullptr;

	LensFlareHandler();

};

