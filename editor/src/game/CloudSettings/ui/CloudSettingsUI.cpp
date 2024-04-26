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


CloudSettingsUI::CloudSettingsUI(BaseUiWindow* base, const char* label) :
	App(base, label),
	CloudsVec(mCloudsHandler.GetCloudSettingsVec())
{
	auto& gHatsNames = mCloudsHandler.GetCloudHatNamesArray();
	CloudHatNames.reserve(gHatsNames.GetSize() + 1);	

	for (size_t i = 0; i < gHatsNames.GetSize(); i++)
	{
		CloudHatNames.push_back(gHatsNames[i].Name);
	}

	CloudHatNames.push_back("NONE");

	time_samples = {
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

	time_array = {	
		"00:00" , "04:00" , "05:00" , 
		"06:00" , "07:00" , "10:00" , 
		"12:00" , "16:00" , "17:00" , 
		"18:00" , "19:00" , "20:00" ,
		"21:00" , "22:00" , "24:00" 
	};
}


void CloudSettingsUI::GetCurrentTimeSample(int current_hour)
{
	static int i = 0;

	for (i = 0; i < time_samples.size(); i++)
	{
		if (current_hour <= time_samples[i].first)
		{
			if (current_hour == time_samples[i].first) {
				current_time_sample_idx = i;
				return;
			}
			current_time_sample_idx = i - 1;
			return;
		}
		if (current_hour > 22) {
			current_time_sample_idx = time_samples.size() - 1;
			return;
		}
	}
	return;
}


void CloudSettingsUI::ParamsWindow()
{
	static char buff[64];
	static auto& arr = mCloudsHandler.GetCloudHatNamesArray();
	static int CloudsComboIndex;
	
	GetCurrentTimeSample(CClock::GetCurrentHour());

	current_hat = mCloudsHandler.GetActiveCloudhatIndex();
	if (current_hat == -1) {
		CloudsComboIndex = 20;
	} else {
		CloudsComboIndex = current_hat;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////

	PushStyleCompact();

	static float regionWidth;
	static float buttonWidth = 100;

	regionWidth = ImGui::GetContentRegionAvail().x;

	if (ImGui::BeginTable("##VScltable", 2, ImGuiTableFlags_NoSavedSettings, { regionWidth, 0 }))
	{
		ImGui::TableSetupColumn("_Clfirst", ImGuiTableColumnFlags_WidthFixed, regionWidth - buttonWidth);
		ImGui::TableSetupColumn("_Clsecond", ImGuiTableColumnFlags_WidthStretch);

		ImGui::TableNextColumn();
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 110);

		if (ImGui::Combo("Active CloudHat", &CloudsComboIndex, CloudHatNames.data(), CloudHatNames.size()))
		{
			if (CloudsComboIndex != current_hat) {
				mCloudsHandler.RequestCloudHat(CloudHatNames[CloudsComboIndex], 0.1f);
			}
		}

		ImGui::TableNextColumn();

		if (ImGui::Button("AppSettings"))
			ImGui::OpenPopup("settingsToggle");
		
		if (ImGui::BeginPopup("settingsToggle"))
		{
			ImGui::MenuItem("Show only the current sample", "", &ShowOnlyTheCurrentSample);
			ImGui::EndPopup();
		}

		ImGui::EndTable();
	}

	PopStyleCompact();
	
	ImGui::Separator();

	for (int i = 0; i < CloudsVec.size(); i++)
	{
		if (ImGui::CollapsingHeader(CloudsVec[i].str_name, ImGuiTreeNodeFlags_None))
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
	static bool bitFlag;
	static char buf[128];
	static u32 CurrentBits;
	static const int cols_count = 3;
	static float width_arr[cols_count] = { 128.5, 150, 0 };
	static int row = 0;
	static int col = 0;


	FORMAT_TO_BUFF(buf, "Probabilities ##TrPr{}", clIdx);
	if (ImGui::TreeNode(buf))
	{
		CurrentBits = CloudsVec[clIdx].bits.to_ulong();

		FORMAT_TO_BUFF(buf, "Binary bitset : {:021b}", CurrentBits);
		ImGui::Text(buf);

		FORMAT_TO_BUFF(buf, "Hex bitset : 0x{:08X}", CurrentBits);
		ImGui::Text(buf);

		PushStyleCompact(0.7);

		FORMAT_TO_BUFF(buf, "##prob_t_{}", clIdx);
		if (ImGui::BeginTable(buf, cols_count, CloudsImguiFlags::table_flags2 | ImGuiTableFlags_NoHostExtendX))
		{
			ImGui::TableSetupColumn(" Name", ImGuiTableColumnFlags_WidthFixed, width_arr[0]);
			ImGui::TableSetupColumn(" Probability ", ImGuiTableColumnFlags_WidthFixed, width_arr[1]);
			ImGui::TableSetupColumn("Enabled ", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableHeadersRow();

			for ( row = 0; row < CloudsVec[clIdx].CloudSettings->probability_array.GetSize(); row++)
			{
				ImGui::TableNextRow();

				for ( col = 0; col < cols_count; col++)
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

						FORMAT_TO_BUFF(buf, "##_{}_{}_CloudHat_index", row, clIdx);

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						
						if (ImGui::InputInt(buf, &CloudsVec[clIdx].CloudSettings->probability_array[row], 1, 5))
						{
							int& var = CloudsVec[clIdx].CloudSettings->probability_array[row];
							if (var < 0) 
								var = 0;
							if (var > 500) 
								var = 500;
						}
						
						break;
					case (2):

						FORMAT_TO_BUFF(buf, "##{}_{}_bitset_flag", row, clIdx);

						bitFlag = CloudsVec[clIdx].bits.test(row);

						ImGui::SameLine();
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 5);

						if (ImGui::Checkbox(buf, &bitFlag))
						{
							CloudsVec[clIdx].bits.set(row, bitFlag);
							CloudsVec[clIdx].CloudSettings->bits.setRawPtrData(CloudsVec[clIdx].bits.to_ulong());
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
			nextIdx = mCloudsHandler.GetNewRandomCloudhatIndex();
			mCloudsHandler.RequestCloudHat(CloudHatNames[nextIdx], 0.1f);
		}
		ImGui::Text("Selected index : %i" , nextIdx);
		
		ImGui::TreePop();
	}
}


void CloudSettingsUI::CloudDataTreeNode(const char* label, int clIdx,atArray<ptxKeyframeEntry>& arr, std::function<void(atArray<ptxKeyframeEntry>&, int, const char*)> func)
{
	static char buf[128];

	FORMAT_TO_BUFF(buf, "{}##{}_treeNode", label, clIdx);
	if (ImGui::TreeNode(buf))
	{
		func(arr, clIdx, label);
		ImGui::TreePop();
	}
}


void CloudSettingsUI::CloudsDataWidgets(int clIdx)
{
	auto ColoursFunc = [this](atArray<ptxKeyframeEntry>& arr, int table_id, const char* param_name)
		{
			if (ShowOnlyTheCurrentSample) {
				CloudSettingsColourSimpleParam(arr, table_id, param_name);
			} else {
				CloudSettingsColourTable(arr, table_id, param_name);
			}
		};

	auto VariablesFunc = [this](atArray<ptxKeyframeEntry>& arr, int table_id, const char* param_name)
		{
			if (ShowOnlyTheCurrentSample) {
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
		clIdx, CloudsVec[clIdx].CloudSettings->m_CloudScaleFillColors.data,	
		VariablesFunc);

	CloudDataTreeNode("Density Shift -> Scale / Scattering Constant -> Scale",
		clIdx, CloudsVec[clIdx].CloudSettings->m_CloudDensityShift_Scale_ScatteringConst_Scale.data, 
		VariablesFunc);
	
	CloudDataTreeNode("Piercing Light Power / Strength / Normal Strength / Thickness",
		clIdx, CloudsVec[clIdx].CloudSettings->m_CloudPiercingLightPower_Strength_NormalStrength_Thickness.data, 
		VariablesFunc);
	
	CloudDataTreeNode("Scale Diffuse / Scale Fill / Scale Ambient / Wrap Amount",
		clIdx, CloudsVec[clIdx].CloudSettings->m_CloudScaleDiffuseFillAmbient_WrapAmount.data, 
		VariablesFunc);

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void CloudSettingsUI::CloudSettingsColourTable(atArray<ptxKeyframeEntry>& arr, int table_id, const char* param_name)
{
	static char color_label[128];
	static char table_label[128];
	static int time;
	static int i;

	FORMAT_TO_BUFF(table_label, "##table_{}_{}", table_id, param_name);
	
	if (ImGui::BeginTable(table_label, time_array.size() - 1, CloudsImguiFlags::table_flags))
	{
		for (i = 0; i < 2; i++)
		{
			ImGui::TableNextRow();

			for (time = 0; time < time_array.size() - 1; time++) 
			{
				ImGui::TableNextColumn();

				if (i == 0)
				{
					ImGui::Text(time_array[time]);
				}
				if (i == 1)
				{
					FORMAT_TO_BUFF(color_label, "##_{}_{}_{}_col", arr[time].vTime[0],table_id ,param_name);
					
					if (ImGui::ColorEdit4(color_label, arr[time].vValue, CloudsImguiFlags::color_vec4_flags))
					{
						if (time == 0) { std::copy(arr[time].vValue, arr[time].vValue + 4, arr[14].vValue); }
					}
				}
			}
		}
		ImGui::EndTable();
	}
}


void CloudSettingsUI::CloudSettingsColourSimpleParam(atArray<ptxKeyframeEntry>& arr, int table_id, const char* param_name)
{
	static char color_label[128];
	static u8& time = current_time_sample_idx;

	FORMAT_TO_BUFF(color_label, "{}##_{}_{}_col", time_samples[time].second, table_id, param_name);

	if (ImGui::ColorEdit4(color_label, arr[time].vValue, CloudsImguiFlags::color_vec4_flags_single))
	{
		if (time == 0) { std::copy(arr[time].vValue, arr[time].vValue + 4, arr[14].vValue); }
	}
}


void CloudSettingsUI::CloudSettingsVariablesTable(atArray<ptxKeyframeEntry>& arr, int table_id, const char* param_name)
{
	static char var_label[128];
	static char table_label[128];
	static int time;
	static int i;
	static float table_width;
	static float time_data_width = 55;

	FORMAT_TO_BUFF(table_label, "##table_{}_{}", table_id, param_name);

	table_width = ImGui::GetContentRegionAvail().x;

	if (ImGui::BeginTable(table_label, 2, CloudsImguiFlags::table_flags2))
	{
		ImGui::TableSetupColumn("One", ImGuiTableColumnFlags_WidthFixed, 55);

		for (time = 0; time < time_array.size() - 1; time++)
		{
			ImGui::TableNextRow();
			
			for (i = 0; i < 2; i++)
			{
				ImGui::TableNextColumn();

				if (i == 0)
				{
					ImGui::Text(time_array[time]);
				}
				if (i == 1)
				{
					PushStyleCompact(0.7);

					FORMAT_TO_BUFF(var_label, "##{}_{}_{}_var", arr[time].vTime[0], table_id ,param_name);
				
					ImGui::SetNextItemWidth(table_width - time_data_width);
		
					if (ImGui::DragFloat4(var_label, arr[time].vValue, 0.015))
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
	static char var_label[128];
	static u8& time = current_time_sample_idx;

	FORMAT_TO_BUFF(var_label, "{}##_{}_{}_var_", time_samples[time].second, table_id, param_name);

	if (ImGui::DragFloat4(var_label, arr[time].vValue, 0.015))
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
	mXmlParser.ImportCloudKfData(path, mCloudsHandler);
}

void CloudSettingsUI::exportData(std::filesystem::path path)
{
	mXmlParser.ExportCloudKfData(path, mCloudsHandler.GetCloudSettingsVec());
}

