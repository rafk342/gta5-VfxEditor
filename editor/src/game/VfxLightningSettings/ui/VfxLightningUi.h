#pragma once
#include "uiBase/uiBaseWindow.h"
#include "VfxLightningSettings/VfxLightningSettings.h"
#include "ImGui/imgui.h"
#include "uiBase/ImguiHelpers.h"
#include "scripthookTh.h"

class VfxLightningXmlParser 
{
public:
	void exportLightningData(std::string path) {};
	void importLightningData(std::string path) {};
};


class VfxLightningUi : public App
{
	VfxLightningOwner mVfxLightingOwner;
	VfxLightningXmlParser mXmlParser;

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

