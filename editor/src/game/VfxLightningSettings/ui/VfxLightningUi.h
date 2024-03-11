#pragma once
#include "uiBase/uiBaseWindow.h"

#include "VfxLightningSettings/xml/vfxLightningXmlParser.h"
#include "VfxLightningSettings/VfxLightningSettings.h"

#include "ImGui/imgui.h"
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
public:

	VfxLightningUi(BaseUiWindow* base, const char* label);

	virtual void window() override;
	virtual void importData(std::string path) override;
	virtual void exportData(std::string path) override;
};

