#pragma once
#include "ImGui/imgui.h"
#include "../CClock.h"


class ClockUi
{
	static int curr_hour;
	static int curr_min;
	static bool showTimeWindow;
public:
	static void timeWindowCheckBox();
};

