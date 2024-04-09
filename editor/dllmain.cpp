
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
#include "helpers/SimpleTimer.h"


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
	mlogger("mRender::Shutdown()");
	mRender::Shutdown();
#if am_version
	mlogger("ScriptHook::Shutdown()");
	ScriptHook::Shutdown();
#endif
	mlogger("Hook::Shutdown()");
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
		mlogger("case detach");
		mlogger("Shutdown...");
		Shutdown();
		mlogger("Shutdown done");
		mlogger("FreeLibraryAndExitThread...");
		FreeLibraryAndExitThread(hModule,0);
		mlogger("FreeLibraryAndExitThread done");
		break;
	}
#endif // !am_version
	return TRUE;
}







