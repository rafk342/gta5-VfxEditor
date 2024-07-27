#pragma once
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <mutex>
#include <wrl.h>

#include "app/memory/hook.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImPlot/implot.h"

#include "scripthook/inc/natives.h"

#include "game/rageControl/control.h"
#include "uiBase/uiBaseWindow.h"
#include "gameClock/CClock.h"
#include "scripthookTh.h"
#include "Preload/Preload.h";

#include "CamCinematicContext/CamContext.h"
#include "rage/atl/atHashMap.h"

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")


using Microsoft::WRL::ComPtr;
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class Renderer
{
	static inline int  sm_OpenWindowButton = 0x2D;
	static inline bool sm_IsWindowVisible = false;
	static inline bool sm_Initialized = false;
	static inline bool sm_ImGuiCursorUsage = false;
	static inline bool sm_RenderState = false;
	static inline int  font_size = 15;
	static inline bool font_scale_expected_to_be_changed = false;
	static inline bool sm_ShowFontSzSelWnd = false;

	static inline HWND	window = nullptr;
	static inline ComPtr<ID3D11Device> p_device = nullptr;
	static inline ComPtr<ID3D11DeviceContext> p_context = nullptr;
	static inline ComPtr<IDXGISwapChain> p_SwapChain = nullptr;

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static void Search_for_gDevice();
	static void hk_GpuEndFrame();
	static void InitBackend();
	
	static void ImStartFrame();
	static void ImEndFrame();
	static void ImDrawUI();

	static void n_ClipCursor(LPRECT rect);
	static int  n_ShowCursor(bool visible);
	static void SetMouseVisible(bool visible);

	static void ChangeFontSize();
	static void LoadFont();
	static void ImStyle();

	static void loadConfigParams();
	
public:

	static ID3D11Device*			GetDevice();
	static ID3D11DeviceContext*		GetContext();
	static HWND						GetHandle();

	static void Init();
	static void Shutdown();
};


#define DX11Context (Renderer::GetContext())
#define DX11Device (Renderer::GetDevice())

