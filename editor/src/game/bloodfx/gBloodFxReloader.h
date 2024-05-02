#pragma once
#include <filesystem>

#include "app/uiBase/uiBaseWindow.h"
#include "memory/address.h"
#include "memory/hook.h"
#include "logger.h"
#include "imgui.h"


class gBloodfxReloader 
{
	static std::filesystem::path new_path;
	static void* n_asset_open_f(u64 arg1, char* fname, char* ext, bool arg4, bool arg5);
public:
	static bool invalid_path;
	static bool bloodfx_loaded;
	void reload_bloodfx(char* path);
};

class gBloodfxUi : public App, private gBloodfxReloader
{
public:
	using App::App;

	virtual void window() override;
	virtual void importData(std::filesystem::path path) override;
	virtual void exportData(std::filesystem::path path) override;
};

