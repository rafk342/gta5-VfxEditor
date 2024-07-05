
#include <Windows.h>
#include <iostream>
#include <format>
#include <fstream>
#include <mutex>

#include "Preload/Preload.h"
#include "game/overlayRender/Render.h"
#include "app/logger.h"
#include "app/compiler/compiler.h"
#include "scripthookTh.h"


FILE* f;

void Console() {
	AllocConsole();
	freopen_s(&f, "CONOUT$", "w", stdout);
}

//48 8B 0D ?? ?? ?? ?? 8B D3 E8 ?? ?? ?? ?? 48 8B 0D ?? ?? ?? ?? 8B D6
// ptxMngr -> ref(3) 7FF71C5DC800

AM_EXPORT void Init()
{
#if with_console
	Console();
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
	LogInfo("Renderer::Shutdown()");

	Renderer::Shutdown();
	LogInfo("Preload::Destroy()");
	Preload::Destroy();
	LogInfo("ScriptHook::Shutdown()");

#if am_version
	ScriptHook::Shutdown();
#endif
	LogInfo("Hook::Shutdown()");
	Hook::Shutdown();
	std::this_thread::sleep_for(std::chrono::milliseconds(300));
	LogInfo("Shutdown Done()");
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







