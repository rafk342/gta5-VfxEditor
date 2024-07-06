#include "CLensFlare.h"

#define PI	(3.14159265358979323846264338327950288l)


CFlareFX::CFlareFX(FlareFxTextureType_e type)
{
	switch (type)
	{
	case AnimorphicFx:

		m_fDistFromLight = 0.2f;
		m_fSize = 0.2f;
		m_fWidthRotate = 0.0f;
		m_fScrollSpeed = 0.0f;
		m_fCurrentScroll = 0.0f;
		m_fDistanceInnerOffset = 0.0f;
		m_fIntensityScale = 100.f;
		m_fIntensityFade = 0.0f;
		m_fAnimorphicScaleFactorU = 0.0f;
		m_fAnimorphicScaleFactorV = 0.0f;
		m_color = { 1.0f, 1.0f, 1.0f, 1.0f };
		m_fTextureColorDesaturate = 0.0f;
		m_fGradientMultiplier = 0.0f;
		m_nTexture = 0;
		m_nSubGroup = 0;
		m_bAnimorphicBehavesLikeArtefact = false;
		m_bAlignRotationToSun = false;
		m_fPositionOffsetU = 0.0f;

		break;
	case ArtefactFx:


		break;
	case ChromaticFx:


		break;
	case CoronaFx:


		break;
	default:
		break;
	}
}


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

ImVec2 ConvertToPixelCoordinates(const ImVec2& normalizedPoint)
{
	auto windowSize = ImGui::GetMainViewport()->Size;
	return { normalizedPoint.x * windowSize.x, normalizedPoint.y * windowSize.y };
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//								DebugOverlay
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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


LensFlares_DebugOverlay::self_t* LensFlares_DebugOverlay::self = nullptr;

static gmAddress s_RenderFlareFxAddr;
//animorphic + 
//artefact size+ / scale - 
//corona scale + / scale +


void(*RenderFlareFxfn)(u64, u64, u64, float, float*);
void LensFlares_DebugOverlay::hk_RenderFlareFx(u64 arg1, u64 arg2, u64 arg3, float fIntensity, float* SunPos)
{
	using namespace rage;
	RenderFlareFxfn(arg1, arg2, arg3, fIntensity, SunPos);

	if (!self)
		return;

	if (self->m_Handler->pCLensFlares->m_SunVisibility < 0.01f)
		return;


	self->m_StartPoint = { SunPos[0], SunPos[1] };

	Vec3V vFlareDirNorm = Vec3V(0.5f) - self->m_StartPoint;
	ScalarV fLen = LengthVec2(vFlareDirNorm);

	Vec3V vDist = vFlareDirNorm.Abs() + Vec3V(0.5f);
	vFlareDirNorm /= fLen;

	ScalarV fDistToEdge = LengthVec2(vDist);

	Vec3V vFlareDir = vFlareDirNorm * fDistToEdge;
	self->m_EndPoint = self->m_StartPoint + vFlareDir;

	ScalarV fDistScale = std::min(1.0f, (fLen * 2).Get());
	fDistScale *= fDistScale;

	Vec3V vAspectRatio = { 1,GetMainViewportAspectRatio() };

	auto* lensflares = self->m_Handler->pCLensFlares;
	auto& settings = lensflares->m_Settings[lensflares->m_ActiveSettingsIndex];
	auto& arr = settings.m_arrFlareFX;

	for (auto& flare : arr)
	{
		/*
		*	Position
		*
		*/

		ScalarV fDistFromLightFactor;
		bool bCurAnamorphicLikeArtefact = flare.m_nTexture == AnimorphicFx && flare.m_bAnimorphicBehavesLikeArtefact;
		if (flare.m_nTexture == ArtefactFx || bCurAnamorphicLikeArtefact)
		{
			float fDistFromCenterNorm = 2.0f * fLen.Get();
			fDistFromLightFactor = ScalarV(2.0f * fDistFromCenterNorm + flare.m_fDistFromLight * fDistFromCenterNorm);
		}
		else
		{
			fDistFromLightFactor = ScalarV(flare.m_fDistFromLight) * fDistScale;
		}

		Vec3V vFlarePos = (self->m_StartPoint + (vFlareDir * fDistFromLightFactor));
		if (flare.m_nTexture == ArtefactFx)
		{
			vFlarePos.SetX(vFlarePos.X() + flare.m_fPositionOffsetU);
		}
		Vec3V vDirFromLight = (self->m_StartPoint - vFlarePos).Normalized();

		/*
		*	Scale
		*
		*/
		
		//	size
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

		//	scale
		float scale_factor_x = fHalfSize;
		float scale_factor_y = fHalfSize;

		if (flare.m_nTexture != ArtefactFx)
		{
			scale_factor_x = fHalfSize + fLen.Get() * flare.m_fAnimorphicScaleFactorU;
			scale_factor_y = fHalfSize + fLen.Get() * flare.m_fAnimorphicScaleFactorV;
		}
		else
		{
			Vec3V finalAspect = vAspectRatio;
			finalAspect.SetX(finalAspect.X() + fLen.Get() * flare.m_fAnimorphicScaleFactorU);
			finalAspect.SetY(finalAspect.Y() + fLen.Get() * flare.m_fAnimorphicScaleFactorV);

			scale_factor_x *= finalAspect.X();
			scale_factor_y *= finalAspect.Y() / 2;

		}
		
		if (flare.m_nTexture == CoronaFx)
		{
			scale_factor_x /= 2;
			scale_factor_y /= 2;
		}

		/*
		*	Rotate
		* 
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

		self->m_CirclesDrawData.push_back(CircleDrawData(
		/*pos	*/	ConvertToPixelCoordinates({vFlarePos[0], vFlarePos[1]}),
		/*rotate*/	rotate,
		/*color	*/	flare.m_color * self->scalar_color,
		/*scale	*/	{ scale_factor_x * 1000, scale_factor_y * 1000 } 
			));

	}
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


LensFlares_DebugOverlay::LensFlares_DebugOverlay(LensFlareHandler* handler) : m_Handler(handler)
{
	self = this;
	s_RenderFlareFxAddr = gmAddress::Scan("48 8B C4 F3 0F 11 58 ?? 4C 89 40 ?? 55");
	p_msTime = s_RenderFlareFxAddr.GetAt(0xbea).GetRef(2).To<u32*>();
	Hook::Create(s_RenderFlareFxAddr, hk_RenderFlareFx, &RenderFlareFxfn, "RenderFlareFx");
}

LensFlares_DebugOverlay::~LensFlares_DebugOverlay()
{
	Hook::Remove(s_RenderFlareFxAddr, "RenderFlareFx");
	m_Handler = nullptr;
	self = nullptr;
}

void LensFlares_DebugOverlay::EndFrame()
{
	if (m_Handler->pCLensFlares->m_SunVisibility < 0.01f) 
		return;

	ImVec2 start_p = ConvertToPixelCoordinates({ m_StartPoint.X(), m_StartPoint.Y() });
	ImVec2 end_p = ConvertToPixelCoordinates({ m_EndPoint.X(), m_EndPoint.Y() });

	line_color.setAlphaf(scalar_color.getAlphaf());

	ImDrawList* dl = ImGui::GetBackgroundDrawList();
	dl->AddLine(start_p, end_p, ConvertToImguiCol32(line_color), thickness);

	for (auto& data : m_CirclesDrawData)
	{
		dl->AddEllipse(data.pos, data.scale.x, data.scale.y , ConvertToImguiCol32(data.col), data.rotate, 32, thickness);
	}
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////			
//							LensFlareHandler
//							
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LensFlareHandler::self_t* LensFlareHandler::self = nullptr;

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
		self->m_DebugOverlay.EndFrame();
	}
	self->m_DebugOverlay.m_CirclesDrawData.clear();
	self->m_DebugOverlay.m_StartPoint = 0;
	self->m_DebugOverlay.m_EndPoint = 0;
}


void LensFlareHandler::SetDebugOverlayVisibility(bool show)
{
	m_ShowDebugOverlay = show;
}

void LensFlareHandler::SortFlaresByDistance(atArray<CFlareFX>& arr)
{
	std::sort(arr.begin(), arr.end(), [](CFlareFX& left, CFlareFX& right) -> bool
		{
			return left.m_fDistFromLight < right.m_fDistFromLight;
		});
}

