#pragma once
#include <iostream>
#include <array>
#include <string>
#include <map>
#include <thread>
#include <mutex>

#include "ImGui/imgui.h"
#include "scripthook/inc/natives.h"
#include "scripthook/inc/types.h"
#include "scripthook/inc/enums.h"
#include "scripthook/inc/scripthookMain.h"

#include "game/timecycle/tñHandler.h"
#include "game/timecycle/tcData.h"
#include "game/timecycle/xml/tcXmlParser.h"
#include "game/timecycle/tcdef.h"
#include "game/gameClock/CClock.h"
#include "helpers/helpers.h"
#include "uiBase/ImguiHelpers.h"

#include "app/Preload/Preload_Integration.h"
#include "app/uiBase/uiBaseWindow.h"

#include "Preload/Preload.h"

class TimecycleUI : public App
{
	 timeñycleHandler m_tcHandler;
	
	 std::vector<std::pair<int, const char*>> time_samples;
	 std::array<const char*, 2>	regions;

	 tcCycle*	currentCycle;
	 int		current_weather_index = 0;
	 Regions	current_region = GLOBAL;
	 bool		edit_both_regions = false;
	 bool		edit_all_time_samples = false;
	 bool		time_override = false;
	 bool		show_only_current_sample = false;
	 u8			current_time_sample = 0;
	 bool		Categories_usage = false;

	 void GetCurrentTimeSample(int curr_hour);

	 void WeatherAndRegions();

	 void MainParamsWindow_without_Categories();
	 void MainParamsWindow_with_Categories();
	
	 void makeTable(Regions region, int index);
	 void makeJustSingleParamWidget(Regions region, int index);

	 enum Action { GET, SET }; 
	 template<Action action>
	 void manageKeyframeValues(float* color, Regions region, int VarIndex, size_t time);

	 void setValuesForAllTimeSamples(float* color, Regions region, int VarIndex);
	 void handleParams(float* color, Regions region, int VarIndex, size_t time);
	 
	 virtual void window() override;
	 virtual void importData(std::filesystem::path path) override;
	 virtual void exportData(std::filesystem::path path) override;

public:
	
	TimecycleUI(const char* label);
	void MainParamsWindow();

};

template void TimecycleUI::manageKeyframeValues<TimecycleUI::Action::GET>(float* color, Regions region, int VarIndex, size_t time);
template void TimecycleUI::manageKeyframeValues<TimecycleUI::Action::SET>(float* color, Regions region, int VarIndex, size_t time);
