﻿#pragma once
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

#include "rage/atl/atHashMap.h"

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")



#define SAFE_RELEASE(obj) if(obj) { (obj)->Release(); (obj) = nullptr; }
#define AM_ASSERT_STATUS(expr) if ((expr) != S_OK) LogInfo("Line : {} | File : {} | Something went wrong", __LINE__, __FILE__);

using Microsoft::WRL::ComPtr;
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class Renderer
{

	static int		sm_OpenWindowButton;
	static bool		sm_IsWindowVisible;
	static bool		sm_Initialized;
	static bool		sm_ImGuiCursorUsage;
	static bool		sm_RenderState;
	static float	font_size;
	static bool		font_scale_expected_to_be_changed;

	static HWND	window;
	static ComPtr<ID3D11Device> p_device;
	static ComPtr<ID3D11DeviceContext> p_context;
	static ComPtr<IDXGISwapChain> p_SwapChain;

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

