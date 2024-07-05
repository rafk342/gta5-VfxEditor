#include "CloudSettingsUI.h"
#include <thread>

#include "scripthook/inc/natives.h"
#include "scripthook/inc/types.h"
#include "scripthook/inc/enums.h"
#include "scripthook/inc/scripthookMain.h"
#include "Windows.h"
#include "scripthookTh.h"


namespace
{
	struct CloudsImguiFlags
	{
		static const int color_vec4_flags = 0
			| ImGuiColorEditFlags_NoInputs
			| ImGuiColorEditFlags_NoLabel
			| ImGuiColorEditFlags_NoBorder
			| ImGuiColorEditFlags_AlphaBar
			| ImGuiColorEditFlags_Float
			| ImGuiColorEditFlags_AlphaPreview
			| ImGuiColorEditFlags_HDR
			| ImGuiColorEditFlags_DisplayHSV
			| ImGuiColorEditFlags_DisplayHex;

		static const int color_vec4_flags_single = 0
			| ImGuiColorEditFlags_AlphaBar
			| ImGuiColorEditFlags_Float
			| ImGuiColorEditFlags_AlphaPreview
			| ImGuiColorEditFlags_HDR
			| ImGuiColorEditFlags_DisplayHSV;

		static const int table_flags = 0
			| ImGuiTableFlags_Borders
			| ImGuiTableFlags_RowBg
			| ImGuiTableFlags_SizingStretchSame
			| ImGuiTableFlags_ContextMenuInBody
			| ImGuiTableFlags_NoSavedSettings;

		static const int table_flags2 = 0
			| ImGuiTableFlags_Borders
			| ImGuiTableFlags_ContextMenuInBody
			| ImGuiTableFlags_NoSavedSettings
			| ImGuiTableFlags_SizingFixedFit;
	};
}


CloudSettingsUI::CloudSettingsUI(const char* title) : App(title)
{
	m_TimeSamples = {
		{0, "00:00"},
		{4, "04:00"},
		{5, "05:00"},
		{6, "06:00"},
		{7, "07:00"},
		{10, "10:00"},
		{12, "12:00"},
		{16, "16:00"},
		{17, "17:00"},
		{18, "18:00"},
		{19, "19:00"},
		{20, "20:00"},
		{21, "21:00"},
		{22, "22:00"},
	};

	m_TimeArray = {	
		"00:00" , "04:00" , "05:00" , 
		"06:00" , "07:00" , "10:00" , 
		"12:00" , "16:00" , "17:00" , 
		"18:00" , "19:00" , "20:00" ,
		"21:00" , "22:00" , "24:00" 
	};
}


void CloudSettingsUI::GetCurrentTimeSample(int current_hour)
{
	for (size_t i = 0; i < m_TimeSamples.size(); i++)
	{
		if (current_hour <= m_TimeSamples[i].first)
		{
			if (current_hour == m_TimeSamples[i].first) {
				m_CurrentTimeSampleIndex = i;
				return;
			}
			m_CurrentTimeSampleIndex = i - 1;
			return;
		}
		if (current_hour > m_TimeSamples.back().first) {
			m_CurrentTimeSampleIndex = m_TimeSamples.size() - 1;
			return;
		}
	}
	return;
}


void CloudSettingsUI::ParamsWindow()
{
	auto& CloudHatNames = m_CloudsHandler.GetCloudHatNamesArray();
	auto& CloudsVec = m_CloudsHandler.GetCloudSettingsVec();
	int CloudsComboIndex;
	
	GetCurrentTimeSample(CClock::GetCurrentHour());

	int CurrentHat = m_CloudsHandler.GetActiveCloudhatIndex();
	if (CurrentHat == -1) {
		CloudsComboIndex = CloudHatNames.size() - 1;
	} else {
		CloudsComboIndex = CurrentHat;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////

	PushStyleCompact();

	float regionWidth;
	float buttonWidth = 100;

	regionWidth = ImGui::GetContentRegionAvail().x;

	if (ImGui::BeginTable("##VScltable", 2, ImGuiTableFlags_NoSavedSettings, { regionWidth, 0 }))
	{
		ImGui::TableSetupColumn("_Clfirst", ImGuiTableColumnFlags_WidthFixed, regionWidth - buttonWidth);
		ImGui::TableSetupColumn("_Clsecond", ImGuiTableColumnFlags_WidthStretch);

		ImGui::TableNextColumn();
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 110);

		if (ImGui::Combo("Active CloudHat", &CloudsComboIndex, CloudHatNames.data(), CloudHatNames.size()))
		{
			if (CloudsComboIndex != CurrentHat) {
				m_CloudsHandler.RequestCloudHat(CloudHatNames[CloudsComboIndex], 0.1f);
			}
		}

		ImGui::TableNextColumn();

		if (ImGui::Button("AppSettings"))
			ImGui::OpenPopup("settingsToggle");
		
		if (ImGui::BeginPopup("settingsToggle"))
		{
			ImGui::MenuItem("Show only the current sample", "", &m_ShowOnlyTheCurrentSample);
			ImGui::EndPopup();
		}

		ImGui::EndTable();
	}

	PopStyleCompact();
	
	ImGui::Separator();

	for (int i = 0; i < CloudsVec.size(); i++)
	{
		if (ImGui::CollapsingHeader(vfmt("{}", CloudsVec[i].name), ImGuiTreeNodeFlags_None))
		{
			ImGui::SeparatorText("CloudList");
			ProbabilityWidgets(i);
			ImGui::SeparatorText("CloudSettings");
			CloudsDataWidgets(i);
		}
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void CloudSettingsUI::ProbabilityWidgets(int clIdx)
{
	auto& CloudHatNames = m_CloudsHandler.GetCloudHatNamesArray();
	auto& CloudsVec = m_CloudsHandler.GetCloudSettingsVec();

	bool bitFlag;
	const int cols_count = 3;
	float width_arr[cols_count] = { 128.5, 150, 0 };


	if (ImGui::TreeNode(vfmt("Probabilities ##TrPr{}", clIdx)))
	{
		u32 CurrentBits = *CloudsVec[clIdx].CloudSettings->m_Bits.data();

		ImGui::Text(vfmt("Binary bitset view : {:032b}", CurrentBits));
		ImGui::Text(vfmt("Hex bitset view : 0x{:08X}", CurrentBits));

		PushStyleCompact(0.7);

		if (ImGui::BeginTable(vfmt("##prob_t_{}", clIdx), cols_count, CloudsImguiFlags::table_flags2 | ImGuiTableFlags_NoHostExtendX))
		{
			ImGui::TableSetupColumn(" Name", ImGuiTableColumnFlags_WidthFixed, width_arr[0]);
			ImGui::TableSetupColumn(" Probability ", ImGuiTableColumnFlags_WidthFixed, width_arr[1]);
			ImGui::TableSetupColumn("Enabled ", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableHeadersRow();

			for (size_t row = 0; row < CloudsVec[clIdx].CloudSettings->m_ProbabilitiesArray.size(); row++)
			{
				ImGui::TableNextRow();

				for (size_t col = 0; col < cols_count; col++)
				{
					ImGui::TableNextColumn();

					switch (col)
					{
					case (0):

						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 3);

						ImGui::Text("%i", row);
						ImGui::SameLine();
						ImGui::SetCursorPosX(56);
						ImGui::Text(CloudHatNames[row]);

						break;
					case (1):

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::InputInt(vfmt("##_{}_{}_CloudHat_index", row, clIdx), &CloudsVec[clIdx].CloudSettings->m_ProbabilitiesArray[row], 1, 5))
						{
							int& var = CloudsVec[clIdx].CloudSettings->m_ProbabilitiesArray[row];
							if (var < 0) var = 0;
							if (var > 500) var = 500;
						}
						
						break;
					case (2):

						bitFlag = CloudsVec[clIdx].CloudSettings->m_Bits.test(row);

						ImGui::SameLine();
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 5);

						if (ImGui::Checkbox(vfmt("##{}_{}_bitset_flag", row, clIdx), &bitFlag))
						{
							CloudsVec[clIdx].CloudSettings->m_Bits.set(row, bitFlag);
						}
						
						break;
					default:
						break;
					}
				}
			}
			ImGui::EndTable();
		}
		PopStyleCompact();
		
		static int nextIdx = 0;
		
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
		ImGui::Text("Request new random CloudHat");

		if (ImGui::Button("PressMe"))
		{
			nextIdx = m_CloudsHandler.GetNewRandomCloudhatIndex();
			m_CloudsHandler.RequestCloudHat(CloudHatNames[nextIdx], 0.1f);
		}
		ImGui::Text("Selected index : %i" , nextIdx);
		
		ImGui::TreePop();
	}
}


void CloudSettingsUI::CloudDataTreeNode(const char* label, int clIdx,atArray<ptxKeyframeEntry>& arr, std::function<void(atArray<ptxKeyframeEntry>&, int, const char*)> func)
{
	if (ImGui::TreeNode(vfmt("{}##{}_treeNode", label, clIdx)))
	{
		func(arr, clIdx, label);
		ImGui::TreePop();
	}
}


void CloudSettingsUI::CloudsDataWidgets(int clIdx)
{
	auto& CloudsVec = m_CloudsHandler.GetCloudSettingsVec();

	auto ColoursFunc = [this](atArray<ptxKeyframeEntry>& arr, int table_id, const char* param_name)
		{
			if (m_ShowOnlyTheCurrentSample) {
				CloudSettingsColourSingleParam(arr, table_id, param_name);
			} else {
				CloudSettingsColourTable(arr, table_id, param_name);
			}
		};

	auto VariablesFunc = [this](atArray<ptxKeyframeEntry>& arr, int table_id, const char* param_name)
		{
			if (m_ShowOnlyTheCurrentSample) {
				CloudSettingsVariablesSingleParam(arr, table_id, param_name);
			} else {
				CloudSettingsVariablesTable(arr, table_id, param_name);
			}
		};


	CloudDataTreeNode("Cloud Color",		clIdx,	CloudsVec[clIdx].CloudSettings->m_CloudColor.data,			ColoursFunc);
	CloudDataTreeNode("Light Color",		clIdx,	CloudsVec[clIdx].CloudSettings->m_CloudLightColor.data,		ColoursFunc);
	CloudDataTreeNode("Ambient Color",		clIdx,	CloudsVec[clIdx].CloudSettings->m_CloudAmbientColor.data,	ColoursFunc);
	CloudDataTreeNode("Sky Fill Color",		clIdx,	CloudsVec[clIdx].CloudSettings->m_CloudSkyColor.data,		ColoursFunc);
	CloudDataTreeNode("Bounce Fill Color",	clIdx,	CloudsVec[clIdx].CloudSettings->m_CloudBounceColor.data,	ColoursFunc);
	CloudDataTreeNode("East Fill Color",	clIdx,	CloudsVec[clIdx].CloudSettings->m_CloudEastColor.data,		ColoursFunc);
	CloudDataTreeNode("West Fill Color",	clIdx,	CloudsVec[clIdx].CloudSettings->m_CloudWestColor.data,		ColoursFunc);
	
	CloudDataTreeNode("Scale Sky Color / Scale Bounce / Scale East Colour / Scale West Colour",		
		clIdx,
		CloudsVec[clIdx].CloudSettings->m_CloudScaleFillColors.data,	
		VariablesFunc);

	CloudDataTreeNode("Density Shift -> Scale / Scattering Constant -> Scale",
		clIdx, 
		CloudsVec[clIdx].CloudSettings->m_CloudDensityShift_Scale_ScatteringConst_Scale.data, 
		VariablesFunc);
	
	CloudDataTreeNode("Piercing Light Power / Strength / Normal Strength / Thickness",
		clIdx, 
		CloudsVec[clIdx].CloudSettings->m_CloudPiercingLightPower_Strength_NormalStrength_Thickness.data, 
		VariablesFunc);
	
	CloudDataTreeNode("Scale Diffuse / Scale Fill / Scale Ambient / Wrap Amount",
		clIdx, 
		CloudsVec[clIdx].CloudSettings->m_CloudScaleDiffuseFillAmbient_WrapAmount.data, 
		VariablesFunc);

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void CloudSettingsUI::CloudSettingsColourTable(atArray<ptxKeyframeEntry>& arr, int table_id, const char* param_name)
{
	if (ImGui::BeginTable(vfmt("##table_{}_{}", table_id, param_name), m_TimeArray.size() - 1, CloudsImguiFlags::table_flags))
	{
		for (size_t i = 0; i < 2; i++)
		{
			ImGui::TableNextRow();

			for (size_t time = 0; time < m_TimeArray.size() - 1; time++) 
			{
				ImGui::TableNextColumn();

				if (i == 0)
				{
					ImGui::Text(m_TimeArray[time]);
				}
				if (i == 1)
				{	
					if (ImGui::ColorEdit4(vfmt("##_{}_{}_{}_col", arr[time].vTime[0], table_id, param_name), arr[time].vValue, CloudsImguiFlags::color_vec4_flags))
					{
						if (time == 0) { std::copy(arr[time].vValue, arr[time].vValue + 4, arr[14].vValue); }
					}
				}
			}
		}
		ImGui::EndTable();
	}
}


void CloudSettingsUI::CloudSettingsColourSingleParam(atArray<ptxKeyframeEntry>& arr, int table_id, const char* param_name)
{
	u8 time = m_CurrentTimeSampleIndex;

	if (ImGui::ColorEdit4(vfmt("{}##_{}_{}_col", m_TimeSamples[time].second, table_id, param_name), arr[time].vValue, CloudsImguiFlags::color_vec4_flags_single))
	{
		if (time == 0) { std::copy(arr[time].vValue, arr[time].vValue + 4, arr[14].vValue); }
	}
}


void CloudSettingsUI::CloudSettingsVariablesTable(atArray<ptxKeyframeEntry>& arr, int table_id, const char* param_name)
{
	float time_data_width = 55;
	float table_width = ImGui::GetContentRegionAvail().x;

	if (ImGui::BeginTable(vfmt("##table_{}_{}", table_id, param_name), 2, CloudsImguiFlags::table_flags2))
	{
		ImGui::TableSetupColumn("One", ImGuiTableColumnFlags_WidthFixed, 55);

		for (size_t time = 0; time < m_TimeArray.size() - 1; time++)
		{
			ImGui::TableNextRow();
			
			for (size_t i = 0; i < 2; i++)
			{
				ImGui::TableNextColumn();

				if (i == 0)
				{
					ImGui::Text(m_TimeArray[time]);
				}
				if (i == 1)
				{
					PushStyleCompact(0.7);
					ImGui::SetNextItemWidth(table_width - time_data_width);

					if (ImGui::DragFloat4(vfmt("##{}_{}_{}_var", arr[time].vTime[0], table_id, param_name), arr[time].vValue, 0.015))
					{
						if (time == 0) { std::copy(arr[time].vValue, arr[time].vValue + 4, arr[14].vValue); }
					}
					PopStyleCompact();
				}
			}
		}
		ImGui::EndTable();
	}
}


void CloudSettingsUI::CloudSettingsVariablesSingleParam(atArray<ptxKeyframeEntry>& arr, int table_id, const char* param_name)
{
	u8 time = m_CurrentTimeSampleIndex;

	if (ImGui::DragFloat4(vfmt("{}##_{}_{}_var_", m_TimeSamples[time].second, table_id, param_name), arr[time].vValue, 0.015))
	{
		if (time == 0) { std::copy(arr[time].vValue, arr[time].vValue + 4, arr[14].vValue); }
	}
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CloudSettingsUI::window()
{
	ParamsWindow();
}

void CloudSettingsUI::importData(std::filesystem::path path)
{
	m_XmlParser.ImportCloudKfData(path, m_CloudsHandler);
}

void CloudSettingsUI::exportData(std::filesystem::path path)
{
	m_XmlParser.ExportCloudKfData(path, m_CloudsHandler);
}

