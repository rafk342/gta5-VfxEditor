#pragma once
#include <iostream>

#include "../CLensFlare.h"

#include "ImGui/imgui.h"
#include "uiBase/uiBaseWindow.h"
#include "uiBase/ImguiHelpers.h"


class LensFlareUi : public App
{
	LensFlareHandler m_Handler;

public:

	LensFlareUi(const char* title);


	virtual void window() override;
	virtual void importData(std::filesystem::path path) override;
	virtual void exportData(std::filesystem::path path) override;

};

