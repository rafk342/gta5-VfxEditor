#include "LensFlareUi.h"

#include <map>



LensFlareUi::LensFlareUi(const char* title) : App(title)
{ }

void LensFlareUi::window()
{
	static char buff[128];
	float v_speed = 0.01f;
	float col[4];

	CLensFlareSettings* settings = &m_Handler.m_CLensFlare->m_Settings[0];

	ImGui::Text("Active Lensflare : %s", m_Handler.GetFileNameAtIndex(static_cast<size_t>(m_Handler.m_CLensFlare->m_ActiveIndex)));

	for (size_t i = 0; i < 3; i++)
	{
		FORMAT_TO_BUFF(buff, "File : {}##TreeNode", m_Handler.GetFileNameAtIndex(i));

		if (ImGui::TreeNode(buff))
		{
			FORMAT_TO_BUFF(buff, "Sun Visibility Factor##{}", i);
			ImGui::DragFloat(buff, &settings[i].m_fSunVisibilityFactor, 0.001f);

			FORMAT_TO_BUFF(buff, "Sun Visibility Alpha Clip##{}", i);
			ImGui::DragInt(buff, &settings[i].m_iSunVisibilityAlphaClip, 0.5f);

			FORMAT_TO_BUFF(buff, "Sun Fog Factor##{}", i);
			ImGui::DragFloat(buff, &settings[i].m_fSunFogFactor, v_speed);

			FORMAT_TO_BUFF(buff, "Chromatic TexU Size##{}", i);
			ImGui::DragFloat(buff, &settings[i].m_fChromaticTexUSize, v_speed);

			FORMAT_TO_BUFF(buff, "Exposure Scale Factor##{}", i);
			ImGui::DragFloat(buff, &settings[i].m_fExposureScaleFactor, v_speed);

			FORMAT_TO_BUFF(buff, "Exposure Intensity Factor##{}", i);
			ImGui::DragFloat(buff, &settings[i].m_fExposureIntensityFactor, v_speed);

			FORMAT_TO_BUFF(buff, "Exposure Rotation Factor##{}", i);
			ImGui::DragFloat(buff, &settings[i].m_fExposureRotationFactor, v_speed);

			FORMAT_TO_BUFF(buff, "Chromatic Fade Factor##{}", i);
			ImGui::DragFloat(buff, &settings[i].m_fChromaticFadeFactor, v_speed);
			
			FORMAT_TO_BUFF(buff, "Artefact Distance Fade Factor##{}", i);
			ImGui::DragFloat(buff, &settings[i].m_fArtefactDistanceFadeFactor, v_speed);

			FORMAT_TO_BUFF(buff, "Artefact Rotation Factor##{}", i);
			ImGui::DragFloat(buff, &settings[i].m_fArtefactRotationFactor, v_speed);

			FORMAT_TO_BUFF(buff, "Artefact Scale Factor##{}", i);
			ImGui::DragFloat(buff, &settings[i].m_fArtefactScaleFactor, v_speed);

			FORMAT_TO_BUFF(buff, "Artefact Gradient Factor##{}", i);
			ImGui::DragFloat(buff, &settings[i].m_fArtefactGradientFactor, v_speed);

			FORMAT_TO_BUFF(buff, "Corona Distance Additional Size##{}", i);
			ImGui::DragFloat(buff, &settings[i].m_fCoronaDistanceAdditionalSize, v_speed);

			FORMAT_TO_BUFF(buff, "Min Exposur eScale##{}", i);
			ImGui::DragFloat(buff, &settings[i].m_fMinExposureScale, v_speed);

			FORMAT_TO_BUFF(buff, "Max Exposure Scale##{}", i);
			ImGui::DragFloat(buff, &settings[i].m_fMaxExposureScale, v_speed);

			FORMAT_TO_BUFF(buff, "Min Exposure Intensity##{}", i);
			ImGui::DragFloat(buff, &settings[i].m_fMinExposureIntensity, v_speed);

			FORMAT_TO_BUFF(buff, "Max Exposure Intensity##{}", i);
			ImGui::DragFloat(buff, &settings[i].m_fMaxExposureIntensity, v_speed);

			auto& arr = settings[i].m_arrFlareFX;
			
			FORMAT_TO_BUFF(buff, "FlareFX Array##_{}", i);
			if (ImGui::TreeNode(buff))
			{
				for (size_t j = 0; j < arr.GetSize(); j++)
				{
					CFlareFX& CurrFlareFx = arr[j];

					FORMAT_TO_BUFF(buff, "index : {}##_{}", j, i);
					if (ImGui::TreeNode(buff))
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


						FORMAT_TO_BUFF(buff, "{}## {} {}", m_Handler.GetTextureTypeName(CurrFlareFx.m_nTexture), i, j);
						ImGui::Text(buff);

						FORMAT_TO_BUFF(buff, "Dist From Light ##{} {}", i,j);
						ImGui::DragFloat(buff, &CurrFlareFx.m_fDistFromLight, v_speed);
						
						FORMAT_TO_BUFF(buff, "Size ##{} {}", i, j);
						ImGui::DragFloat(buff, &CurrFlareFx.m_fSize, v_speed);

						FORMAT_TO_BUFF(buff, "Width Rotate ##{} {}", i, j);
						ImGui::DragFloat(buff, &CurrFlareFx.m_fWidthRotate, v_speed);
						
						FORMAT_TO_BUFF(buff, "Scroll Speed ##{} {}", i, j);
						ImGui::DragFloat(buff, &CurrFlareFx.m_fScrollSpeed, v_speed);
						
						FORMAT_TO_BUFF(buff, "Distance Inner Offset ##{} {}", i, j);
						ImGui::DragFloat(buff, &CurrFlareFx.m_fDistanceInnerOffset, v_speed);
								
						FORMAT_TO_BUFF(buff, "Intensity Scale ##{} {}", i, j);
						ImGui::DragFloat(buff, &CurrFlareFx.m_fIntensityScale, v_speed);

						FORMAT_TO_BUFF(buff, "Intensity Fade ##{} {}", i, j);
						ImGui::DragFloat(buff, &CurrFlareFx.m_fIntensityFade, v_speed);

						FORMAT_TO_BUFF(buff, "Animorphic Scale FactorU ##{} {}", i, j);
						ImGui::DragFloat(buff, &CurrFlareFx.m_fAnimorphicScaleFactorU, v_speed);

						FORMAT_TO_BUFF(buff, "Animorphic Scale FactorV ##{} {}", i, j);
						ImGui::DragFloat(buff, &CurrFlareFx.m_fAnimorphicScaleFactorV, v_speed);
						
						FORMAT_TO_BUFF(buff, "Animorphic Behaves Like Artefact ##{} {}", i, j);
						ImGui::Checkbox(buff, &CurrFlareFx.m_bAnimorphicBehavesLikeArtefact);
	
						FORMAT_TO_BUFF(buff, "Align Rotation To Sun ##{} {}", i, j);
						ImGui::Checkbox(buff, &CurrFlareFx.m_bAlignRotationToSun);
	
						
						FORMAT_TO_BUFF(buff, "Color ##{} {}", i, j);
						
						CurrFlareFx.m_color.Getf_col4().ToArray(col);
						if (ImGui::ColorEdit4(buff, col))
							CurrFlareFx.m_color.Setf_col4(col);
						

						FORMAT_TO_BUFF(buff, "Gradient Multiplier ##{} {}", i, j);
						ImGui::DragFloat(buff, &CurrFlareFx.m_fGradientMultiplier, v_speed);

						FORMAT_TO_BUFF(buff, "Texture Num##{} {}", i, j);
						
						int v1 = CurrFlareFx.m_nTexture;
						if (ImGui::InputInt(buff, &v1))
						{
							if (v1 > 3) v1 = 3; 
							if (v1 < 0) v1 = 0;

							CurrFlareFx.m_nTexture = v1;
						}
												
						FORMAT_TO_BUFF(buff, "SubGroup Num ##{} {}", i, j);
						
						int v2 = CurrFlareFx.m_nSubGroup;
						if (ImGui::InputInt(buff, &v2))
						{
							if (v2 > 8) v2 = 8;
							if (v2 < 0) v2 = 0;

							CurrFlareFx.m_nSubGroup = v2;
						}

						FORMAT_TO_BUFF(buff, "Position OffsetU ##{} {}", i, j);
						ImGui::DragFloat(buff, &CurrFlareFx.m_fPositionOffsetU, v_speed);
						
						FORMAT_TO_BUFF(buff, "Texture Color Desaturate ##{} {}", i, j);
						ImGui::DragFloat(buff, &CurrFlareFx.m_fTextureColorDesaturate, v_speed);


						if (ImGui::Button("Remove"))
						{
							arr.RemoveAt(j);
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
