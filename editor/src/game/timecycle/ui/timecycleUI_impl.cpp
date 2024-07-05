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


TimecycleUI::TimecycleUI(const char* title) : App(title)
{
	m_CurrentCycle = m_tcHandler.GetCycle(0);
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
	m_CategoriesUsage = Preload::Get()->getConfigParser()->GetBoolean("Settings","Categories",false);
}


void TimecycleUI::MainParamsWindow()
{
	GetCurrentTimeSample(CClock::GetCurrentHour());

	PushStyleCompact();
	WeatherAndRegions();
	PopStyleCompact();

	ImGui::Separator();

	if (m_CategoriesUsage){
		MainParamsWindow_with_Categories();
	} else {
		MainParamsWindow_without_Categories();
	}
}


void TimecycleUI::WeatherAndRegions()
{
	static std::vector<const char*> weather_names;
	static bool init = false;

	if (!init)
	{
		GAMEPLAY::_GET_CURRENT_WEATHER_TYPE();
		GAMEPLAY::SET_OVERRIDE_WEATHER((char*)m_tcHandler.GetCycleName(m_CurrentWeatherIndex).c_str());
		for (size_t i = 0; i < m_tcHandler.GetWeatherNamesVec().size(); i++) {
			weather_names.push_back(m_tcHandler.GetWeatherNamesVec()[i].c_str());
		}
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

		if (ImGui::Combo("Weather", (int*)&m_CurrentWeatherIndex, weather_names.data(), weather_names.size()))
		{
			m_CurrentCycle = m_tcHandler.GetCycle(m_CurrentWeatherIndex);
			GAMEPLAY::SET_OVERRIDE_WEATHER((char*)m_tcHandler.GetCycleName(m_CurrentWeatherIndex).c_str());
		}

		ImGui::TableNextColumn();

		if (ImGui::Button("AppSettings"))
			ImGui::OpenPopup("settingsToggle");

		if (ImGui::BeginPopup("settingsToggle"))
		{
			ImGui::MenuItem("Edit both regions", "", &m_EditBothRegions);
			ImGui::MenuItem("Show only the current sample", "", &m_ShowOnlyTheCurrentSample);
			ImGui::MenuItem("Edit all time samples", "", &m_EditAllTimeSamples);

			ImGui::EndPopup();
		}


		ImGui::TableNextRow();
		ImGui::TableNextColumn();

		if (!(result < 50)) {
			ImGui::SetNextItemWidth(result);
		}

		ImGui::Combo("Regions", (int*)&m_CurrentRegion, regions.data(), regions.size());

		ImGui::EndTable();
	}
}


void TimecycleUI::GetCurrentTimeSample(int current_hour)
{
	for (size_t i = 0; i < time_samples.size(); i++)
	{
		if (current_hour <= time_samples[i].first)
		{
			if (current_hour == time_samples[i].first) {
				m_CurrentTimeSample = i;
				return;
			}
			m_CurrentTimeSample = i - 1;
			return;
		}
		if (current_hour > 22) {
			m_CurrentTimeSample = 12;
			return;
		}
	}
}


void TimecycleUI::MainParamsWindow_without_Categories()
{
	for (size_t i = 0; i < TCVAR_NUM; i++)
	{			
		if (g_varInfos[i].varType == tcVarType_e::VARTYPE_NONE)
			continue;

		if (ImGui::TreeNode(g_varInfos[i].menuName))
		{
			if (m_ShowOnlyTheCurrentSample) {
				makeJustSingleParamWidget(m_CurrentRegion, i);
			} else {
				makeTable(m_CurrentRegion, i);
			}

			ImGui::TreePop();
		}
	}
}


void TimecycleUI::MainParamsWindow_with_Categories()
{
	static char buff[128];
	static auto& categoryNames = Preload::Get()->getTcCategoriesHandler()->getCategoriesOrder();
	static auto& categoriesMap = Preload::Get()->getTcCategoriesHandler()->getCategoriesMap();

//					category name -> vec ( pair (new param_name , varId))
//std::unordered_map<std::string, std::vector<std::pair<std::string, int>>> CategoriesMap;

	for (auto& category : categoryNames)
	{
		if (ImGui::CollapsingHeader(vfmt("{}##ChTc0", category)))
		{
			for (auto& [param_name, id] : categoriesMap.at(category))
			{
				if (g_varInfos[id].varType == tcVarType_e::VARTYPE_NONE)
					continue;

				if (ImGui::TreeNode(g_varInfos[id].menuName))
				{
					if (m_ShowOnlyTheCurrentSample) {
						makeJustSingleParamWidget(m_CurrentRegion, id);
					} else {
						makeTable(m_CurrentRegion, id);
					}
					ImGui::TreePop();
				}
			}
		}
	}

}


void TimecycleUI::makeTable(Regions region, int VarIndex)
{
	float color[4];
	ImVec2 tableSize = ImGui::GetContentRegionAvail();

	if (ImGui::BeginTable(vfmt("##{}_table_tc", VarIndex), time_samples.size(), TcImguiFlags::table_flags))
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
					const char* text = vfmt("##{}_{}_{}_tableTcItem", static_cast<int>(time), VarIndex, g_varInfos[VarIndex].name);
					

					switch (g_varInfos[VarIndex].varType)
					{
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			//								float	

					case (VARTYPE_FLOAT):
						
						ImGui::SetNextItemWidth(tableSize.x / 13);

						ManageKeyframeValues<GET>(color, region, VarIndex, time);

						if (ImGui::DragFloat(text, color, 0.05f))
						{
							SetParams(color, region, VarIndex, time);
						}
						break;
						
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			//									Vec3

					case (VARTYPE_COL3):

						ManageKeyframeValues<GET>(color, region, VarIndex, time);

						if (ImGui::ColorEdit3(text, color, TcImguiFlags::color_vec3_flags))
						{
							SetParams(color, region, VarIndex, time);
						}
						break;
						
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//									Vec4

					case (VARTYPE_COL4):			
						
						ManageKeyframeValues<GET>(color, region, VarIndex, time);

						if (ImGui::ColorEdit4(text, color, TcImguiFlags::color_vec4_flags))
						{
							SetParams(color, region, VarIndex, time);
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
	float color[4];
	size_t time = m_CurrentTimeSample;
	const char* text = vfmt("{} VarId : {}##_{}_var", time_samples[m_CurrentTimeSample].second, VarIndex, static_cast<int>(time));

	switch (g_varInfos[VarIndex].varType)
	{
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//								float

	case (VARTYPE_FLOAT):

		ManageKeyframeValues<GET>(color, region, VarIndex, time);

		if (ImGui::DragFloat(text, color, 0.05f))
		{
			SetParams(color, region, VarIndex, time);
		}
		break;

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//									Vec3    

	case (VARTYPE_COL3):

		ManageKeyframeValues<GET>(color, region, VarIndex, time);
		
		//ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::ColorEdit3(text, color, TcImguiFlags::color_vec3_flags_single))
		{
			SetParams(color, region, VarIndex, time);
		}
		break;

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//									Vec4

	case (VARTYPE_COL4):

		ManageKeyframeValues<GET>(color, region, VarIndex, time);
		
		if (ImGui::ColorEdit4(text, color, TcImguiFlags::color_vec4_flags_single))
		{
			SetParams(color, region, VarIndex, time);
		}
		break;

	default:
		break;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////


template<TimecycleUI::Action action>
void TimecycleUI::ManageKeyframeValues(float* color, Regions region, int VarIndex, size_t time)
{
	u8 n =	g_varInfos[VarIndex].varType == VARTYPE_COL3  ? 3 :
			g_varInfos[VarIndex].varType == VARTYPE_COL4  ? 4 :
			g_varInfos[VarIndex].varType == VARTYPE_FLOAT ? 1 : 0;

	for (u8 i = 0; i < n; i++)
	{
		switch (action)
		{
		case TimecycleUI::Action::SET:
			m_CurrentCycle->SetKeyframeValue(region, g_varInfos[VarIndex + i].varId, time, color[i]);
			break;
		case TimecycleUI::Action::GET:
			color[i] = m_CurrentCycle->GetKeyframeValue(region, g_varInfos[VarIndex + i].varId, time);
			break;
		default:
			break;
		}
	}
}


void TimecycleUI::setValuesForAllTimeSamples(float* color, Regions region, int VarIndex)
{
	for (u8 time = 0; time < 13; time++)
		ManageKeyframeValues<SET>(color, region, VarIndex, time);
}


void TimecycleUI::SetParams(float* color, Regions region, int VarIndex, size_t time)
{
	Regions other_region = static_cast<Regions>(!(static_cast<bool>(region)));
	
	ManageKeyframeValues<SET>(color, region, VarIndex, time);
	
	if (m_EditAllTimeSamples) 
		setValuesForAllTimeSamples(color, region, VarIndex);
	
	if (m_EditBothRegions) //other region 
	{
		ManageKeyframeValues<SET>(color, other_region, VarIndex, time);	
		
		if (m_EditAllTimeSamples)  
			setValuesForAllTimeSamples(color, other_region, VarIndex);
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////

void TimecycleUI::window()
{
	MainParamsWindow();
}

void TimecycleUI::importData(std::filesystem::path path)
{
	m_XmlParser.load_tcData(path, this->m_CurrentCycle);
}

void TimecycleUI::exportData(std::filesystem::path path)
{
	m_XmlParser.export_tcData(path, this->m_CurrentCycle, m_tcHandler.GetCycleName(this->m_CurrentWeatherIndex));
}

