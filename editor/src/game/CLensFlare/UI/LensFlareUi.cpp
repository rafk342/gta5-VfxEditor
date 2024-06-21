#include "LensFlareUi.h"

#include <map>



LensFlareUi::LensFlareUi(const char* title) : App(title)
{ }

void LensFlareUi::window()
{
	float v_speed = 0.01f;
	float col[4];

	CLensFlareSettings* settings = &m_Handler.m_CLensFlare->m_Settings[0];

	ImGui::Text("Active Lensflare : %s", m_Handler.GetFileNameAtIndex(static_cast<size_t>(m_Handler.m_CLensFlare->m_ActiveIndex)));

	for (size_t i = 0; i < 3; i++)
	{
		if (ImGui::TreeNode(vfmt("File : {}##TreeNode", m_Handler.GetFileNameAtIndex(i))))
		{
			ImGui::DragFloat(vfmt("Sun Visibility Factor##{}", i),			&settings[i].m_fSunVisibilityFactor, 0.001f);
			ImGui::DragInt(vfmt("Sun Visibility Alpha Clip##{}", i),		&settings[i].m_iSunVisibilityAlphaClip, 0.5f);
			ImGui::DragFloat(vfmt("Sun Fog Factor##{}", i),					&settings[i].m_fSunFogFactor, v_speed);
			ImGui::DragFloat(vfmt("Chromatic TexU Size##{}", i),			&settings[i].m_fChromaticTexUSize, v_speed);
			ImGui::DragFloat(vfmt("Exposure Scale Factor##{}",i),			&settings[i].m_fExposureScaleFactor, v_speed);
			ImGui::DragFloat(vfmt("Exposure Intensity Factor##{}", i),		&settings[i].m_fExposureIntensityFactor, v_speed);
			ImGui::DragFloat(vfmt("Exposure Rotation Factor##{}", i),		&settings[i].m_fExposureRotationFactor, v_speed);
			ImGui::DragFloat(vfmt("Chromatic Fade Factor##{}", i),			&settings[i].m_fChromaticFadeFactor, v_speed);
			ImGui::DragFloat(vfmt("Artefact Distance Fade Factor##{}", i),	&settings[i].m_fArtefactDistanceFadeFactor, v_speed);
			ImGui::DragFloat(vfmt("Artefact Rotation Factor##{}", i),		&settings[i].m_fArtefactRotationFactor, v_speed);
			ImGui::DragFloat(vfmt("Artefact Scale Factor##{}", i),			&settings[i].m_fArtefactScaleFactor, v_speed);
			ImGui::DragFloat(vfmt("Artefact Gradient Factor##{}", i),		&settings[i].m_fArtefactGradientFactor, v_speed);
			ImGui::DragFloat(vfmt("Corona Distance Additional Size##{}", i), &settings[i].m_fCoronaDistanceAdditionalSize, v_speed);
			ImGui::DragFloat(vfmt("Min Exposur eScale##{}", i),				&settings[i].m_fMinExposureScale, v_speed);
			ImGui::DragFloat(vfmt("Max Exposure Scale##{}", i),				&settings[i].m_fMaxExposureScale, v_speed);
			ImGui::DragFloat(vfmt("Min Exposure Intensity##{}", i),			&settings[i].m_fMinExposureIntensity, v_speed);
			ImGui::DragFloat(vfmt("Max Exposure Intensity##{}", i),			&settings[i].m_fMaxExposureIntensity, v_speed);

			auto& arr = settings[i].m_arrFlareFX;
			
			if (ImGui::TreeNode(vfmt("FlareFX Array##_{}", i)))
			{
				for (size_t j = 0; j < arr.size(); j++)
				{
					CFlareFX& CurrFlareFx = arr[j];

					if (ImGui::TreeNode(vfmt("index : {}##_{}", j, i)))
					{
						//switch (static_cast<FlareFxTextureType_e>(CurrFlareFx.m_nTexture))
						//{
						//case AnimorphicType:

						//	TreeNodeForAnimorphicType();
						//	break;

						//case ArtefactType:

						//	TreeNodeForArtefactType();
						//	break;

						//case ChromaticType:

						//	TreeNodeForChromaticType();
						//	break;

						//case CoronaType:

						//	TreeNodeForCoronaType();
						//	break;

						//default:
						//	break;
						//}

						ImGui::Text(vfmt("{}## {} {}", m_Handler.GetTextureTypeName(CurrFlareFx.m_nTexture), i, j));
					
						ImGui::DragFloat(vfmt("Dist From Light ##{} {}", i, j),				&CurrFlareFx.m_fDistFromLight, v_speed);
						ImGui::DragFloat(vfmt("Size ##{} {}", i, j),						&CurrFlareFx.m_fSize, v_speed);
						ImGui::DragFloat(vfmt("Width Rotate ##{} {}", i, j),				&CurrFlareFx.m_fWidthRotate, v_speed);
						ImGui::DragFloat(vfmt("Scroll Speed ##{} {}", i, j),				&CurrFlareFx.m_fScrollSpeed, v_speed);
						ImGui::DragFloat(vfmt("Distance Inner Offset ##{} {}", i, j),		&CurrFlareFx.m_fDistanceInnerOffset, v_speed);
						ImGui::DragFloat(vfmt("Intensity Scale ##{} {}", i, j),				&CurrFlareFx.m_fIntensityScale, v_speed);
						ImGui::DragFloat(vfmt("Intensity Fade ##{} {}", i, j),				&CurrFlareFx.m_fIntensityFade, v_speed);
						ImGui::DragFloat(vfmt("Animorphic Scale FactorU ##{} {}", i, j),	&CurrFlareFx.m_fAnimorphicScaleFactorU, v_speed);
						ImGui::DragFloat(vfmt("Animorphic Scale FactorV ##{} {}", i, j),	&CurrFlareFx.m_fAnimorphicScaleFactorV, v_speed);
						ImGui::Checkbox(vfmt("Animorphic Behaves Like Artefact ##{} {}", i, j), &CurrFlareFx.m_bAnimorphicBehavesLikeArtefact);
						ImGui::Checkbox(vfmt("Align Rotation To Sun ##{} {}", i, j),		&CurrFlareFx.m_bAlignRotationToSun);
						
						CurrFlareFx.m_color.Getf_col4().ToArray(col);
						if (ImGui::ColorEdit4(vfmt("Color ##{} {}", i, j), col))
							CurrFlareFx.m_color.Setf_col4(col);
						
						ImGui::DragFloat(vfmt("Gradient Multiplier ##{} {}", i, j), &CurrFlareFx.m_fGradientMultiplier, v_speed);
						
						int v1 = CurrFlareFx.m_nTexture;
						if (ImGui::InputInt(vfmt("Texture Num##{} {}", i, j), &v1))
						{
							if (v1 > 3) v1 = 3; 
							if (v1 < 0) v1 = 0;

							CurrFlareFx.m_nTexture = v1;
						}

						
						int v2 = CurrFlareFx.m_nSubGroup;
						if (ImGui::InputInt(vfmt("SubGroup Num ##{} {}", i, j), &v2))
						{
							if (v2 > 8) v2 = 8;
							if (v2 < 0) v2 = 0;

							CurrFlareFx.m_nSubGroup = v2;
						}

						ImGui::DragFloat(vfmt("Position OffsetU ##{} {}", i, j), &CurrFlareFx.m_fPositionOffsetU, v_speed);
						ImGui::DragFloat(vfmt("Texture Color Desaturate ##{} {}", i, j), &CurrFlareFx.m_fTextureColorDesaturate, v_speed);

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
		}
	}
}


void LensFlareUi::importData(std::filesystem::path path)
{

}

void LensFlareUi::exportData(std::filesystem::path path)
{

}



void LensFlareUi::TreeNodeForAnimorphicType()
{

}

void LensFlareUi::TreeNodeForArtefactType()
{

}

void LensFlareUi::TreeNodeForChromaticType()
{

}

void LensFlareUi::TreeNodeForCoronaType()
{

}
