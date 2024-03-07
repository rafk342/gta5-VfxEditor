
#include <Windows.h>
#include <iostream>
#include <format>
#include <fstream>

#include "app/Preload/Preload_Integration.h"
#include "game/overlayRender/Render.h"
#include "app/logger.h"
#include "app/compiler/compiler.h"
#include "game/CloudSettings/CloudSettings.h"
#include "scripthookTh.h"

#define test_ver 0


FILE* f;

void Console() {
	AllocConsole();
	freopen_s(&f, "CONOUT$", "w", stdout);
}


AM_EXPORT void Init()
{
#if test_ver

	Console();
	mlogger("Init()");
	
	CloudsHandler clHandler;

	mlogger("done");
#else

	Preload_Integration::Preload();
	FileListUI::setPreBuff(config_params::path_from_cfg);
	mRender::SetOpenWindowBtn(config_params::open_window_btn_key);
	mRender::SetCursorImguiUsage(config_params::cursor_imgui_usage);
	Hook::Init();
	ScriptHook::Init();
	std::thread th(mRender::Init);
	th.detach();

#endif
}

AM_EXPORT void Shutdown()
{
#if test_ver
	fclose(f);
	FreeConsole();
#else	

	mRender::Shutdown();
	ScriptHook::Shutdown();
	Hook::Shutdown();
	std::this_thread::sleep_for(std::chrono::milliseconds(300));

#endif
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD  dwReason, LPVOID lpReserved)
{

#if !am_version
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hModule);
		Init();
		
		break;
	case DLL_PROCESS_DETACH:
		Shutdown();
		
		break;
	}
#endif // !am_version

	return TRUE;
}







