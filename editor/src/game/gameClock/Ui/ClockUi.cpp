#include "ClockUi.h"


int ClockUi::curr_hour = 0;
int ClockUi::curr_min = 0;
bool ClockUi::showTimeWindow = false;

void ClockUi::timeWindow()
{
	static bool time_flag = false;
	static bool is_slider_active = false;
	static bool pause_clock = CClock::IsTimePaused();
	static bool tmp = true;

	if (ImGui::Checkbox("Time settings", &showTimeWindow)) {
		if (showTimeWindow) {
			curr_hour = CClock::GetCurrentHour();
			curr_min = CClock::GetCurrentMin();
		}
	}

	if (!showTimeWindow) {
		return;
	}

	ImGui::SetNextWindowSize({ 239,124 }, 0);
	ImGui::Begin("Time settings", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings);

	if (ImGui::Checkbox("PauseClock", &pause_clock)) 
	{
		CClock::PauseClock(pause_clock);
	}

	if (!pause_clock)
	{
		curr_hour = CClock::GetCurrentHour();
		curr_min = CClock::GetCurrentMin();
	}

	if (ImGui::DragInt("##Hours", &curr_hour, 0.05, -1, 24, "Hour : %i", ImGuiSliderFlags_NoInput))
	{
		if (curr_hour >= 24) {
			curr_hour = 0;
		}
		else if (curr_hour <= 0) {
			curr_hour = 23;
		}
		CClock::setCurrentHour(curr_hour);
	}
	
	{
		if (ImGui::IsItemActivated())
		{
			tmp = true;
			CClock::PauseClock(tmp);
		}
		if (ImGui::IsItemDeactivated())
		{
			if (!pause_clock)
			{
				tmp = false;
				CClock::PauseClock(tmp);
			}
		}
	}

	if (ImGui::DragInt("##Minutes", &curr_min, 0.5, -1, 60, "Minute : %i", ImGuiSliderFlags_NoInput))
	{
		if (curr_min >= 60)
		{
			curr_hour = curr_hour + 1;
			if (curr_hour >= 24) {
				curr_hour = 0;
			}
			curr_min = 0;
		}
		else if (curr_min <= 0)
		{
			curr_hour = curr_hour - 1;
			if (curr_hour <= 0) {
				curr_hour = 23;
			}
			curr_min = 59;
		}
		CClock::setCurrentHour(curr_hour);
		CClock::setCurrentMin(curr_min);
	}

	{
		if (ImGui::IsItemActivated())
		{
			tmp = true;
			CClock::PauseClock(tmp);
		}
		if (ImGui::IsItemDeactivated())
		{
			if (!pause_clock)
			{
				tmp = false;
				CClock::PauseClock(tmp);
			}
		}
	}

	ImGui::End();
}
