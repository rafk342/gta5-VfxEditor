#include "timecycleUI_impl.h"


struct TcImguiFlags
{
	static const int color_vec4_flags = 0
		| ImGuiColorEditFlags_NoInputs
		| ImGuiColorEditFlags_NoLabel
		| ImGuiColorEditFlags_NoBorder
		| ImGuiColorEditFlags_AlphaBar
		| ImGuiColorEditFlags_Float
		| ImGuiColorEditFlags_AlphaPreview
		| ImGuiColorEditFlags_HDR;

	static const int color_vec3_flags = 0
		| ImGuiColorEditFlags_NoInputs
		| ImGuiColorEditFlags_NoLabel
		| ImGuiColorEditFlags_NoBorder
		| ImGuiColorEditFlags_Float
		| ImGuiColorEditFlags_HDR;

	static const int color_vec3_flags_single = 0
		| ImGuiColorEditFlags_Float
		| ImGuiColorEditFlags_HDR
		| ImGuiColorEditFlags_DisplayHSV;

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
		| ImGuiTableFlags_NoSavedSettings
		| ImGuiTableFlags_Borders;
};


TimecycleUI::TimecycleUI(BaseUiWindow* base, const char* label) : App(base, label)
{
	currentCycle = m_tcHandler.GetCycle(0);
	regions = { "Global", "Urban" };
	time_samples = {
		 {0 , " 00:00"},
		 {5 , " 05:00"},
		 {6 , " 06:00"},
		 {7 , " 07:00"},
		 {10, " 10:00"},
		 {12, " 12:00"},
		 {16, " 16:00"},
		 {17, " 17:00"},
		 {18, " 18:00"},
		 {19, " 19:00"},
		 {20, " 20:00"},
		 {21, " 21:00"},
		 {22, " 22:00"},
	};
}


void TimecycleUI::WeatherAndRegions()
{
	static auto& weather_names = m_tcHandler.GetWeatherNamesVec();
	static bool init = false;

	if (!init)
	{
		GAMEPLAY::SET_OVERRIDE_WEATHER((char*)m_tcHandler.GetCycleName(current_weather_index).c_str());
		init = true;
	}


	static float regionWidth;
	static float buttonWidth = 100;
	static float result;

	regionWidth = ImGui::GetContentRegionAvail().x;

	if (ImGui::BeginTable("##VStctable", 2, ImGuiTableFlags_NoSavedSettings, { regionWidth, 0 }))
	{
		ImGui::TableSetupColumn("_Tcfirst", ImGuiTableColumnFlags_WidthFixed, regionWidth - buttonWidth);
		ImGui::TableSetupColumn("_Tcsecond", ImGuiTableColumnFlags_WidthStretch);

		ImGui::TableNextColumn();

		result = ImGui::GetContentRegionAvail().x - 70;
		if (!(result < 50)) {
			ImGui::SetNextItemWidth(result);
		}

		if (ImGui::Combo("Weather", (int*)&current_weather_index, weather_names.data(), weather_names.size()))
		{
			currentCycle = m_tcHandler.GetCycle(current_weather_index);
			GAMEPLAY::SET_OVERRIDE_WEATHER((char*)m_tcHandler.GetCycleName(current_weather_index).c_str());
		}

		ImGui::TableNextColumn();

		if (ImGui::Button("AppSettings"))
			ImGui::OpenPopup("settingsToggle");

		if (ImGui::BeginPopup("settingsToggle"))
		{
			ImGui::MenuItem("Edit both regions", "", &edit_both_regions);
			ImGui::MenuItem("Show only the current sample", "", &show_only_current_sample);
			ImGui::MenuItem("Edit all time samples", "", &edit_all_time_samples);

			ImGui::EndPopup();
		}


		ImGui::TableNextRow();
		ImGui::TableNextColumn();

		if (!(result < 50)) {
			ImGui::SetNextItemWidth(result);
		}

		ImGui::Combo("Regions", (int*)&current_region_index, regions.data(), regions.size());

		ImGui::EndTable();
	}
}


void TimecycleUI::GetCurrentTimeSample(int current_hour)
{
	static int i = 0;

	for (i = 0; i < time_samples.size(); i++)
	{
		if (current_hour <= time_samples[i].first)
		{
			if (current_hour == time_samples[i].first) {
				current_time_sample = i;
				return;
			}
			current_time_sample = i - 1;
			return;
		}
		if (current_hour > 22) {
			current_time_sample = 12;
			return;
		}
	}
}


void TimecycleUI::MainParamsWindow()
{
	GetCurrentTimeSample(CClock::GetCurrentHour());

	PushStyleCompact();
	WeatherAndRegions();
	PopStyleCompact();

	ImGui::Separator();

	if (config_params::categories)
		MainParamsWindow_with_Categories();
	else
		MainParamsWindow_without_Categories();
}


void TimecycleUI::MainParamsWindow_without_Categories()
{
	for (size_t i = 0; i < TIMECYCLE_VAR_COUNT; i++)
	{			
		if (g_varInfos[i].varType == tcVarType_e::VARTYPE_NONE)
			continue;

		if (ImGui::TreeNode(g_varInfos[i].labelName))
		{
			if (show_only_current_sample)
				makeJustSingleParamWidget((Regions)current_region_index, i);
			else
				makeTable((Regions)current_region_index, i);

			ImGui::TreePop();
		}
	}
}


void TimecycleUI::MainParamsWindow_with_Categories()
{
	static char buff[128];
	static auto& categoryNames = CategoriesIntegrated::getNamesVec();
	static auto& categoriesMap = CategoriesIntegrated::getCategoriesMap();

//					category name -> vec ( pair (param_label , varId))
//std::unordered_map<std::string, std::vector<std::pair<std::string, int>>> CategoriesMapIngr;

	for (auto& category : categoryNames)
	{
		FORMAT_TO_BUFF(buff, "{}##ChTc0", category)
		if (ImGui::CollapsingHeader(buff))
		{
			for (auto& [param_name, id] : categoriesMap.at(category))
			{
				if (ImGui::TreeNode(g_varInfos[id].labelName))
				{
					if (show_only_current_sample)
						makeJustSingleParamWidget((Regions)current_region_index, id);
					else
						makeTable((Regions)current_region_index, id);

					ImGui::TreePop();
				}
			}
		}
	}

}


void TimecycleUI::makeTable(Regions region, int VarIndex)
{
	static float val;
	static float color[4];
	static ImVec2 tableSize;

	static char buff[128];

	tableSize = ImGui::GetContentRegionAvail();

	FORMAT_TO_BUFF(buff, "##{}_table_tc", VarIndex);
	if (ImGui::BeginTable(buff, time_samples.size(), TcImguiFlags::table_flags))
	{
		for (size_t i = 0; i < 2; i++)
		{
			ImGui::TableNextRow();
			for (size_t time = 0; time < time_samples.size(); time++)
			{
				ImGui::TableNextColumn();

				if (i == 0)
				{
					ImGui::Text(time_samples[time].second);
				}
				else if (i == 1)
				{
					FORMAT_TO_BUFF(buff, "##{}_{}_{}_tableTcItem", static_cast<int>(time), VarIndex, g_varInfos[VarIndex].name);

					switch (g_varInfos[VarIndex].varType)
					{
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			//								float	

					case (VARTYPE_FLOAT):
						
						ImGui::SetNextItemWidth(tableSize.x / 13);

						manageKeyframeValues<GET>((float*)&color, region, VarIndex, time);

						if (ImGui::DragFloat(buff, &color[0], 0.05f))
						{
							handleParams((float*)&color, region, VarIndex, time);
						}
						break;
						
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			//									Vec3

					case (VARTYPE_COL3):

						manageKeyframeValues<GET>((float*)&color, region, VarIndex, time);

						if (ImGui::ColorEdit3(buff, (float*)&color, TcImguiFlags::color_vec3_flags))
						{
							handleParams((float*)&color, region, VarIndex, time);
						}
						break;
						
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//									Vec4

					case (VARTYPE_COL4):			
						
						manageKeyframeValues<GET>((float*)&color, region, VarIndex, time);

						if (ImGui::ColorEdit4(buff, (float*)&color, TcImguiFlags::color_vec4_flags))  
						{
							handleParams((float*)&color, region, VarIndex, time);
						}
						break;
					
					default:
						break;
					}
				}
			}
		}
	}
	ImGui::EndTable();
}


void TimecycleUI::makeJustSingleParamWidget(Regions region, int VarIndex)
{
	static float color[4];
	static size_t time;
	static char buff[128];
	
	time = current_time_sample;

	FORMAT_TO_BUFF(buff, "{} VarId : {}##_{}_var", time_samples[current_time_sample].second, VarIndex, static_cast<int>(time))

	switch (g_varInfos[VarIndex].varType)
	{
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//								float

	case (VARTYPE_FLOAT):

		manageKeyframeValues<GET>((float*)&color, region, VarIndex, time);

		if (ImGui::DragFloat(buff, &color[0], 0.05f))
		{
			handleParams((float*)&color, region, VarIndex, time);
		}
		break;

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//									Vec3    

	case (VARTYPE_COL3):

		manageKeyframeValues<GET>((float*)&color, region, VarIndex, time);
		
		//ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::ColorEdit3(buff, (float*)&color, TcImguiFlags::color_vec3_flags_single))
		{
			handleParams((float*)&color, region, VarIndex, time);
		}
		break;

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//									Vec4

	case (VARTYPE_COL4):

		manageKeyframeValues<GET>((float*)&color, region, VarIndex, time);
		
		if (ImGui::ColorEdit4(buff, (float*)&color, TcImguiFlags::color_vec4_flags_single))
		{
			handleParams((float*)&color, region, VarIndex, time);
		}
		break;

	default:
		break;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////


template<TimecycleUI::Action action>
void TimecycleUI::manageKeyframeValues(float* color, Regions region, int VarIndex, size_t time)
{
	u8 n =	g_varInfos[VarIndex].varType == VARTYPE_COL3  ? 3 :
			g_varInfos[VarIndex].varType == VARTYPE_COL4  ? 4 :
			g_varInfos[VarIndex].varType == VARTYPE_FLOAT ? 1 : 0;

	for (u8 i = 0; i < n; i++)
	{
		switch (action)
		{
		case TimecycleUI::Action::SET:
			currentCycle->SetKeyframeValue(region, g_varInfos[VarIndex + i].varId, time, color[i]);
			break;
		case TimecycleUI::Action::GET:
			color[i] = currentCycle->GetKeyframeValue(region, g_varInfos[VarIndex + i].varId, time);
			break;
		default:
			break;
		}
	}
}


void TimecycleUI::setValuesForAllTimeSamples(float* color, Regions region, int VarIndex)
{
	for (u8 time = 0; time < 13; time++)
		manageKeyframeValues<SET>(color, region, VarIndex, time);
}


void TimecycleUI::handleParams(float* color, Regions region, int VarIndex, size_t time)
{
	Regions other_region = static_cast<Regions>(!(static_cast<bool>(region)));
	
	manageKeyframeValues<SET>(color, region, VarIndex, time);
	
	if (edit_all_time_samples) 
		setValuesForAllTimeSamples(color, region, VarIndex);
	
	if (edit_both_regions) //other region 
	{
		manageKeyframeValues<SET>(color, other_region, VarIndex, time);
		
		if (edit_all_time_samples)  
			setValuesForAllTimeSamples(color, other_region, VarIndex);
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////

void TimecycleUI::window()
{
	MainParamsWindow();
}

void TimecycleUI::importData(std::string path)
{
	m_tcHandler.xmlParser.load_tcData(path, this->currentCycle);
}

void TimecycleUI::exportData(std::string path)
{
	m_tcHandler.xmlParser.export_tcData(path, this->currentCycle, m_tcHandler.GetCycleName(this->current_weather_index));
}

