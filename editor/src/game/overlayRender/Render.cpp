#include <chrono>
#include <thread>

#include "game/CLensFlare/CLensFlare.h"
#include "Render.h"
#include "app/logger.h"
#include "app/compiler/compiler.h"

#include <d3d11.h>
#include <Windows.h>

static void WaitWhileGameIsStarting();

namespace
{
	gmAddress	s_ResizeBuffersAddr;
	gmAddress	s_EndFrameAddr;
	gmAddress	s_WndProcAddr;
	gmAddress	s_SafeModeOperationsAddr;
	
	bool shutdown_request = false;
}

HWND							Renderer::window = nullptr;
ComPtr<ID3D11Device>			Renderer::p_device = nullptr;
ComPtr<ID3D11DeviceContext>		Renderer::p_context = nullptr;
ComPtr<IDXGISwapChain>			Renderer::p_SwapChain = nullptr;

int		Renderer::sm_OpenWindowButton = 0;
bool	Renderer::sm_IsWindowVisible = false;
bool	Renderer::sm_Initialized = false;
bool    Renderer::sm_ImGuiCursorUsage = false;
bool	Renderer::sm_RenderState = false;
float	Renderer::font_size = 15.0f;
bool	Renderer::font_scale_expected_to_be_changed = false;

ID3D11Device*			Renderer::GetDevice()	{ return p_device.Get(); }
ID3D11DeviceContext*	Renderer::GetContext()	{ return p_context.Get(); }
HWND					Renderer::GetHandle()	{ return window; }

void Renderer::Search_for_gDevice()
{
	s_ResizeBuffersAddr = gmAddress::Scan("48 89 5C 24 ?? 48 89 74 24 ?? 57 48 81 EC 90 00 00 00 48 8B F1 48 8D 0D");
	s_EndFrameAddr = gmAddress::Scan("75 0C 44 38 35", "rage::grcDevice::EndFrame+0xC8").GetAt(-0xC8);
	
	window = FindWindowW(L"grcWindow", NULL);
	SetWindowTextW(window, L"Hello World");
	
	s_WndProcAddr = gmAddress::Scan
#if game_version == gameVer3095
		("48 8D 05 ?? ?? ?? ?? 33 C9 89 75 20")
#elif game_version == gameVer2060
		("85 C0 BF 00 00 CA 00").GetAt(80)
#endif
	.GetRef(3);

	p_SwapChain = ComPtr<IDXGISwapChain>(*s_ResizeBuffersAddr.GetAt(33).GetRef(3).To<IDXGISwapChain**>());
	if (SUCCEEDED(p_SwapChain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(p_device.GetAddressOf()))))
	{
		p_device->GetImmediateContext(p_context.GetAddressOf());
	}
}



void (*g_ClipCursor)(LPRECT);
void Renderer::n_ClipCursor(LPRECT rect) 
{
	if (!sm_IsWindowVisible)
		g_ClipCursor(rect);
}

int (*g_ShowCursor)(bool);
int Renderer::n_ShowCursor(bool visible) 
{
	if (!sm_IsWindowVisible)
		g_ShowCursor(visible);
	
	return visible ? 0 : -1; 
}

void Renderer::SetMouseVisible(bool visible)
{
	if (visible)
		while (g_ShowCursor(true) < 0);
	else
		while (g_ShowCursor(false) >= 0);
}

void ClipCursorToWindowRect(HWND handle, bool clip)
{
	RECT rect;
	GetWindowRect(handle, &rect);
	g_ClipCursor(clip ? &rect : NULL);
}




LRESULT(*g_WndProc)(HWND, UINT, WPARAM, LPARAM);
LRESULT Renderer::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	if (GetAsyncKeyState(VK_HOME) && sm_IsWindowVisible)
		return 0;

	if (GetAsyncKeyState(sm_OpenWindowButton) & 1) 
	{
		sm_IsWindowVisible = !sm_IsWindowVisible;
		
		if (!sm_ImGuiCursorUsage) 
			ClipCursorToWindowRect(window, !sm_IsWindowVisible);
	}
	
	if (!sm_ImGuiCursorUsage) 
		SetMouseVisible(sm_IsWindowVisible); 

	if (sm_IsWindowVisible)
	{
		if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) 
			return true;
	}

	return g_WndProc(hWnd, uMsg, wParam, lParam);
}



// Called from CApp::RunGame() -> fwRenderThreadInterface::Synchronise()
// Render thread is guaranteed to be blocked here
//void (*g_PerformSafeModeOperations)(void*);
//void Renderer::hk_PerformSafeModeOperations(void* instance)
//{
//	g_PerformSafeModeOperations(instance);
//}
// 
void(*g_GpuEndFrame)();
void Renderer::hk_GpuEndFrame()
{
	sm_RenderState = true;
	if (!sm_Initialized)
	{
		InitBackend();
		BaseUiWindow::Create();
		ScriptHook::Start();
		CClock::Init();

		sm_Initialized = true;
	}
	if (sm_Initialized && !shutdown_request)
	{
		ImStartFrame();
		if (sm_IsWindowVisible) 
		{
			ImDrawUI();
		}
		LensFlareHandler::EndFrame();
		ImEndFrame();
	}
	g_GpuEndFrame();
	sm_RenderState = false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void Renderer::Init()
{
	loadConfigParams();
	WaitWhileGameIsStarting();

	if (shutdown_request)
		return;

	Search_for_gDevice();
	Hook::Create(s_EndFrameAddr,	Renderer::hk_GpuEndFrame,	&g_GpuEndFrame,	"EndFrame");
	Hook::Create(s_WndProcAddr,		Renderer::WndProc,			&g_WndProc,		"WndProc");

	if (!sm_ImGuiCursorUsage)
	{
		Hook::Create(ClipCursor, Renderer::n_ClipCursor, &g_ClipCursor, "ClipCursor");
		Hook::Create(ShowCursor, Renderer::n_ShowCursor, &g_ShowCursor, "ShowCursor");
	}
	//s_SafeModeOperationsAddr = gmAddress::Scan("B9 2D 92 F5 3C", "CGtaRenderThreadGameInterface::PerformSafeModeOperations")
	//	.GetAt(-0x31);
	//Hook::Create(s_SafeModeOperationsAddr, hk_PerformSafeModeOperations, &g_PerformSafeModeOperations, "CGtaRenderThreadGameInterface::PerformSafeModeOperations");
}

void Renderer::loadConfigParams()
{
	auto* cfg = Preload::Get()->getConfigParser();

	font_scale_expected_to_be_changed = true;

	font_size				= cfg->GetInteger("Settings", "Font_size", 15);
	sm_ImGuiCursorUsage		= cfg->GetBoolean("Settings", "CursorImgui_Impl", false);
	sm_OpenWindowButton		= cfg->GetInteger("Settings", "OpenClose_window_button", 0x2D);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Renderer::InitBackend()
{
	ImGui::CreateContext();
	ImPlot::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	ImStyle();
	LoadFont();

	if (sm_ImGuiCursorUsage)
		io.MouseDrawCursor = true;

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(p_device.Get(), p_context.Get());
}

void Renderer::ImStartFrame()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void Renderer::ImEndFrame()
{
	ImGui::EndFrame();
	ImGui::Render();

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	
	if (font_scale_expected_to_be_changed) {
		ChangeFontSize();
		font_scale_expected_to_be_changed = false;
	}
}


void Renderer::ImDrawUI()
{
	GameInput::DisableAllControlsThisFrame();
	BaseUiWindow::GetInstance()->OnRender();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Renderer::Shutdown()
{
	shutdown_request = true;
	
	if (!sm_Initialized) 
		return;	

	sm_IsWindowVisible = false;
	while (sm_RenderState) {};
	
	BaseUiWindow::Destroy();

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();

	ImPlot::DestroyContext();
	ImGui::DestroyContext();

	Hook::Remove(s_EndFrameAddr);
	Hook::Remove(s_WndProcAddr);
	Hook::Remove(s_SafeModeOperationsAddr);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace 
{
	const char* font_path = "c:\\Windows\\Fonts\\calibri.ttf";
	ImFontConfig fontConfig{};
	
	// latin& cyrillic& chinese
	static const ImWchar fontRange[] =
	{
		//Latin
		0x0020, 0x00FF, // Basic Latin + Latin Supplement
		0x0400, 0x052F, // Cyrillic + Cyrillic Supplement
		0x2DE0, 0x2DFF, // Cyrillic Extended-A
		0xA640, 0xA69F, // Cyrillic Extended-B
		//Chinese
		0x2000, 0x206F, // General Punctuation
		0x3000, 0x30FF, // CJK Symbols and Punctuations, Hiragana, Katakana
		0x31F0, 0x31FF, // Katakana Phonetic Extensions
		0xFF00, 0xFFEF, // Half-width characters
		0xFFFD, 0xFFFD, // Invalid
		0x4e00, 0x9FAF, // CJK Ideograms
		0,
	};
}

void Renderer::ChangeFontSize()
{
	auto& io = ImGui::GetIO();

	fontConfig.RasterizerDensity = log(font_size) / 5 + 1;

	io.Fonts->Clear();
	io.Fonts->AddFontFromFileTTF(font_path, font_size, &fontConfig, fontRange);
	io.Fonts->Build();

	ImGui_ImplDX11_InvalidateDeviceObjects();
	ImGui_ImplDX11_CreateDeviceObjects();
}

void Renderer::LoadFont()
{
	ImGuiIO& io = ImGui::GetIO();

	fontConfig.RasterizerMultiply = 1.3f;
	fontConfig.RasterizerDensity = 1.2f;
	fontConfig.OversampleH = 2;
	fontConfig.OversampleV = 1;

	io.Fonts->AddFontFromFileTTF(font_path, font_size, &fontConfig, fontRange);
}

void Renderer::ImStyle()
{
	ImGuiStyle* style = &ImGui::GetStyle();
	
	style->GrabRounding = 4.0f;
	style->CellPadding = { 0, 3 };
	style->FramePadding = { 4, 4 };

	style->Colors[ImGuiCol_PopupBg] = { 45.0f/255.0f, 45.0f/255.0f, 45.0f/255.0f, 1.0f };
}


static void WaitWhileGameIsStarting()
{
#if !am_version
	static bool* is_game_rendering = gmAddress::Scan("88 1D ?? ?? ?? ?? 89 05 ?? ?? ?? ?? EB ?? 8A 1D")
		.GetRef(2)
		.To<bool*>();

	while (*is_game_rendering == 0)
	{
		if (shutdown_request)	// gets true in render::shutdown
			break;

		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
#endif
}

////#if game_version == gameVer3095

////	//static gmAddress gAddr = gmAddress::Scan("48 8B 05 ?? ?? ?? ?? BE 08 00 00 00");
////	//p_context = *gAddr.GetRef(3).To<ID3D11DeviceContext**>();
////	//p_device = *gAddr.GetRef(-19 - 4).To<ID3D11Device**>();

////	p_SwapChain = *s_ResizeBuffersAddr.GetAt(33).GetRef(3).To<IDXGISwapChain**>();
////	//p_SwapChain = *gAddr.GetRef(-194 - 4).To<IDXGISwapChain**>();

////#elif game_version == gameVer2060

////	//static gmAddress gAddr = gmAddress::Scan("48 8D 05 ?? ?? ?? ?? 45 33 C9 48 89 44 24 58 48 8D 85 D0 08 00 00");
////	//p_context = *gAddr.GetRef(3).To<ID3D11DeviceContext**>();		
////	//p_device = *gAddr.GetRef(33).To<ID3D11Device**>();		
////	//p_SwapChain = *gAddr.GetRef(47).To<IDXGISwapChain**>();

////#endif




	//if (ImGui::Begin("tmp window"))
	//{
	//	if (ImGui::DragFloat("font sz", &font_size, 0.1f, 1, 100))
	//	{
	//		font_scale_expected_to_be_changed = true;
	//	}
	//	ImGui::End();
	//}
