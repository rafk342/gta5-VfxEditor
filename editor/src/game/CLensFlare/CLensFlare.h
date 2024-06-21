#pragma once

#include <iostream>
#include <map>
#include <directxmath.h>
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")


#include "memory/address.h"
#include "memory/hook.h"
#include "rage/atl/atArray.h"
#include "rage/atl/color32.h"

#include "rage/math/vec.h"
#include "rage/math/vecv.h"

#include "overlayRender/DrawList.h"


enum FlareFxTextureType_e : u8
{						 // SubGroups :
	AnimorphicType	= 0, // 0
	ArtefactType	= 1, // 1-8 here
	ChromaticType	= 2, // 0
	CoronaType		= 3, //	0
};

enum LensFlareFile_e
{
	lensflare_m = 0,
	lensflare_f = 1,
	lensflare_t = 2,
};


struct CFlareFX
{
	float	m_fDistFromLight = 0.2f;
	float	m_fSize = 0.2f;
	float	m_fWidthRotate = 0.0f;
	float	m_fScrollSpeed = 0.0f;
	float	m_fCurrentScroll = 0.0f;
	float	m_fDistanceInnerOffset = 0.0f;
	float	m_fIntensityScale = 100.f;
	float	m_fIntensityFade = 0.0f;
	float	m_fAnimorphicScaleFactorU = 0.0f;
	float	m_fAnimorphicScaleFactorV = 0.0f;
	Color32 m_color = { 1.0f,1.0f,1.0f };
	float	m_fTextureColorDesaturate = 0.0f;
	float	m_fGradientMultiplier = 0.0f;
	u8		m_nTexture = 0;
	u8		m_nSubGroup = 0;
	bool	m_bAnimorphicBehavesLikeArtefact = false;
	bool	m_bAlignRotationToSun = false;
	float	m_fPositionOffsetU = 0.0f;

	CFlareFX() = default;
	CFlareFX(FlareFxTextureType_e type);
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
private:
	u8 pad[4];
public:
	atArray<CFlareFX> m_arrFlareFX;
};



class LensFlareHandler
{
	class gCLensFlare
	{
		u8 pad01[0x78];
	public:
		CLensFlareSettings m_Settings[3];
		u8 m_ActiveIndex;
	private:
		u8 pad02[7];
	};
	static void n_RenderFlareFx(u64 arg1, u64 arg2, u64 arg3, u64 arg4, float* vPos);

	static LensFlareHandler* self;
	static rage::Vec3V	sm_StartPoint;
	static rage::Vec3V	sm_EndPoint;

public:
	
	gCLensFlare* m_CLensFlare = nullptr;
	
	void Update();

	LensFlareHandler();
	~LensFlareHandler();

	const char* GetFileNameAtIndex(size_t idx);
	const char* GetTextureTypeName(u8 NumTexture);
};

