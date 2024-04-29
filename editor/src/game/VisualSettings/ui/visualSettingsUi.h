#pragma once
#include "VisualSettings/visualSettingsHandler.h"
#include "ImGui/imgui.h"
#include "uiBase/uiBaseWindow.h"

class VisualSettingsUi : public App
{
	VisualSettingsHandler mHandler;
	bool hide_unused = false;
	bool check_if_category_should_be_shown(const char* category);

public:
	VisualSettingsUi(const char* label) : App(label) {}

	virtual void window() override;
	virtual void importData(std::filesystem::path path) override;
	virtual void exportData(std::filesystem::path path) override;
};

