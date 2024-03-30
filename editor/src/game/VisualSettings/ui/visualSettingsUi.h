#pragma once
#include "VisualSettings/visualSettingsHandler.h"
#include "ImGui/imgui.h"
#include "uiBase/uiBaseWindow.h"

class VisualSettingsUi : App
{
	VisualSettingsHandler mHandler;
public:
	using App::App;
	virtual void window() override;
	virtual void importData(std::string path) override;
	virtual void exportData(std::string path) override;
};

