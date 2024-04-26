#pragma once
#include "uiBase/uiBaseWindow.h"

#include "VfxLightningSettings/xml/vfxLightningXmlParser.h"
#include "VfxLightningSettings/VfxLightningSettings.h"

#include "ImGui/imgui.h"
#include "ImPlot/implot.h"
#include "uiBase/ImguiHelpers.h"
#include "memory/hook.h"


class VfxLightningUi : public App
{
	VfxLightningHandler mVfxLightingHandler;
	VfxLightningsXmlParser mXmlParser;

	void LightningTimeCycleModsWidgets();
	void DirectionalBurstSettingsWidgets();
	void CloudBurstSettingsWidgets();
	void StrikeSettingsWidgets();

	void StrikeVariationsWidgets(u8 idx);
	void CloudBurstCommonSettingsWidgets(CloudBurstCommonSettings& rCloudBurstCommonSettings, const char* treeLabel);
	void keyframePlot(ptxKeyframe& keyframe, const char* label);
public:
	using App::App;

	virtual void window() override;
	virtual void importData(std::filesystem::path path) override;
	virtual void exportData(std::filesystem::path path) override;
};

extern void keyframeTable(const char* label, ptxKeyframe& keyframe, u8 idx);
