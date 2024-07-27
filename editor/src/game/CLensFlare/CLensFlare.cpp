#include "CLensFlare.h"

#define PI	(3.14159265358979323846264338327950288l)
static gmAddress s_RenderFlareFxAddr;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


rage::ScalarV LengthVec2(rage::Vec3V V)
{
	return std::sqrt(V.X() * V.X() + V.Y() * V.Y());
}

rage::ScalarV LengthVec3(rage::Vec3V V)
{
	return std::sqrt(V.X() * V.X() + V.Y() * V.Y() + V.Z() * V.Z());
}

u32 ConvertToImguiCol32(Color32 color)
{
	return IM_COL32(color.getRedU8(), color.getGreenU8(), color.getBlueU8(), color.getAlphaU8());
}

ImVec2 ConvertToPixelCoordinates(const ImVec2& NormalizedPoint)
{
	ImVec2 WndSz = ImGui::GetMainViewport()->Size;
	return { NormalizedPoint.x * WndSz.x, NormalizedPoint.y * WndSz.y };
}

float GetMainViewportAspectRatio()
{
	ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	if (main_viewport)
	{
		float width = main_viewport->Size.x;
		float height = main_viewport->Size.y;
		return width / height;
	}
	return 1.0f;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//								DebugOverlay
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void(*RenderFlareFxfn)(u64, u64, u64, u64, float*);
void LensFlares_DebugOverlay::hk_RenderFlareFx(u64 arg1, u64 arg2, u64 arg3, u64 arg4, float* SunPos)
{
	using namespace rage;
	RenderFlareFxfn(arg1, arg2, arg3, arg4, SunPos);

	if (!self)
		return;

	if (self->p_Handler->pCLensFlares->m_SunVisibility < 0.01f)
		return;

	self->m_LightVecStartPoint = { SunPos[0], SunPos[1] };
	self->m_ScalarColor.setAlphaU8(self->m_OverlayAlpha);

	Vec3V vFlareDirNorm = Vec3V(0.5f) - self->m_LightVecStartPoint;
	ScalarV fLen = LengthVec2(vFlareDirNorm);
	Vec3V vDist = vFlareDirNorm.Abs() + Vec3V(0.5f);
	vFlareDirNorm /= fLen;

	Vec3V vFlareDir = vFlareDirNorm * LengthVec2(vDist);
	self->m_LightVecEndPoint = self->m_LightVecStartPoint + vFlareDir;

	ScalarV fDistScale = std::min(1.0f, (fLen * 2).Get());
	fDistScale *= fDistScale;

	float fAspectRatio = GetMainViewportAspectRatio();

	auto* lensflares = self->p_Handler->pCLensFlares;
	auto& settings = lensflares->m_Settings[lensflares->m_ActiveSettingsIndex];
	auto& arr = settings.m_arrFlareFX;

	for (auto& flare : arr)
	{
		/*
		*	Position
		*/
		ScalarV fDistFromLightFactor;
		if (flare.m_nTexture == ArtefactFx || (flare.m_nTexture == AnimorphicFx && flare.m_bAnimorphicBehavesLikeArtefact))
		{
			float fDistFromCenterNorm = 2.0f * fLen.Get();
			fDistFromLightFactor = ScalarV(2.0f * fDistFromCenterNorm + flare.m_fDistFromLight * fDistFromCenterNorm);
		}
		else
		{
			fDistFromLightFactor = ScalarV(flare.m_fDistFromLight) * fDistScale;
		}

		Vec3V vFlarePos = (self->m_LightVecStartPoint + (vFlareDir * fDistFromLightFactor));
		if (flare.m_nTexture == ArtefactFx)
		{
			vFlarePos.SetX(vFlarePos.X() + flare.m_fPositionOffsetU);
		}

		/*
		*	Size
		*/
		float fHalfSize;
		if (flare.m_nTexture == ChromaticFx)
		{
			fHalfSize = flare.m_fSize;
		}
		else
		{
			fHalfSize = lensflares->m_fExposureScale * flare.m_fSize;
			if (flare.m_nTexture == ArtefactFx)
			{
				fHalfSize *= settings.m_fArtefactScaleFactor;
			}
			else if (flare.m_nTexture == CoronaFx)
			{
				fHalfSize += fLen.Get() * settings.m_fCoronaDistanceAdditionalSize;
			}
		}

		/*
		*	Scale 
		*/
		float fScaleU = fHalfSize;
		float fScaleV = fHalfSize;
		float fOuterRad;

		switch (static_cast<FlareFxTextureType_e>(flare.m_nTexture))
		{
		case AnimorphicFx:
			fScaleU = fHalfSize + fLen.Get() * flare.m_fAnimorphicScaleFactorU;
			fScaleV = fHalfSize + fLen.Get() * flare.m_fAnimorphicScaleFactorV;
			break;

		case ArtefactFx:
			fScaleU *= 1 + fLen.Get() * flare.m_fAnimorphicScaleFactorU;
			fScaleV *= (fAspectRatio + fLen.Get() * flare.m_fAnimorphicScaleFactorV) / 2;
			break;

		case ChromaticFx:
			fOuterRad = fHalfSize + flare.m_fWidthRotate / 2;
			fScaleU = fOuterRad * 1.0f + fLen.Get() * flare.m_fAnimorphicScaleFactorU;
			fScaleV = (fOuterRad * fAspectRatio + fLen.Get() * flare.m_fAnimorphicScaleFactorV) / 2;
			break;

		case CoronaFx:
			fScaleU = (fHalfSize + fLen.Get() * flare.m_fAnimorphicScaleFactorU) / 2;
			fScaleV = (fHalfSize + fLen.Get() * flare.m_fAnimorphicScaleFactorV) / 2;
			break;

		default:
			break;
		}

		fScaleU *= 1000;
		fScaleV *= 1000;


		/*
		*	Rotate
		*/
		float rotate = (-flare.m_fWidthRotate) * PI;
		if (flare.m_nTexture == ChromaticFx || flare.m_nTexture == ArtefactFx)
		{
			rotate = 0.0f;
		}
		if (flare.m_nTexture == CoronaFx)
		{
			rotate = (*self->p_msTime) * 0.001f * (-flare.m_fWidthRotate);
		}
		

		self->m_DrawData.push_back(CircleDrawData(
		/*pos	*/	ConvertToPixelCoordinates({vFlarePos[0], vFlarePos[1]}),
		/*rotate*/	rotate,
		/*color	*/	flare.m_color * self->m_ScalarColor,
		/*scale	*/	{ fScaleU, fScaleV },
		/*thickness*/ self->m_DefaultThickness
			));

	}
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


LensFlares_DebugOverlay::LensFlares_DebugOverlay(LensFlareHandler* handler) : p_Handler(handler)
{
	self = this;
	s_RenderFlareFxAddr = gmAddress::Scan("48 8B C4 F3 0F 11 58 ?? 4C 89 40 ?? 55");
	p_msTime = s_RenderFlareFxAddr.GetAt(0xbea).GetRef(2).To<u32*>();
	Hook::Create(s_RenderFlareFxAddr, hk_RenderFlareFx, &RenderFlareFxfn, "RenderFlareFx");
}

LensFlares_DebugOverlay::~LensFlares_DebugOverlay()
{
	Hook::Remove(s_RenderFlareFxAddr, "RenderFlareFx");
	p_Handler = nullptr;
	self = nullptr;
}

void LensFlares_DebugOverlay::SetThicknessForFlareCircleByIndex(size_t index, float thickness)
{
	if (m_DrawData.empty() || index >= m_DrawData.size())
		return;

	m_DrawData[index].thickness = thickness;
}

void LensFlares_DebugOverlay::SetOverlayAlpha(u8 value)
{
	m_OverlayAlpha = value;
}

void LensFlares_DebugOverlay::DrawOverlay()
{
	if (p_Handler->pCLensFlares->m_SunVisibility < 0.01f)
		return;

	for (auto& data : m_DrawData)
	{
		ImGui::GetBackgroundDrawList()->AddEllipse(data.pos, data.scale.x, data.scale.y , ConvertToImguiCol32(data.col), data.rotate, 32, data.thickness);
	}
}

void LensFlares_DebugOverlay::DrawLightVec()
{
	if (p_Handler->pCLensFlares->m_SunVisibility < 0.01f)
		return;

	ImVec2 start_p = ConvertToPixelCoordinates({ m_LightVecStartPoint.X(), m_LightVecStartPoint.Y() });
	ImVec2 end_p = ConvertToPixelCoordinates({ m_LightVecEndPoint.X(), m_LightVecEndPoint.Y() });
	m_LightVecColor.setAlphaf(m_ScalarColor.getAlphaf());
	ImGui::GetBackgroundDrawList()->AddLine(start_p, end_p, ConvertToImguiCol32(m_LightVecColor), m_DefaultThickness);
}

void LensFlares_DebugOverlay::Reset()
{
	m_DrawData.clear();
	m_LightVecStartPoint = 0;
	m_LightVecEndPoint = 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////			
//							LensFlareHandler
//							
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


LensFlareHandler::LensFlareHandler()
	: m_DebugOverlay(this)
{
	self = this;
	pCLensFlares = gmAddress::Scan("48 8D 0D ?? ?? ?? ?? 75 ?? 33 D2").GetRef(3).To<CLensFlares*>();
}

LensFlareHandler::~LensFlareHandler()
{
	self = nullptr;
}

void LensFlareHandler::ChangeSettings(u8 index)
{
	if (pCLensFlares)
	{
		pCLensFlares->m_ActiveSettingsIndex = index;
		pCLensFlares->m_DestinationSettingsIndex = index;
	}
}

const char* LensFlareHandler::GetFileNameAtIndex(size_t idx)
{
	static std::map<LensFlareFile_e, const char*> FileNamesMap
	{
		{ lensflare_m, "M" },
		{ lensflare_f, "F" },
		{ lensflare_t, "T" },
	};

	auto fName_e = static_cast<LensFlareFile_e>(idx);
	if (!FileNamesMap.contains(fName_e)) {
		return "fNone";
	} else {
		return FileNamesMap.at(fName_e);
	}
}

const char* LensFlareHandler::GetTextureTypeName(u8 NumTexture)
{
	static std::map<FlareFxTextureType_e, const char*> NamesMap
	{
		{ AnimorphicFx,"AnimorphicFx" },
		{ ArtefactFx, "ArtefactFx" },
		{ ChromaticFx, "ChromaticFx" },
		{ CoronaFx,	 "CoronaFx"	},
	};

	auto name_e = static_cast<FlareFxTextureType_e>(NumTexture);
	if (!NamesMap.contains(name_e)) {
		return "Unknown";
	} else {
		return NamesMap.at(name_e);
	}
}

void LensFlareHandler::EndFrame()
{
	if (!self)
		return;

	if (self->m_ShowDebugOverlay)
	{
		self->m_DebugOverlay.DrawOverlay();
	}
	if (self->m_ShowLightVec)
	{
		self->m_DebugOverlay.DrawLightVec();
	}
	self->m_DebugOverlay.Reset();
}


void LensFlareHandler::SetDebugOverlayVisibility(bool show)
{
	m_ShowDebugOverlay = show;
}

void LensFlareHandler::SetLightVecVisibility(bool show)
{
	m_ShowLightVec = show;
}

void LensFlareHandler::SortFlaresByDistance(atArray<CFlareFX>& arr)
{
	using namespace rage;
	atArray<std::pair<float, CFlareFX*>> TempSortDataArray;

	ScalarV fLen = LengthVec2(Vec3V(0.5f) - Vec3V(0.2f));
	ScalarV fDistScale = std::min(1.0f, (fLen * 2).Get());
	fDistScale *= fDistScale;

	for (auto& flare : arr)
	{
		ScalarV fDistFromLightFactor;
		if (flare.m_nTexture == ArtefactFx || (flare.m_nTexture == AnimorphicFx && flare.m_bAnimorphicBehavesLikeArtefact))
		{
			float fDistFromCenterNorm = 2.0f * fLen.Get();
			fDistFromLightFactor = ScalarV(2.0f * fDistFromCenterNorm + flare.m_fDistFromLight * fDistFromCenterNorm);
		}
		else
		{
			fDistFromLightFactor = ScalarV(flare.m_fDistFromLight) * fDistScale;
		}
		TempSortDataArray.push_back({ fDistFromLightFactor.Get(), &flare });
	}

	std::sort(TempSortDataArray.begin(), TempSortDataArray.end(), [](std::pair<float, CFlareFX*>& left, std::pair<float, CFlareFX*>& right) 
		{ 
			return left.first < right.first;
		});
	
	atArray<CFlareFX> SortedFlaresArray(TempSortDataArray.size());
	for (size_t i = 0; i < TempSortDataArray.size(); i++)
	{
		SortedFlaresArray[i] = (*TempSortDataArray[i].second);
	}

	arr = std::move(SortedFlaresArray);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////			
//					CFlareFX		
//							
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CFlareFX::CFlareFX()
{
	ResetToDefaultByType(AnimorphicFx);
}

void CFlareFX::ResetToDefaultByType(FlareFxTextureType_e type)
{
	m_nTexture = type;

	switch (type)
	{
	case AnimorphicFx:

		m_bAnimorphicBehavesLikeArtefact = false;
		//m_fDistFromLight;
		//m_fSize = 0.1000;
		//m_fAnimorphicScaleFactorU = 0.0000;
		//m_fAnimorphicScaleFactorV = 0.0000;
		m_fWidthRotate = 0.0000;
		m_fIntensityScale = 25.0000;
		//m_color = { 1.0f, 1.0f, 1.0f, 1.0f };

		m_fScrollSpeed = 0.0f;
		m_fCurrentScroll = 0.0f;
		m_fDistanceInnerOffset = 0.0f;
		m_fIntensityFade = 0.0f;
		m_fTextureColorDesaturate = 0.0f;
		m_fGradientMultiplier = 0.0f;
		m_nSubGroup = 0;
		m_bAlignRotationToSun = false;
		m_fPositionOffsetU = 0.0f;

		break;
	case ArtefactFx:
		
		//m_fDistFromLight = -1.7395;
		//m_fSize = 0.05;
		//m_fAnimorphicScaleFactorU = 0.0000;
		//m_fAnimorphicScaleFactorV = 0.0000;
		m_fIntensityScale = 180.0000;
		m_bAlignRotationToSun = false;
		//m_color = { 1.0f, 1.0f, 1.0f, 1.0f };
		m_fGradientMultiplier = 0.5000;
		m_nSubGroup = 1;
		m_fPositionOffsetU = 0.0000;

		m_fWidthRotate = 0.0f;
		m_fScrollSpeed = 0.0f;
		m_fCurrentScroll = 0.0f;
		m_fDistanceInnerOffset = 0.0f;
		m_fIntensityFade = 0.0f;
		m_fTextureColorDesaturate = 0.0f;
		m_bAnimorphicBehavesLikeArtefact = false;


		break;
	case ChromaticFx:
		
		//m_fDistFromLight = 0.3000;
		//m_fSize = 0.0500;
		m_fWidthRotate = 0.0500;
		//m_fAnimorphicScaleFactorU = 0.0000;
		//m_fAnimorphicScaleFactorV = 0.0000;
		m_fScrollSpeed = 0.0800;
		m_fDistanceInnerOffset = 0.0000;
		m_fIntensityScale = 10.0000;
		m_fIntensityFade = 0.0000;
		//m_color = { 1.0f, 1.0f, 1.0f, 1.0f };
		m_fTextureColorDesaturate = 0.0000;

		m_fCurrentScroll = 0.0f;
		m_fGradientMultiplier = 0.0f;
		m_nSubGroup = 0.0f;
		m_bAnimorphicBehavesLikeArtefact = false;
		m_bAlignRotationToSun = false;
		m_fPositionOffsetU = 0.0f;

		break;
	case CoronaFx:
		
		//m_fDistFromLight = 0.4000;
		//m_fSize = -0.6000;
		//m_fAnimorphicScaleFactorU = 0.0000;
		//m_fAnimorphicScaleFactorV = 0.0000;
		m_fWidthRotate = 0.0000;
		m_fIntensityScale = 20.0000;
		//m_color = { 1.0f, 1.0f, 1.0f, 1.0f };

		m_fScrollSpeed = 0.0f;
		m_fCurrentScroll = 0.0f;
		m_fDistanceInnerOffset = 0.0f;
		m_fIntensityFade = 0.0f;
		m_fTextureColorDesaturate = 0.0f;
		m_fGradientMultiplier = 0.0f;
		m_nSubGroup = 0.0f;
		m_bAnimorphicBehavesLikeArtefact = 0.0f;
		m_bAlignRotationToSun = 0.0f;
		m_fPositionOffsetU = 0.0f;

		break;
	default:
		break;
	}
}
