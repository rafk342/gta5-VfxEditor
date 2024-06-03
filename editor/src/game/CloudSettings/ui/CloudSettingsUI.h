#pragma once

#include <format>
#include <unordered_map>

#include "ImGui/imgui.h"
#include "CloudSettings/CloudSettings.h"
#include "uiBase/ImguiHelpers.h"
#include "uiBase/uiBaseWindow.h"
#include "CloudSettings/xml/CloudSettingsXmlParser.h"
#include "gameClock/CClock.h"


class CloudSettingsUI : public App
{
	CloudsHandler								mCloudsHandler;
	
	std::vector<CloudSettingsNamed>&			CloudsVec;
	std::vector<const char*>					CloudHatNames;
	std::array<const char*, 15>					time_array;
	std::vector<std::pair<int, const char*>>	time_samples;
	int											current_hat = 0;
	u8											current_time_sample_idx = 0;
	bool										ShowOnlyTheCurrentSample = false;


	void GetCurrentTimeSample(int current_hour);

	void CloudsDataWidgets(int clIdx);
	void ProbabilityWidgets(int clIdx);
	
	void CloudDataTreeNode(const char* label, int clIdx, atArray<ptxKeyframeEntry>& arr, std::function<void(atArray<ptxKeyframeEntry>&, int, const char*)> func);

	void CloudSettingsColourTable(atArray<ptxKeyframeEntry>& arr, int table_id, const char* param_name);
	void CloudSettingsColourSingleParam(atArray<ptxKeyframeEntry>& arr, int table_id, const char* param_name);

	void CloudSettingsVariablesTable(atArray<ptxKeyframeEntry>& arr, int table_id, const char* param_name);
	void CloudSettingsVariablesSingleParam(atArray<ptxKeyframeEntry>& arr, int table_id, const char* param_name);

	virtual void	window() override;
	virtual void	importData(std::filesystem::path path) override;
	virtual void	exportData(std::filesystem::path path) override;

public:
	CloudSettingsXmlParser	mXmlParser;
	
	CloudSettingsUI(const char* title);
	void ParamsWindow();
};

