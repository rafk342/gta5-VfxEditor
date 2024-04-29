
#include <Windows.h>
#include <iostream>
#include <format>
#include <fstream>

#include "Preload/Preload.h"
//#include "app/Preload/Preload_Integration.h"
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

	Preload::Create();
	Preload::Get()->preload();
	INIReader* cfg = Preload::Get()->getConfigParser();

	FileListUI::setPreBuff(cfg->GetString("Settings","Default_path","E:\\GTAV\\timecycles"));
	mRender::SetOpenWindowBtn(cfg->GetInteger("Settings", "OpenClose_window_button", 0x2D));
	mRender::SetCursorImguiUsage(cfg->GetBoolean("Settings", "CursorImgui_Impl", false));
	mRender::SetFontSize(cfg->GetInteger("Settings", "Font_size", 15));

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
	Preload::Destroy();
#if am_version
	ScriptHook::Shutdown();
#endif
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
		FreeLibraryAndExitThread(hModule,0);
		break;
	}
#endif // !am_version
	return TRUE;
}







