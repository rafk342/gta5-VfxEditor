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

#include "CLensFlareXmlParser.h"

enum LensFlareFile_e
{
	lensflare_m = 0,
	lensflare_f = 1,
	lensflare_t = 2,
};

enum FlareFxTextureType_e : u8
{					// SubGroups :
	AnimorphicFx = 0, // 0
	ArtefactFx	 = 1, // 1-8 
	ChromaticFx	 = 2, // 0
	CoronaFx	 = 3, // 0
};

struct CFlareFX
{
	float	m_fDistFromLight = 0.3000;
	float	m_fSize = 0.0500;
	float	m_fWidthRotate;
	float	m_fScrollSpeed;
	float	m_fCurrentScroll;
	float	m_fDistanceInnerOffset;
	float	m_fIntensityScale;
	float	m_fIntensityFade;
	float	m_fAnimorphicScaleFactorU = 0.0000;
	float	m_fAnimorphicScaleFactorV = 0.0000;
	Color32 m_color = { 1.0f, 1.0f, 1.0f, 1.0f };
	float	m_fTextureColorDesaturate;
	float	m_fGradientMultiplier;
	u8		m_nTexture;
	u8		m_nSubGroup;
	bool	m_bAnimorphicBehavesLikeArtefact;
	bool	m_bAlignRotationToSun;
	float	m_fPositionOffsetU;

	CFlareFX();
	void ResetToDefaultByType(FlareFxTextureType_e type);
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
private:
	friend class LensFlareHandler;
	using self_t = LensFlares_DebugOverlay;
	static inline self_t* self = nullptr;

	struct CircleDrawData
	{
		ImVec2	pos;
		float	rotate;
		Color32 col;
		ImVec2	scale;
		float	thickness;
	};

	LensFlareHandler*	p_Handler = nullptr;
	u32*				p_msTime = nullptr;
	rage::Vec3V			m_LightVecStartPoint;
	rage::Vec3V			m_LightVecEndPoint;
	Color32				m_LightVecColor = { 255, 0, 0, 180 };
	float				m_DefaultThickness = 1.0f;
	Color32				m_ScalarColor = { 200, 200, 200, 180 };
	u8					m_OverlayAlpha = 180;

	std::vector<CircleDrawData> m_DrawData;
	
private:
	static void hk_RenderFlareFx(u64 arg1, u64 arg2, u64 arg3, u64 arg4, float* vPos);

public:

	LensFlares_DebugOverlay(LensFlareHandler* handler);
	~LensFlares_DebugOverlay();
	
	void SetThicknessForFlareCircleByIndex(size_t index, float thickness);
	void SetOverlayAlpha(u8 value);
	void DrawOverlay();
	void DrawLightVec();
	void Reset();
};


class LensFlareHandler
{
	friend class LensFlares_DebugOverlay;
	using self_t = LensFlareHandler;
	static inline self_t* self = nullptr;

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

public:
	
	LensFlares_DebugOverlay	m_DebugOverlay;
	CLensFlares* pCLensFlares = nullptr;
	[[msvc::no_unique_address]] CLensFlareXmlParser m_XmlParser;

private:

	bool m_ShowDebugOverlay = false;
	bool m_ShowLightVec = false;

public:
	
	LensFlareHandler();
	~LensFlareHandler();

	void ChangeSettings(u8 index);
	const char* GetFileNameAtIndex(size_t idx);
	const char* GetTextureTypeName(u8 NumTexture);
	void SetDebugOverlayVisibility(bool show);
	void SetLightVecVisibility(bool show);
	void SortFlaresByDistance(atArray<CFlareFX>& arr);
	static void EndFrame();
};

