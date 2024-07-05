#include "LensFlareUi.h"

#include <map>



LensFlareUi::LensFlareUi(const char* title) : App(title)
{ }

void LensFlareUi::window()
{
	float v_speed = 0.01f;
	float col[4]{};
	static bool DebugOverlay = false;
	static float col1[4]{};
	
	static std::array FileNames
	{
		"M" ,
		"F" ,
		"T" ,
	};

	static int F_Index = m_Handler.gCLensFlares->m_ActiveSettingsIndex;

	if (ImGui::Combo("SelectedFile", &F_Index, FileNames.data(), FileNames.size())) 
	{
		m_Handler.ChangeSettings(F_Index);
	}
	if (F_Index != m_Handler.gCLensFlares->m_ActiveSettingsIndex)
	{
		m_Handler.ChangeSettings(F_Index);
	}

	if (ImGui::Checkbox("DebugOverlay", &DebugOverlay))
	{
		m_Handler.SetDebugOverlayVisibility(DebugOverlay);
	}

	m_Handler.m_DebugOverlay.scalar_color.Getf_col4().ToArray(col1);
	ImGui::ColorEdit4("scalar", col1);
	m_Handler.m_DebugOverlay.scalar_color = col1;

	ImGui::DragFloat("scale 1", &m_Handler.m_DebugOverlay.scale, 0.001);

		//static float col1[4]{};
	//ImGui::DragFloat("thickness", &m_Handler.m_DebugOverlay.line_thickness, 0.001);
	//ImGui::ColorPicker4("line color", col1);

	//m_Handler.m_DebugOverlay.line_color = col1;

	CLensFlareSettings* settings = m_Handler.gCLensFlares->m_Settings;

	size_t i = F_Index;
	//for (size_t i = 0; i < 3; i++)
	//{
		atArray<CFlareFX>& arr = settings[i].m_arrFlareFX;

		//if (ImGui::TreeNode(vfmt("File : {}##TreeNode", m_Handler.GetFileNameAtIndex(i))))
		//{

			if (ImGui::Button("Sort by distance"))
			{
				m_Handler.SortFlaresByDistance(arr);
			}

			if (ImGui::TreeNode(vfmt("CommonSettings## {}", i)))
			{
				ImGui::DragFloat(vfmt("Sun Visibility Factor##{}", i), &settings[i].m_fSunVisibilityFactor, 0.001f);
				ImGui::DragInt(vfmt("Sun Visibility Alpha Clip##{}", i), &settings[i].m_iSunVisibilityAlphaClip, 0.5f);
				ImGui::DragFloat(vfmt("Sun Fog Factor##{}", i), &settings[i].m_fSunFogFactor, v_speed);
				ImGui::DragFloat(vfmt("Chromatic TexU Size##{}", i), &settings[i].m_fChromaticTexUSize, v_speed);
				ImGui::DragFloat(vfmt("Exposure Scale Factor##{}", i), &settings[i].m_fExposureScaleFactor, v_speed);
				ImGui::DragFloat(vfmt("Exposure Intensity Factor##{}", i), &settings[i].m_fExposureIntensityFactor, v_speed);
				ImGui::DragFloat(vfmt("Exposure Rotation Factor##{}", i), &settings[i].m_fExposureRotationFactor, v_speed);
				ImGui::DragFloat(vfmt("Chromatic Fade Factor##{}", i), &settings[i].m_fChromaticFadeFactor, v_speed);
				ImGui::DragFloat(vfmt("Artefact Distance Fade Factor##{}", i), &settings[i].m_fArtefactDistanceFadeFactor, v_speed);
				ImGui::DragFloat(vfmt("Artefact Rotation Factor##{}", i), &settings[i].m_fArtefactRotationFactor, v_speed);
				ImGui::DragFloat(vfmt("Artefact Scale Factor##{}", i), &settings[i].m_fArtefactScaleFactor, v_speed);
				ImGui::DragFloat(vfmt("Artefact Gradient Factor##{}", i), &settings[i].m_fArtefactGradientFactor, v_speed);
				ImGui::DragFloat(vfmt("Corona Distance Additional Size##{}", i), &settings[i].m_fCoronaDistanceAdditionalSize, v_speed);
				ImGui::DragFloat(vfmt("Min Exposur eScale##{}", i), &settings[i].m_fMinExposureScale, v_speed);
				ImGui::DragFloat(vfmt("Max Exposure Scale##{}", i), &settings[i].m_fMaxExposureScale, v_speed);
				ImGui::DragFloat(vfmt("Min Exposure Intensity##{}", i), &settings[i].m_fMinExposureIntensity, v_speed);
				ImGui::DragFloat(vfmt("Max Exposure Intensity##{}", i), &settings[i].m_fMaxExposureIntensity, v_speed);
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
				if (ImGui::Button("PushBack Empty"))
				{
					arr.push_back(CFlareFX());
				}

				ImGui::TreePop();
			}

			ImGui::TreePop();
		//}
	//}
}


void LensFlareUi::importData(std::filesystem::path path)
{

}

void LensFlareUi::exportData(std::filesystem::path path)
{

}



void LensFlareUi::TreeNodeForAnimorphicType(CFlareFX& CurrFlareFx, size_t fileIndex, size_t FlareFxIndex)
{
	float col[4]{};
	size_t i = fileIndex;
	size_t j = FlareFxIndex;
	float v_speed = 0.01f;

	ImGui::Checkbox(vfmt("Behaves Like Artefact ##{} {}", i, j),	&CurrFlareFx.m_bAnimorphicBehavesLikeArtefact);
	ImGui::DragFloat(vfmt("Distance From Light ##{} {}", i, j),		&CurrFlareFx.m_fDistFromLight, v_speed);
	ImGui::DragFloat(vfmt("Size ##{} {}", i, j),					&CurrFlareFx.m_fSize, v_speed);
	ImGui::DragFloat(vfmt("Scale FactorU ##{} {}", i, j),			&CurrFlareFx.m_fAnimorphicScaleFactorU, v_speed);
	ImGui::DragFloat(vfmt("Scale FactorV ##{} {}", i, j),			&CurrFlareFx.m_fAnimorphicScaleFactorV, v_speed);
	ImGui::DragFloat(vfmt("Rotate ##{} {}", i, j),					&CurrFlareFx.m_fWidthRotate, v_speed);
	ImGui::DragFloat(vfmt("Intensity Scale ##{} {}", i, j),			&CurrFlareFx.m_fIntensityScale, v_speed);
	
	CurrFlareFx.m_color.Getf_col4().ToArray(col);
	if (ImGui::ColorEdit4(vfmt("Color ##{} {}", i, j), col))
		CurrFlareFx.m_color.Setf_col4(col);

}

void LensFlareUi::TreeNodeForArtefactType(CFlareFX& CurrFlareFx, size_t fileIndex, size_t FlareFxIndex)
{
	float col[4]{};
	size_t i = fileIndex;
	size_t j = FlareFxIndex;
	float v_speed = 0.01f;


	ImGui::DragFloat(vfmt("Dist From Light ##{} {}", i, j),			&CurrFlareFx.m_fDistFromLight, v_speed);
	ImGui::DragFloat(vfmt("Size ##{} {}", i, j),					&CurrFlareFx.m_fSize, v_speed);
	ImGui::DragFloat(vfmt("Scale FactorU ##{} {}", i, j),			&CurrFlareFx.m_fAnimorphicScaleFactorU, v_speed);
	ImGui::DragFloat(vfmt("Scale FactorV ##{} {}", i, j),			&CurrFlareFx.m_fAnimorphicScaleFactorV, v_speed);
	ImGui::DragFloat(vfmt("Intensity Scale ##{} {}", i, j),			&CurrFlareFx.m_fIntensityScale, v_speed);
	ImGui::Checkbox(vfmt("Align Rotation To Sun ##{} {}", i, j),	&CurrFlareFx.m_bAlignRotationToSun);
	
	CurrFlareFx.m_color.Getf_col4().ToArray(col);
	if (ImGui::ColorEdit4(vfmt("Color ##{} {}", i, j), col))
		CurrFlareFx.m_color.Setf_col4(col);

	ImGui::DragFloat(vfmt("Gradient Multiplier ##{} {}", i, j), &CurrFlareFx.m_fGradientMultiplier, v_speed);

	int v2 = CurrFlareFx.m_nSubGroup;
	if (ImGui::InputInt(vfmt("SubGroup##{} {}", i, j), &v2)) {
		if (v2 > 8) v2 = 8;
		if (v2 < 0) v2 = 0;
		CurrFlareFx.m_nSubGroup = v2;
	}
	ImGui::DragFloat(vfmt("Position OffsetU ##{} {}", i, j), &CurrFlareFx.m_fPositionOffsetU, v_speed);

}

void LensFlareUi::TreeNodeForChromaticType(CFlareFX& CurrFlareFx, size_t fileIndex, size_t FlareFxIndex)
{
	float col[4]{};
	size_t i = fileIndex;
	size_t j = FlareFxIndex;
	float v_speed = 0.01f;

	ImGui::DragFloat(vfmt("Dist From Light ##{} {}", i, j),			&CurrFlareFx.m_fDistFromLight, v_speed);
	ImGui::DragFloat(vfmt("Size ##{} {}", i, j),					&CurrFlareFx.m_fSize, v_speed);
	ImGui::DragFloat(vfmt("Width ##{} {}", i, j),					&CurrFlareFx.m_fWidthRotate, v_speed);
	ImGui::DragFloat(vfmt("Scale FactorU ##{} {}", i, j),			&CurrFlareFx.m_fAnimorphicScaleFactorU, v_speed);
	ImGui::DragFloat(vfmt("Scale FactorV ##{} {}", i, j),			&CurrFlareFx.m_fAnimorphicScaleFactorV, v_speed);
	ImGui::DragFloat(vfmt("Scroll Speed ##{} {}", i, j),			&CurrFlareFx.m_fScrollSpeed, v_speed);
	ImGui::DragFloat(vfmt("Distance Inner Offset ##{} {}", i, j),	&CurrFlareFx.m_fDistanceInnerOffset, v_speed);
	ImGui::DragFloat(vfmt("Intensity Scale ##{} {}", i, j),			&CurrFlareFx.m_fIntensityScale, v_speed);
	ImGui::DragFloat(vfmt("Intensity Fade ##{} {}", i, j),			&CurrFlareFx.m_fIntensityFade, v_speed);
	
	CurrFlareFx.m_color.Getf_col4().ToArray(col);
	if (ImGui::ColorEdit4(vfmt("Color ##{} {}", i, j), col))
		CurrFlareFx.m_color.Setf_col4(col);
	
	ImGui::DragFloat(vfmt("Texture Color Desaturate ##{} {}", i, j), &CurrFlareFx.m_fTextureColorDesaturate, v_speed);
}

void LensFlareUi::TreeNodeForCoronaType(CFlareFX& CurrFlareFx, size_t fileIndex, size_t FlareFxIndex)
{
	float col[4]{};
	size_t i = fileIndex;
	size_t j = FlareFxIndex;
	float v_speed = 0.01f;

	ImGui::DragFloat(vfmt("Dist From Light ##{} {}", i, j),				&CurrFlareFx.m_fDistFromLight, v_speed);
	ImGui::DragFloat(vfmt("Size ##{} {}", i, j),						&CurrFlareFx.m_fSize, v_speed);
	ImGui::DragFloat(vfmt("Scale FactorU ##{} {}", i, j),	&CurrFlareFx.m_fAnimorphicScaleFactorU, v_speed);
	ImGui::DragFloat(vfmt("Scale FactorV ##{} {}", i, j),	&CurrFlareFx.m_fAnimorphicScaleFactorV, v_speed);
	ImGui::DragFloat(vfmt("Rotate ##{} {}", i, j),				&CurrFlareFx.m_fWidthRotate, v_speed);
	ImGui::DragFloat(vfmt("Intensity Scale ##{} {}", i, j),				&CurrFlareFx.m_fIntensityScale, v_speed);
	
	CurrFlareFx.m_color.Getf_col4().ToArray(col);
	if (ImGui::ColorEdit4(vfmt("Color ##{} {}", i, j), col))
			CurrFlareFx.m_color.Setf_col4(col);
}

//ImGui::DragFloat(vfmt("_Dist From Light ##{} {}", i, j),				&CurrFlareFx.m_fDistFromLight, v_speed);
//ImGui::DragFloat(vfmt("_Size ##{} {}", i, j),						&CurrFlareFx.m_fSize, v_speed);
//ImGui::DragFloat(vfmt("_Width Rotate ##{} {}", i, j),				&CurrFlareFx.m_fWidthRotate, v_speed);
//ImGui::DragFloat(vfmt("_Scroll Speed ##{} {}", i, j),				&CurrFlareFx.m_fScrollSpeed, v_speed);
//ImGui::DragFloat(vfmt("_Distance Inner Offset ##{} {}", i, j),		&CurrFlareFx.m_fDistanceInnerOffset, v_speed);
//ImGui::DragFloat(vfmt("_Intensity Scale ##{} {}", i, j),				&CurrFlareFx.m_fIntensityScale, v_speed);
//ImGui::DragFloat(vfmt("_Intensity Fade ##{} {}", i, j),				&CurrFlareFx.m_fIntensityFade, v_speed);
//ImGui::DragFloat(vfmt("_Animorphic Scale FactorU ##{} {}", i, j),	&CurrFlareFx.m_fAnimorphicScaleFactorU, v_speed);
//ImGui::DragFloat(vfmt("_Animorphic Scale FactorV ##{} {}", i, j),	&CurrFlareFx.m_fAnimorphicScaleFactorV, v_speed);
//ImGui::Checkbox(vfmt("_Animorphic Behaves Like Artefact ##{} {}", i, j), &CurrFlareFx.m_bAnimorphicBehavesLikeArtefact);
//ImGui::Checkbox(vfmt("_Align Rotation To Sun ##{} {}", i, j),		&CurrFlareFx.m_bAlignRotationToSun);
//
//CurrFlareFx.m_color.Getf_col4().ToArray(col);
//if (ImGui::ColorEdit4(vfmt("_Color ##{} {}", i, j), col))
//	CurrFlareFx.m_color.Setf_col4(col);
//
//ImGui::DragFloat(vfmt("_Gradient Multiplier ##{} {}", i, j), &CurrFlareFx.m_fGradientMultiplier, v_speed);
//
//int v1 = CurrFlareFx.m_nTexture;
//if (ImGui::InputInt(vfmt("_Texture Num##{} {}", i, j), &v1))
//{
//	if (v1 > 3) v1 = 3; 
//	if (v1 < 0) v1 = 0;

//	CurrFlareFx.m_nTexture = v1;
//}

//
//int v2 = CurrFlareFx.m_nSubGroup;
//if (ImGui::InputInt(vfmt("_SubGroup Num ##{} {}", i, j), &v2))
//{
//	if (v2 > 8) v2 = 8;
//	if (v2 < 0) v2 = 0;

//	CurrFlareFx.m_nSubGroup = v2;
//}

//ImGui::DragFloat(vfmt("_Position OffsetU ##{} {}", i, j), &CurrFlareFx.m_fPositionOffsetU, v_speed);
//ImGui::DragFloat(vfmt("_Texture Color Desaturate ##{} {}", i, j), &CurrFlareFx.m_fTextureColorDesaturate, v_speed);
