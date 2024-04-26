#pragma once
#include "VisualSettings/visualSettingsHandler.h"
#include "ImGui/imgui.h"
#include "uiBase/uiBaseWindow.h"

class VisualSettingsUi : App
{
	VisualSettingsHandler mHandler;
	bool hide_unused = false;
	bool check_if_category_should_be_shown(const char* category);

public:
	using App::App;
	virtual void window() override;
	virtual void importData(std::filesystem::path path) override;
	virtual void exportData(std::filesystem::path path) override;
};

