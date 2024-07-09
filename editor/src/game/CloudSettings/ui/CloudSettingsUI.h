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
	using TimeArray_t = std::array<const char*, 15>;
	using TimeSamplesArray_t = std::vector<std::pair<int, const char*>>;

	CloudsHandler			m_CloudsHandler;
	CloudSettingsXmlParser	m_XmlParser;
	TimeArray_t				m_TimeArray;
	TimeSamplesArray_t		m_TimeSamples;
	u8						m_CurrentTimeSampleIndex = 0;
	bool					m_ShowOnlyTheCurrentSample = false;
	size_t					m_MaxCloudHatNameLenIdx = -1;

	void GetCurrentTimeSample(int current_hour);

	void CloudsDataWidgets(int clIdx);
	void ProbabilityWidgets(int clIdx);
	
	void CloudDataTreeNode(const char* label, int clIdx, atArray<ptxKeyframeEntry>& arr, std::function<void(atArray<ptxKeyframeEntry>&, int, const char*)> func);

	void CloudSettingsColourTable(atArray<ptxKeyframeEntry>& arr, int table_id, const char* param_name);
	void CloudSettingsColourSingleParam(atArray<ptxKeyframeEntry>& arr, int table_id, const char* param_name);

	void CloudSettingsVariablesTable(atArray<ptxKeyframeEntry>& arr, int table_id, const char* param_name);
	void CloudSettingsVariablesSingleParam(atArray<ptxKeyframeEntry>& arr, int table_id, const char* param_name);

	void CloudHatList();

	void ParamsWindow();

public:

	CloudSettingsUI(const char* title);

	virtual void	window() override;
	virtual void	importData(std::filesystem::path path) override;
	virtual void	exportData(std::filesystem::path path) override;
};

