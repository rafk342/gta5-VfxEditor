#pragma once
#include <filesystem>

#include "app/uiBase/uiBaseWindow.h"
#include "memory/address.h"
#include "memory/hook.h"
#include "logger.h"
#include "imgui.h"
#include "Preload/Preload.h"

class gBloodfxReloader 
{
	static std::filesystem::path new_path;
	static void* n_asset_open_f(u64 arg1, char* fname, char* ext, bool arg4, bool arg5);
public:
	static bool invalid_path;
	static bool bloodfx_loaded;
	bool IsLoaded();
	void reload_bloodfx(char* path);
};

class gBloodfxUi : public App, private gBloodfxReloader
{
	char buff[256];
public:
	gBloodfxUi(const char* title);
	virtual void window() override;
};

