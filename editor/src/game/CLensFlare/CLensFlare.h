#pragma once

#include <iostream>
#include <map>
//#include <directxmath.h>
//#include <d3dcompiler.h>
//#pragma comment(lib, "d3dcompiler.lib")

#include "compiler/compiler.h"


#include "ImGui/imgui.h"

#include "memory/address.h"
#include "memory/hook.h"
#include "rage/atl/atArray.h"
#include "rage/atl/color32.h"

#include "rage/math/vec.h"
#include "rage/math/vecv.h"



enum FlareFxTextureType_e : u8
{						 // SubGroups :
	AnimorphicFx = 0, // 0
	ArtefactFx	 = 1, // 1-8 here
	ChromaticFx	 = 2, // 0
	CoronaFx	 = 3, // 0
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
	Color32 m_color = { 1.0f, 1.0f, 1.0f, 1.0f };
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


class LensFlareHandler;
class LensFlares_DebugOverlay 
{
	friend class LensFlareHandler;
	using self_t = LensFlares_DebugOverlay;
	static self_t* self;
	
	LensFlareHandler*	m_Handler = nullptr;
	rage::Vec3V			m_StartPoint{};
	rage::Vec3V			m_EndPoint{};
	
	struct CircleDrawData
	{
		ImVec2 pos;
		float rotate;
		Color32 col;
		ImVec2 scale;
	};
	
	std::vector<CircleDrawData> m_CirclesDrawData;
	
	//std::map<CFlareFX*, rage::ScalarV> m_DistantsFromLight;

	static void hk_RenderFlareFx(u64 arg1, u64 arg2, u64 arg3, float fIntensity, float* vPos);
	
	Color32 line_color = { 255, 0, 0, 187 };
	float line_thickness = 1;
	
public:
	
	float scale = 10;
	Color32 scalar_color = { 200, 200, 200, 187};

	LensFlares_DebugOverlay(LensFlareHandler* handler);
	~LensFlares_DebugOverlay();
	
	void EndFrame();
};


class LensFlareHandler
{
	friend class LensFlares_DebugOverlay;
	using self_t = LensFlareHandler;
	static self_t* self;


	class CLensFlares
	{
	public:
		u8 pad001[0x40];
		float m_fExposureScale;
		float m_fExposureIntensity;
		float m_fExposureRotation;
		u8 pad002[32];
		float m_SunVisibility;
		u8 pad003[8];
		CLensFlareSettings m_Settings[3];
		u8 m_ActiveSettingsIndex;
		u8 pad004[31];
		u8 m_DestinationSettingsIndex;
	};

	bool m_ShowDebugOverlay = false;

public:
	
	CLensFlares*			gCLensFlares = nullptr;
	LensFlares_DebugOverlay	m_DebugOverlay;
	
	LensFlareHandler();
	~LensFlareHandler();

	void ChangeSettings(u8 index)
	{
		if (gCLensFlares)
		{
			gCLensFlares->m_ActiveSettingsIndex = index;
			gCLensFlares->m_DestinationSettingsIndex = index;
		}
	}

	const char* GetFileNameAtIndex(size_t idx);
	const char* GetTextureTypeName(u8 NumTexture);
	void SetDebugOverlayVisibility(bool show);
	void SortFlaresByDistance(atArray<CFlareFX>& arr);
	static void EndFrame();
};

