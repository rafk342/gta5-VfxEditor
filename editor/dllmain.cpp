
#include <Windows.h>
#include <iostream>
#include <format>
#include <fstream>
#include <mutex>

#include "Preload/Preload.h"
#include "game/overlayRender/Render.h"
#include "app/logger.h"
#include "app/compiler/compiler.h"
#include "game/CloudSettings/CloudSettings.h"
#include "scripthookTh.h"
#include "helpers/SimpleTimer.h"

#include "CLensFlare/CLensFlare.h"
//
//void* operator new(decltype(sizeof(0)) n) noexcept(false)
//{
//	void* ptr = malloc(n);
//	std::cout << vfmt("alloc sz : {} bytes | addr : {:016X}\n", n, (u64)ptr);
//	return ptr;
//}
//
//void operator delete(void* p) noexcept
//{
//	std::cout << vfmt("erased at : {0:016X}\n", (u64)p);
//	free(p);
//}
//

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







