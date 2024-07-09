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
#include "scripthookTh.h"

#include "game/timecycle/tñHandler.h"
#include "game/timecycle/tcData.h"
#include "game/timecycle/xml/tcXmlParser.h"
#include "game/gameClock/CClock.h"
#include "helpers/helpers.h"
#include "uiBase/ImguiHelpers.h"

#include "app/uiBase/uiBaseWindow.h"

#include "Preload/Preload.h"

class TimecycleUI : public App
{
	 TimeñycleHandler	m_tcHandler;
	 [[msvc::no_unique_address]] tcXmlParser m_XmlParser;
	 
	 std::vector<std::pair<int, const char*>> time_samples;
	 std::array<const char*, 2>	regions;

	 tcCycle*	m_CurrentCycle;
	 int		m_CurrentWeatherIndex = 0;
	 Regions	m_CurrentRegion = GLOBAL;
	 bool		m_EditBothRegions = false;
	 bool		m_EditAllTimeSamples = false;
	 bool		m_ShowOnlyTheCurrentSample = false;
	 u8			m_CurrentTimeSample = 0;
	 bool		m_CategoriesUsage = false;

	 void GetCurrentTimeSample(int curr_hour);

	 void WeatherAndRegions();

	 void MainParamsWindow_without_Categories();
	 void MainParamsWindow_with_Categories();
	
	 void makeTable(Regions region, int index);
	 void makeJustSingleParamWidget(Regions region, int index);

	 enum Action { GET, SET }; 
	 template<Action action>
	 void ManageKeyframeValues(float* color, Regions region, int VarIndex, size_t time);

	 void setValuesForAllTimeSamples(float* color, Regions region, int VarIndex);
	 void SetParams(float* color, Regions region, int VarIndex, size_t time);
	 
	 virtual void window() override;
	 virtual void importData(std::filesystem::path path) override;
	 virtual void exportData(std::filesystem::path path) override;

public:
	
	TimecycleUI(const char* title);
	void MainParamsWindow();

};

template void TimecycleUI::ManageKeyframeValues<TimecycleUI::Action::GET>(float* color, Regions region, int VarIndex, size_t time);
template void TimecycleUI::ManageKeyframeValues<TimecycleUI::Action::SET>(float* color, Regions region, int VarIndex, size_t time);
