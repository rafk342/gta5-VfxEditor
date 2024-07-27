
#include <Windows.h>
#include <iostream>
#include <format>
#include <fstream>
#include <mutex>

#include "stringzilla/include.h"

#include "Preload/Preload.h"
#include "game/overlayRender/Render.h"
#include "app/logger.h"
#include "app/compiler/compiler.h"
#include "scripthookTh.h"


static FILE* f;

void ShowConsole() {
	AllocConsole();
	freopen_s(&f, "CONOUT$", "w", stdout);
}

AM_EXPORT void Init()
{
#if with_console
	ShowConsole();
#endif
#if !test_ver
	Preload::Create().Init();
	Hook::Init();
	ScriptHook::Init();
	std::thread th(Renderer::Init);
	th.detach();
#else

#endif
}

AM_EXPORT void Shutdown()
{
#if with_console
	fclose(f);
	FreeConsole();
#endif
#if !test_ver
	Renderer::Shutdown();
	Preload::Destroy();
#if am_version
	ScriptHook::Shutdown();
#endif
	Hook::Shutdown();
	std::this_thread::sleep_for(std::chrono::milliseconds(300));
#else

#endif
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
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

