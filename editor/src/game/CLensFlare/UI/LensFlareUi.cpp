#include "LensFlareUi.h"
#include "overlayRender/Render.h"
#include <map>


static float v_speed = 0.01f;

LensFlareUi::LensFlareUi(const char* title) : App(title)
{ }

void LensFlareUi::window()
{
	constexpr static std::array TextureNames
	{
		"AnimorphicFx",
		"ArtefactFx",
		"ChromaticFx",
		"CoronaFx",
	};

	constexpr size_t TextureWithMaxNameLength_Idx = std::distance(TextureNames.begin(), std::max_element(TextureNames.begin(), TextureNames.end(), [](const char* left, const char* right)
		{
			return constexpr_strlen(left) < constexpr_strlen(right);
		}));

	constexpr static std::array FileNames
	{
		"LENSFLARE_M" ,
		"LENSFLARE_F" ,
		"LENSFLARE_T" ,
	};

	static int F_Index = m_Handler.pCLensFlares->m_ActiveSettingsIndex;
	int curr_settings = m_Handler.pCLensFlares->m_ActiveSettingsIndex;
	if (F_Index != curr_settings) {
		m_Handler.ChangeSettings(F_Index);
	}


	const char* AppSettingsButtonText = "AppSettings";
	const char* ActiveSettingsText = " Active Settings File ";

	float TextWidth = ImGui::CalcTextSize(ActiveSettingsText).x;
	float buttonWidth = ImGui::CalcTextSize(AppSettingsButtonText).x * 1.3;

	if (ImGui::BeginTable("##FlaresTable", 3))
	{
		ImGui::TableSetupColumn("_first", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupColumn("_second", ImGuiTableColumnFlags_WidthFixed, TextWidth);
		ImGui::TableSetupColumn("_third", ImGuiTableColumnFlags_WidthFixed, buttonWidth);

		ImGui::TableNextColumn();

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

		if (ImGui::Combo("##Active Settings", &F_Index, FileNames.data(), FileNames.size()))
		{
			m_Handler.ChangeSettings(F_Index);
		}

		ImGui::TableNextColumn();
		ImGui::Text(ActiveSettingsText);

		ImGui::TableNextColumn();
		if (ImGui::Button(AppSettingsButtonText))
		{
			ImGui::OpenPopup("SettingsToggle");
		}
		{
			if (ImGui::BeginPopup("SettingsToggle"))
			{
				PushStyleCompact();
				static bool ShowDebugOverlay = false;
				if (ImGui::Checkbox("Draw Debug Overlay", &ShowDebugOverlay))
				{
					m_Handler.SetDebugOverlayVisibility(ShowDebugOverlay);
				}

				static bool ShowSunLightVec = false;
				if (ImGui::Checkbox("Draw Sun Light Vector", &ShowSunLightVec))
				{
					m_Handler.SetLightVecVisibility(ShowSunLightVec);
				}

				static int alpha = 187;
				if (ImGui::DragInt("Overlay Visibility", &alpha, 1.0f, 0, 255))
				{
					m_Handler.m_DebugOverlay.SetOverlayAlpha(alpha);
				}
				PopStyleCompact();
				ImGui::EndPopup();
			}
		}

		ImGui::EndTable();
	}



	if (ImGui::CollapsingHeader("Textures"))
	{
		auto* ptr = m_Handler.pCLensFlares;

		static char _buff[4][255]{};

		for (size_t i = 0; i < std::size(ptr->m_pTextures); i++)
		{
			auto* texture = ptr->m_pTextures[i];

			if (!texture)
				continue;

			auto& ref = *texture;
			auto* curr_buff = _buff[i];

			if (ImGui::TreeNode(vfmt(" {} Texture##{}", TextureNames[i], i)))
			{
				ImVec2 UiTex_Size;
				ImVec2 Tex_Size(ref.GetWidth(), ref.GetHeight());
				float fAspectRatio = Tex_Size.x / Tex_Size.y;

				if (Tex_Size.x > 256 || Tex_Size.y > 256) {
					UiTex_Size = { 256.f, 256.f/ fAspectRatio };
				} else {
					UiTex_Size = Tex_Size;
				}

				auto* view = ref.GetTextureView();

				if (view)
					ImGui::Image(view, UiTex_Size);

				ImGui::Text(vfmt("size = {} x {}", Tex_Size.x, Tex_Size.y));

				ImGui::Text("Path:");
				ImGui::SameLine();
				ImGui::InputText(vfmt("##Path_texture{}", i), curr_buff, 255);

				ImGui::SameLine();
				if (ImGui::Button(vfmt("Swap Texture##{}",i)))
				{
					std::filesystem::path path(curr_buff);
					auto ext = path.extension();

					if (ext == ".dds")
					{
						ImGui::OpenPopup("dds texture replace popup");
					}
					else
					{
						rage::grcTexture* texture = rage::grcTextureFactory::GetInstance()->CreateFromPath(path);
						if (texture)
						{
							ptr->m_pTextures[i] = texture;
						}
					}
				}
				
				if (ImGui::BeginPopupContextItem("dds texture replace popup"))
				{
					ImGui::Text("Load As:");

					rage::grcTexture* texture = nullptr;

					if (ImGui::Selectable("DXT1"))
					{
						texture = rage::grcTextureFactory::GetInstance()->CreateFromDDS(curr_buff, rage::grcImage::Format::DXT1);
					}
					if (ImGui::Selectable("DXT3"))
					{
						texture = rage::grcTextureFactory::GetInstance()->CreateFromDDS(curr_buff, rage::grcImage::Format::DXT3);
					}
					if (ImGui::Selectable("DXT5"))
					{
						texture = rage::grcTextureFactory::GetInstance()->CreateFromDDS(curr_buff, rage::grcImage::Format::DXT5);
					}

					if (texture)
					{
						ptr->m_pTextures[i] = texture;
					}
					ImGui::EndPopup();
				}

				ImGui::TreePop();
			}
		}

	}




	size_t i = F_Index;
	CLensFlareSettings* settings = m_Handler.pCLensFlares->m_Settings;
	atArray<CFlareFX>& arr = settings[i].m_arrFlareFX;

	if (ImGui::CollapsingHeader("CommonSettings"))
	{
		ImGui::DragFloat("Sun Visibility Factor",				&settings[i].m_fSunVisibilityFactor, v_speed);
		ImGui::DragInt("Sun Visibility Alpha Clip",				&settings[i].m_iSunVisibilityAlphaClip, 0.5f, 0, 255);
		ImGui::DragFloat("Sun Fog Factor",						&settings[i].m_fSunFogFactor, v_speed);
		ImGui::DragFloat("Exposure Scale Factor",				&settings[i].m_fExposureScaleFactor, v_speed);
		ImGui::DragFloat("Exposure Intensity Factor",			&settings[i].m_fExposureIntensityFactor, v_speed);
		ImGui::DragFloat("Exposure Rotation Factor",			&settings[i].m_fExposureRotationFactor, v_speed);
		ImGui::DragFloat("Chromatic TexU Size",					&settings[i].m_fChromaticTexUSize, v_speed);
		ImGui::DragFloat("Chromatic Fade Factor",				&settings[i].m_fChromaticFadeFactor, v_speed);
		ImGui::DragFloat("Artefact Distance Fade Factor",		&settings[i].m_fArtefactDistanceFadeFactor, v_speed);
		ImGui::DragFloat("Artefact Rotation Factor",			&settings[i].m_fArtefactRotationFactor, v_speed);
		ImGui::DragFloat("Artefact Scale Factor",				&settings[i].m_fArtefactScaleFactor, v_speed);
		ImGui::DragFloat("Artefact Gradient Factor",			&settings[i].m_fArtefactGradientFactor, v_speed);
		ImGui::DragFloat("Corona Distance Additional Size",		&settings[i].m_fCoronaDistanceAdditionalSize, v_speed);
		ImGui::DragFloat("Min Exposur eScale",					&settings[i].m_fMinExposureScale, v_speed);
		ImGui::DragFloat("Max Exposure Scale",					&settings[i].m_fMaxExposureScale, v_speed);
		ImGui::DragFloat("Min Exposure Intensity",				&settings[i].m_fMinExposureIntensity, v_speed);
		ImGui::DragFloat("Max Exposure Intensity",				&settings[i].m_fMaxExposureIntensity, v_speed);
	}

	
	if (ImGui::CollapsingHeader("FlareFX Array"))
	{
		ImGui::Separator();

		if (ImGui::BeginChild("##FlareFxChild", {  -FLT_MIN,( ImGui::GetContentRegionAvail().y - ImGui::GetFrameHeight() * 1.5f )}, 0, ImGuiWindowFlags_HorizontalScrollbar))
		{
			float ThirdColumnWidth = ImGui::CalcTextSize(TextureNames[TextureWithMaxNameLength_Idx]).x * 2;
			float DublicateButtonWidth = ImGui::CalcTextSize("Duplicate").x * 1.5;
			float RemoveButtonWidth = ImGui::CalcTextSize("Remove").x * 1.5;

			for (size_t j = 0; j < arr.size(); j++)
			{
				CFlareFX& CurrFlareFx = arr[j];

				ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 4.0f, 0.0f });
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.0f, 2.0f });

				if (ImGui::BeginTable("TableFlares", 4, ImGuiTableFlags_BordersV))
				{
					ImGui::TableSetupColumn("_first", ImGuiTableColumnFlags_WidthStretch);
					ImGui::TableSetupColumn("_second", ImGuiTableColumnFlags_WidthFixed, ThirdColumnWidth);
					ImGui::TableSetupColumn("_third", ImGuiTableColumnFlags_WidthFixed, RemoveButtonWidth);
					ImGui::TableSetupColumn("_fourth", ImGuiTableColumnFlags_WidthFixed, DublicateButtonWidth);

					///////////////////////////////////////////////////////////////////////////////////////////////
					ImGui::TableNextColumn(); // 1
					bool isOpened = ImGui::TreeNode((void*)(intptr_t)j, "Index : %i", j);
					if (ImGui::IsItemHovered())
					{
						m_Handler.m_DebugOverlay.SetThicknessForFlareCircleByIndex(j, 5);
					}

					///////////////////////////////////////////////////////////////////////////////////////////////
					PushStyleCompact(0.5f);
					ImGui::TableNextColumn(); // 2

					int NumTexture = CurrFlareFx.m_nTexture;
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
					if (ImGui::Combo(vfmt("##Type{}{}", i, j), &NumTexture, TextureNames.data(), TextureNames.size()))
					{
						if (CurrFlareFx.m_nTexture != NumTexture)
							CurrFlareFx.ResetToDefaultByType(static_cast<FlareFxTextureType_e>(NumTexture));
					}

					///////////////////////////////////////////////////////////////////////////////////////////////
					ImGui::TableNextColumn(); // 3
					if (ImGui::Button("Remove", { RemoveButtonWidth ,0 }))
					{
						arr.remove_at(j);
					}
					
					///////////////////////////////////////////////////////////////////////////////////////////////
					ImGui::TableNextColumn(); // 4
					if (ImGui::Button("Duplicate", { DublicateButtonWidth ,0 }))
					{
						arr.insert(j + 1, CFlareFX(CurrFlareFx));
					}
					PopStyleCompact();

					if (isOpened)
					{
						ImGui::TreePop();
						ImGui::EndTable();
						ImGui::Separator();

						ImGui::PopStyleVar();


						switch (static_cast<FlareFxTextureType_e>(CurrFlareFx.m_nTexture))
						{
						case AnimorphicFx:

							TreeNodeForAnimorphicType(CurrFlareFx, i, j);
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


						ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.0f, 2.0f });
						ImGui::Separator();
					}
					else
					{
						ImGui::EndTable();
					}
				}
				ImGui::PopStyleVar();
				ImGui::PopStyleVar();
			}
			ImGui::Separator();

			ImGui::EndChild();
		}
		
		ImGui::Separator();

		if (ImGui::Button("Append Item"))
		{
			arr.push_back(CFlareFX());
		}
		ImGui::SameLine();
		if (ImGui::Button("Clear"))
		{
			arr.clear();
		}
		ImGui::SameLine();
		if (ImGui::Button("Sort by distance"))
		{
			m_Handler.SortFlaresByDistance(arr);
		}
	}

}


void LensFlareUi::importData(std::filesystem::path path)
{
	m_Handler.m_XmlParser.ImportData(path, m_Handler.pCLensFlares->m_Settings[m_Handler.pCLensFlares->m_ActiveSettingsIndex]);
}

void LensFlareUi::exportData(std::filesystem::path path)
{
	m_Handler.m_XmlParser.ExportData(path, m_Handler.pCLensFlares->m_Settings[m_Handler.pCLensFlares->m_ActiveSettingsIndex]);
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
	ImGui::DragFloat(vfmt("Intensity Scale ##{} {}", i, j),			&CurrFlareFx.m_fIntensityScale, 0.2500000000, 0.0f, 0.0f, "%.4f");
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
	ImGui::DragFloat(vfmt("Rotate##{} {}", i, j),					&CurrFlareFx.m_fWidthRotate, 0.0015000070, 0.0f, 0.0f, "%.4f");
	ImGui::DragFloat(vfmt("Intensity Scale ##{} {}", i, j),			&CurrFlareFx.m_fIntensityScale, 0.500000000f, 0.0f, 0.0f, "%.4f");
	
	CurrFlareFx.m_color.Getf_col3().ToArray(col);
	if (ImGui::ColorEdit3(vfmt("Color ##{} {}", i, j), col))
		CurrFlareFx.m_color.Setf_col3(col);
}



//static ComPtr<IWICImagingFactory> g_imagingFactory;

	//if (!g_imagingFactory)
	//{
	//	HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory1, nullptr, CLSCTX_INPROC_SERVER, __uuidof(IWICImagingFactory), (void**)g_imagingFactory.GetAddressOf());
	//}

	//ComPtr<IWICBitmapDecoder> decoder;

	//wstr.clear();
	//wstr.append(_buff, _buff + strlen(_buff));

	//HRESULT hr = g_imagingFactory->CreateDecoderFromFilename(wstr.c_str(), NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, decoder.GetAddressOf());

	//if (FAILED(hr))
	//{
	//	LogInfo("Failed to create decoder from filename");
	//}

	//ComPtr<IWICBitmapFrameDecode> frame;

	//hr = decoder->GetFrame(0, frame.GetAddressOf());

	//if (FAILED(hr))
	//{
	//	LogInfo("Failed to get frame");
	//}

	//UINT width, height;

	//hr = frame->GetSize(&width, &height);


	//if (FAILED(hr))
	//{
	//	LogInfo("Failed to get size");
	//}

	//ComPtr<IWICFormatConverter> converter;

	//hr = g_imagingFactory->CreateFormatConverter(converter.GetAddressOf());

	//if (FAILED(hr))
	//{
	//	LogInfo("Failed to create format converter");
	//}

	//hr = converter->Initialize(frame.Get(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0, WICBitmapPaletteTypeCustom);

	//if (FAILED(hr))
	//{
	//	LogInfo("Failed to initialize converter");
	//}

	//UINT stride = width * 4;

	//UINT size = stride * height;

	//std::vector<BYTE> buffer(size);

	//hr = converter->CopyPixels(NULL, stride, size, buffer.data());

	//if (FAILED(hr))
	//{
	//	LogInfo("Failed to copy pixels");
	//}

	//if (SUCCEEDED(hr))
	//{