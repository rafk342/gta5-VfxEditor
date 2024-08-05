#pragma once
#include <iostream>
#include "stringzilla/include.h"
#include "rage/math/vec.h"
#include "rage/atl/atArray.h"
#include "helpers/helpers.h"

#include "imgui.h"

#include "Particles/ptxUiUniqueNums.h"


enum ptxKeyframeType
{
	PTX_KF_FLOAT,
	PTX_KF_FLOAT2,
	PTX_KF_FLOAT3,
	PTX_KF_FLOAT4,
	PTX_KF_COL3,
	PTX_KF_COL4,
};

struct ptxKeyframeEntry
{
	float vTime[4]{};
	float vValue[4]{};

	ptxKeyframeEntry() = default;
	ptxKeyframeEntry(float time, rage::Vec4V values)
	{
		vTime[0] = time;
		for (int i = 0; i < 4; ++i) {
			vValue[i] = values[i];
		}
	}
};

class ptxKeyframe
{
public:
	atArray<ptxKeyframeEntry> data;
private:
	u8 pad[16]{};
};


inline void ptxKeyframeTable(ptxKeyframe& ptxKf,
	size_t EntryIdx,
	ptxKeyframeType type,
	const char* lb1 = "Unnamed1", 
	const char* lb2 = "Unnamed2", 
	const char* lb3 = "Unnamed3", 
	const char* lb4 = "Unnamed4")
{
	if (ImGui::TreeNode(vfmt("Keyframes ## fkProp {}", EntryIdx)))
	{
		auto& keyframes = ptxKf.data;

		size_t num_columns =
			type == PTX_KF_FLOAT ? 1 :
			type == PTX_KF_FLOAT2 ? 2 :
			type == PTX_KF_FLOAT3 ? 3 :
			type == PTX_KF_FLOAT4 ? 4 :
			type == PTX_KF_COL3 ? 1 :
			type == PTX_KF_COL4 ? 1 : 0;


		if (ImGui::BeginTable(vfmt("KeyframeTable## kfProp {}", NextUniqueNum()), num_columns + 1, ImGuiTableFlags_Borders))
		{
			//==========================================================================
			std::array labels = { lb1, lb2, lb3, lb4 };

			for (size_t i = 0; i < num_columns; i++)
				ImGui::TableSetupColumn(labels[i]);

			ImGui::TableSetupColumn("##col_last", ImGuiTableColumnFlags_WidthFixed, ImGui::CalcTextSize(" Value ").x * 1.2);

			//==========================================================================

			if (!(type == PTX_KF_COL3 || type == PTX_KF_COL4))
				ImGui::TableHeadersRow();

			//==========================================================================

			for (size_t kf_idx = 0; kf_idx < keyframes.size(); kf_idx++)
			{
				auto& CurrEntry = keyframes[kf_idx];

				ImGui::TableNextRow();

				for (size_t columnIdx = 0; columnIdx < num_columns; columnIdx++)
				{
					ImGui::TableNextColumn();
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
					ImGui::DragFloat(vfmt("##Time kfProp {}", NextUniqueNum()), &CurrEntry.vTime[columnIdx], 0.01);
				}

				ImGui::TableNextColumn();
				ImGui::Text(" Time ");

				for (size_t columnIdx = 0; columnIdx < num_columns; columnIdx++)
				{
					ImGui::TableNextColumn();
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

					const char* vValue_Label = vfmt("##Value kfProp {}", NextUniqueNum());

					if (type == PTX_KF_COL3)
					{
						ImGui::ColorEdit3(vValue_Label, CurrEntry.vValue, ImGuiColorEditFlags_DisplayHSV | ImGuiColorEditFlags_HDR);
					}
					else if (type == PTX_KF_COL4)
					{
						ImGui::ColorEdit4(vValue_Label, CurrEntry.vValue, 0
							| ImGuiColorEditFlags_DisplayHSV
							| ImGuiColorEditFlags_AlphaPreview
							| ImGuiColorEditFlags_AlphaBar
							| ImGuiColorEditFlags_HDR);
					}
					else
					{
						ImGui::DragFloat(vValue_Label, &CurrEntry.vValue[columnIdx], 0.01);
					}
				}

				ImGui::TableNextColumn();
				ImGui::Text(" Value ");
			}

			ImGui::EndTable();
		}
		if (ImGui::Button("Append Item"))
		{
			keyframes.push_back(ptxKeyframeEntry());
		}

		if (ImGui::Button("Clear"))
		{
			keyframes.clear();
		}

		if (ImGui::Button("Pop back"))
		{
			keyframes.pop_back();
		}


		ImGui::TreePop();
	}
}
