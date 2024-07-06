#include "LensFlareUi.h"

#include <map>
//m_Handler.m_DebugOverlay.scalar_color.Getf_col4().ToArray(col1);
//ImGui::ColorEdit4("scalar", col1);
//m_Handler.m_DebugOverlay.scalar_color = col1;


static float v_speed = 0.01f;

LensFlareUi::LensFlareUi(const char* title) : App(title)
{ }

void LensFlareUi::window()
{
	static std::array FileNames
	{
		"LENSFLARE_M" ,
		"LENSFLARE_F" ,
		"LENSFLARE_T" ,
	};
	static int F_Index = m_Handler.pCLensFlares->m_ActiveSettingsIndex;
	if (F_Index != m_Handler.pCLensFlares->m_ActiveSettingsIndex)
		m_Handler.ChangeSettings(F_Index);


	static float col1[4]{};
	
	if (ImGui::Combo("Active Settings", &F_Index, FileNames.data(), FileNames.size())) 
	{
		m_Handler.ChangeSettings(F_Index);
	}
	
	PushStyleCompact();
	{
		if (ImGui::Checkbox("DebugOverlay", &m_ShowDebugOverlay))
		{
			m_Handler.SetDebugOverlayVisibility(m_ShowDebugOverlay);
		}
	}
	PopStyleCompact();
	ImGui::Separator();


	size_t i = F_Index;
	CLensFlareSettings* settings = m_Handler.pCLensFlares->m_Settings;
	atArray<CFlareFX>& arr = settings[i].m_arrFlareFX;

	if (ImGui::TreeNode(vfmt("CommonSettings## {}", i)))
	{
		ImGui::DragFloat(vfmt("Sun Visibility Factor##{}", i),			&settings[i].m_fSunVisibilityFactor, 0.001f);
		ImGui::DragInt(vfmt("Sun Visibility Alpha Clip##{}", i),		&settings[i].m_iSunVisibilityAlphaClip, 0.5f);
		ImGui::DragFloat(vfmt("Sun Fog Factor##{}", i),					&settings[i].m_fSunFogFactor, v_speed);
		ImGui::DragFloat(vfmt("Chromatic TexU Size##{}", i),			&settings[i].m_fChromaticTexUSize, v_speed);
		ImGui::DragFloat(vfmt("Exposure Scale Factor##{}", i),			&settings[i].m_fExposureScaleFactor, v_speed);
		ImGui::DragFloat(vfmt("Exposure Intensity Factor##{}", i),		&settings[i].m_fExposureIntensityFactor, v_speed);
		ImGui::DragFloat(vfmt("Exposure Rotation Factor##{}", i),		&settings[i].m_fExposureRotationFactor, v_speed);
		ImGui::DragFloat(vfmt("Chromatic Fade Factor##{}", i),			&settings[i].m_fChromaticFadeFactor, v_speed);
		ImGui::DragFloat(vfmt("Artefact Distance Fade Factor##{}", i),	&settings[i].m_fArtefactDistanceFadeFactor, v_speed);
		ImGui::DragFloat(vfmt("Artefact Rotation Factor##{}", i),		&settings[i].m_fArtefactRotationFactor, v_speed);
		ImGui::DragFloat(vfmt("Artefact Scale Factor##{}", i),			&settings[i].m_fArtefactScaleFactor, v_speed);
		ImGui::DragFloat(vfmt("Artefact Gradient Factor##{}", i),		&settings[i].m_fArtefactGradientFactor, v_speed);
		ImGui::DragFloat(vfmt("Corona Distance Additional Size##{}", i),&settings[i].m_fCoronaDistanceAdditionalSize, v_speed);
		ImGui::DragFloat(vfmt("Min Exposur eScale##{}", i),				&settings[i].m_fMinExposureScale, v_speed);
		ImGui::DragFloat(vfmt("Max Exposure Scale##{}", i),				&settings[i].m_fMaxExposureScale, v_speed);
		ImGui::DragFloat(vfmt("Min Exposure Intensity##{}", i),			&settings[i].m_fMinExposureIntensity, v_speed);
		ImGui::DragFloat(vfmt("Max Exposure Intensity##{}", i),			&settings[i].m_fMaxExposureIntensity, v_speed);
		ImGui::TreePop();
	}

	
	if (ImGui::TreeNode(vfmt("FlareFX Array##_{}", i)))
	{
		for (size_t j = 0; j < arr.size(); j++)
		{
			CFlareFX& CurrFlareFx = arr[j];

			if (ImGui::TreeNode(vfmt("index : {}##_{}", j, i)))
			{
				int index = CurrFlareFx.m_nTexture;
				static std::array TextureNames
				{
					"AnimorphicFx",
					"ArtefactFx",
					"ChromaticFx",
					"CoronaFx",
				};

				if (ImGui::Combo(vfmt("Type##{}{}", i, j), &index, TextureNames.data(), TextureNames.size()))
				{
					if (index != ArtefactFx) {
						CurrFlareFx.m_nSubGroup = 0;
					}
					CurrFlareFx.m_nTexture = index;
				}

				ImGui::Separator();

				switch (static_cast<FlareFxTextureType_e>(CurrFlareFx.m_nTexture))
				{
				case AnimorphicFx:

					TreeNodeForAnimorphicType(CurrFlareFx, i,j);
					break;

				case ArtefactFx:

					TreeNodeForArtefactType(CurrFlareFx, i, j);
					break;

				case ChromaticFx:

					TreeNodeForChromaticType(CurrFlareFx, i, j);
					break;

				case CoronaFx:

					TreeNodeForCoronaType(CurrFlareFx, i, j);
					break;

				default:
					break;
				}
				ImGui::Separator();
					
				ImGui::Separator();

				if (ImGui::Button("Remove"))
				{
					arr.remove_at(j);
				}

				ImGui::TreePop();
			}
		}

		if (ImGui::Button("Clear"))
		{
			arr.clear();
		}
		if (ImGui::Button("PushBack"))
		{
			arr.push_back(CFlareFX());
		}
		if (ImGui::Button("Sort by distance"))
		{
			m_Handler.SortFlaresByDistance(arr);
		}

		ImGui::TreePop();
	}

}


void LensFlareUi::importData(std::filesystem::path path)
{

}

void LensFlareUi::exportData(std::filesystem::path path)
{

}



void LensFlareUi::TreeNodeForAnimorphicType(CFlareFX& CurrFlareFx, size_t fileIndex, size_t FlareFxIndex)
{
	float col[3]{};
	size_t i = fileIndex;
	size_t j = FlareFxIndex;

	ImGui::Checkbox(vfmt("Behaves Like Artefact ##{} {}", i, j),	&CurrFlareFx.m_bAnimorphicBehavesLikeArtefact);
	ImGui::DragFloat(vfmt("Distance From Light ##{} {}", i, j),		&CurrFlareFx.m_fDistFromLight, 0.0005f, 0.0f, 0.0f, "%.4f");
	ImGui::DragFloat(vfmt("Size ##{} {}", i, j),					&CurrFlareFx.m_fSize, 0.0010000000f, 0.0f, 0.0f, "%.4f");
	ImGui::DragFloat(vfmt("Scale FactorU ##{} {}", i, j),			&CurrFlareFx.m_fAnimorphicScaleFactorU, 0.0015000000f, 0.0f, 0.0f, "%.4f");
	ImGui::DragFloat(vfmt("Scale FactorV ##{} {}", i, j),			&CurrFlareFx.m_fAnimorphicScaleFactorV, 0.0015000000f, 0.0f, 0.0f, "%.4f");
	ImGui::DragFloat(vfmt("Rotate ##{} {}", i, j),					&CurrFlareFx.m_fWidthRotate, 0.0050000000f, 0.0f, 0.0f, "%.4f");
	ImGui::DragFloat(vfmt("Intensity Scale ##{} {}", i, j),			&CurrFlareFx.m_fIntensityScale, 0.2500000000f, 0.0f, 0.0f, "%.4f");
	
	CurrFlareFx.m_color.Getf_col3().ToArray(col);
	if (ImGui::ColorEdit3(vfmt("Color ##{} {}", i, j), col))
		CurrFlareFx.m_color.Setf_col3(col);

}

void LensFlareUi::TreeNodeForArtefactType(CFlareFX& CurrFlareFx, size_t fileIndex, size_t FlareFxIndex)
{
	float col[3]{};
	size_t i = fileIndex;
	size_t j = FlareFxIndex;


	ImGui::DragFloat(vfmt("Distance From Light ##{} {}", i, j),		&CurrFlareFx.m_fDistFromLight, 0.0005f, 0.0f, 0.0f, "%.4f");
	ImGui::DragFloat(vfmt("Size ##{} {}", i, j),					&CurrFlareFx.m_fSize, 0.0002500000f, 0.0f, 0.0f, "%.4f");
	ImGui::DragFloat(vfmt("Scale FactorU ##{} {}", i, j),			&CurrFlareFx.m_fAnimorphicScaleFactorU, 0.0150000000f, 0.0f, 0.0f, "%.4f");
	ImGui::DragFloat(vfmt("Scale FactorV ##{} {}", i, j),			&CurrFlareFx.m_fAnimorphicScaleFactorV, 0.0150000000f, 0.0f, 0.0f, "%.4f");
	ImGui::DragFloat(vfmt("Intensity Scale ##{} {}", i, j),			&CurrFlareFx.m_fIntensityScale, 1.0000000000, 0.0f, 0.0f, "%.4f");
	ImGui::Checkbox(vfmt("Align Rotation To Sun ##{} {}", i, j),	&CurrFlareFx.m_bAlignRotationToSun);
	
	CurrFlareFx.m_color.Getf_col3().ToArray(col);
	if (ImGui::ColorEdit3(vfmt("Color ##{} {}", i, j), col))
		CurrFlareFx.m_color.Setf_col3(col);

	ImGui::DragFloat(vfmt("Gradient Multiplier ##{} {}", i, j), &CurrFlareFx.m_fGradientMultiplier, 0.0015000000f, 0.0f, 0.0f, "%.4f");

	int v2 = CurrFlareFx.m_nSubGroup;
	if (ImGui::InputInt(vfmt("SubGroup##{} {}", i, j), &v2)) {
		if (v2 > 8) v2 = 8;
		if (v2 < 0) v2 = 0;
		CurrFlareFx.m_nSubGroup = v2;
	}
	ImGui::DragFloat(vfmt("Position OffsetU ##{} {}", i, j), &CurrFlareFx.m_fPositionOffsetU, 0.0005000000f, 0.0f, 0.0f, "%.4f");

}

void LensFlareUi::TreeNodeForChromaticType(CFlareFX& CurrFlareFx, size_t fileIndex, size_t FlareFxIndex)
{
	float col[3]{};
	size_t i = fileIndex;
	size_t j = FlareFxIndex;

	ImGui::DragFloat(vfmt("Distance From Light ##{} {}", i, j),		&CurrFlareFx.m_fDistFromLight, 0.0005f, 0.0f, 0.0f, "%.4f");
	ImGui::DragFloat(vfmt("Size ##{} {}", i, j),					&CurrFlareFx.m_fSize, 0.0010000000f, 0.0f, 0.0f, "%.4f");
	ImGui::DragFloat(vfmt("Width ##{} {}", i, j),					&CurrFlareFx.m_fWidthRotate, 0.0010000000, 0.0f, 0.0f, "%.4f");
	ImGui::DragFloat(vfmt("Scale FactorU ##{} {}", i, j),			&CurrFlareFx.m_fAnimorphicScaleFactorU, 0.0015000000f, 0.0f, 0.0f, "%.4f");
	ImGui::DragFloat(vfmt("Scale FactorV ##{} {}", i, j),			&CurrFlareFx.m_fAnimorphicScaleFactorV, 0.0015000000f, 0.0f, 0.0f, "%.4f");
	ImGui::DragFloat(vfmt("Scroll Speed ##{} {}", i, j),			&CurrFlareFx.m_fScrollSpeed, 0.0010000000, 0.0f, 0.0f, "%.4f");
	ImGui::DragFloat(vfmt("Distance Inner Offset ##{} {}", i, j),	&CurrFlareFx.m_fDistanceInnerOffset, 0.0010000000, 0.0f, 0.0f, "%.4f");
	ImGui::DragFloat(vfmt("Intensity Scale ##{} {}", i, j),			&CurrFlareFx.m_fIntensityScale, 0.0250000000, 0.0f, 0.0f, "%.4f");
	ImGui::DragFloat(vfmt("Intensity Fade ##{} {}", i, j),			&CurrFlareFx.m_fIntensityFade, 0.0050000069, 0.0f, 0.0f, "%.4f");
	
	CurrFlareFx.m_color.Getf_col3().ToArray(col);
	if (ImGui::ColorEdit3(vfmt("Color ##{} {}", i, j), col))
		CurrFlareFx.m_color.Setf_col3(col);

	ImGui::DragFloat(vfmt("Texture Color Desaturate ##{} {}", i, j), &CurrFlareFx.m_fTextureColorDesaturate, 0.0015000070, 0.0f, 0.0f, "%.4f");
}

void LensFlareUi::TreeNodeForCoronaType(CFlareFX& CurrFlareFx, size_t fileIndex, size_t FlareFxIndex)
{
	float col[3]{};
	size_t i = fileIndex;
	size_t j = FlareFxIndex;

	ImGui::DragFloat(vfmt("Distance From Light ##{} {}", i, j),		&CurrFlareFx.m_fDistFromLight, 0.0005f, 0.0f, 0.0f, "%.4f");
	ImGui::DragFloat(vfmt("Size ##{} {}", i, j),					&CurrFlareFx.m_fSize, 0.0010000000f, 0.0f, 0.0f, "%.4f");
	ImGui::DragFloat(vfmt("Scale FactorU ##{} {}", i, j),			&CurrFlareFx.m_fAnimorphicScaleFactorU, 0.0015000000f, 0.0f, 0.0f, "%.4f");
	ImGui::DragFloat(vfmt("Scale FactorV ##{} {}", i, j),			&CurrFlareFx.m_fAnimorphicScaleFactorV, 0.0015000000f, 0.0f, 0.0f, "%.4f");
	ImGui::DragFloat(vfmt("Rotation Speed##{} {}", i, j),		&CurrFlareFx.m_fWidthRotate, 0.0015000070, 0.0f, 0.0f, "%.4f");
	ImGui::DragFloat(vfmt("Intensity Scale ##{} {}", i, j),			&CurrFlareFx.m_fIntensityScale, 0.2500000000f, 0.0f, 0.0f, "%.4f");
	
	CurrFlareFx.m_color.Getf_col3().ToArray(col);
	if (ImGui::ColorEdit3(vfmt("Color ##{} {}", i, j), col))
		CurrFlareFx.m_color.Setf_col3(col);
}
