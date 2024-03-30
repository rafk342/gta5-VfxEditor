#pragma once
#include <format>
#include <ImGui/imgui.h>


#define FORMAT_TO_BUFF(buf, format_str, ...) \
	std::memset(buf, 0, sizeof(buf)); \
    std::format_to(buf, format_str, ##__VA_ARGS__);	\


void PushStyleCompact();
void PushStyleCompact(float coef);
void PopStyleCompact();
void HelpMarker(const char* desc);

