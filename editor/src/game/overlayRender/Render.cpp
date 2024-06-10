#include <chrono>
#include <thread>

#include "Render.h"
#include "app/logger.h"
#include "app/compiler/compiler.h"

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <Windows.h>

#include "CLensFlare/CLensFlare.h"

#pragma comment(lib, "D3DCompiler.lib")
using namespace DirectX;



#define SAFE_RELEASE(p) if (p) { p->Release(); p = nullptr; }

void GetScreenSize(u32& width, u32& height);
MSAAModeEnum GetMSAA();
void WaitWhileGameIsStarting();

namespace
{
	gmAddress	g_ResizeBuffersAddr;
	gmAddress	g_EndFrameAddr;
	gmAddress	g_WndProcAddr;

	bool shutdown_request = false;
}

HWND							Renderer::window = nullptr;
ComPtr<ID3D11Device>			Renderer::p_device = nullptr;
ComPtr<ID3D11DeviceContext>		Renderer::p_context = nullptr;
ComPtr<IDXGISwapChain>			Renderer::p_SwapChain = nullptr;

int		Renderer::open_window_btn = 0;
bool	Renderer::isWindowVisible = false;
bool	Renderer::mInitialized = false;
bool    Renderer::ImGuiCursorUsage = false;
bool	Renderer::mRenderState = false;
float	Renderer::font_size = 15.0f;
bool	Renderer::font_scale_expected_to_be_changed = false;



ID3D11Device*			Renderer::GetDevice()	{ return p_device.Get(); }
ID3D11DeviceContext*	Renderer::GetContext()	{ return p_context.Get(); }
HWND					Renderer::GetHandle()	{ return window; }


void Renderer::Search_for_gDevice()
{
	g_ResizeBuffersAddr = gmAddress::Scan("48 89 5C 24 ?? 48 89 74 24 ?? 57 48 81 EC 90 00 00 00 48 8B F1 48 8D 0D");
	g_EndFrameAddr = gmAddress::Scan("40 55 53 56 57 41 54 41 56 41 57 48 8B EC 48 83 EC 40 48 8B 0D");
	window = FindWindowW(L"grcWindow", NULL);
	SetWindowTextW(window, L"Hello World");
	
	
	g_WndProcAddr = gmAddress::Scan(
#if game_version == gameVer3095
		"48 8D 05 ?? ?? ?? ?? 33 C9 89 75 20"
#elif game_version == gameVer2060
		"85 C0 BF 00 00 CA 00").GetAt(80)
#endif 
	).GetRef(3);


	p_SwapChain = ComPtr<IDXGISwapChain>(*g_ResizeBuffersAddr.GetAt(33).GetRef(3).To<IDXGISwapChain**>());

	if (SUCCEEDED(p_SwapChain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(p_device.GetAddressOf())))) {
		p_device->GetImmediateContext(p_context.GetAddressOf());
	}
}



void (*g_ClipCursor)(LPRECT);
void Renderer::n_ClipCursor(LPRECT rect) 
{
	if (!isWindowVisible)
		g_ClipCursor(rect);
}

int (*g_ShowCursor)(bool);
int Renderer::n_ShowCursor(bool visible) 
{
	if (!isWindowVisible)
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

	if (GetAsyncKeyState(VK_HOME) && isWindowVisible)
		return 0;

	if (GetAsyncKeyState(open_window_btn) & 1) 
	{
		isWindowVisible = !isWindowVisible;
		
		if (!ImGuiCursorUsage) 
			ClipCursorToWindowRect(window, !isWindowVisible);
	}
	
	if (!ImGuiCursorUsage) 
		SetMouseVisible(isWindowVisible); 

	if (isWindowVisible)
	{
		//it handles mouse input even when ui isn't displayed, so i placed it under "isVisible" flag
		if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) 
			return true;
	}
	
	return g_WndProc(hWnd, uMsg, wParam, lParam);
}


bool test_flag = false;
//Vector3 pos1{};
//Vector3 pos2{};
//float col[4]{};



ComPtr<ID3D11InputLayout>		inputLayout;
ComPtr<ID3D11Buffer>			vertexBuffer;

ComPtr<ID3D11VertexShader>		vertexShader;
ComPtr<ID3D11PixelShader>		pixelShader;


UINT numVerts;
UINT stride;
UINT offset;



void(*g_EndFrame)();
void Renderer::n_EndFrame()
{
	mRenderState = true;

	if (!mInitialized)
	{
		InitBackend();
		BaseUiWindow::Create();
		ScriptHook::Start();
		CClock::Init();
		
		mlogger(" mInitialized");

		mInitialized = true;
	}
	if (mInitialized && isWindowVisible)
	{
		ImRenderFrame();
	}

	if (mInitialized)
	{
		LensFlareHandler::EndFrame();
	}


	//if (test_flag && mInitialized)
	//{
	//	scrInvoke([]
	//		{
	//			GRAPHICS::DRAW_LINE(
	//				pos1.x, pos1.y, pos1.z,
	//				pos2.x, pos2.y, pos2.z,
	//				(col[0] * 255), (col[1] * 255), (col[2] * 255), (col[3] * 255)); 
	//		});
	//}



	g_EndFrame();
	
	mRenderState = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////



void Renderer::Init()
{
	loadConfigParams();
	WaitWhileGameIsStarting();

	if (shutdown_request)
		return;

	Search_for_gDevice();

	Hook::Create(g_EndFrameAddr,	Renderer::n_EndFrame,	&g_EndFrame,	"EndFrame");
	Hook::Create(g_WndProcAddr,		Renderer::WndProc,		&g_WndProc,		"WndProc");

	if (!ImGuiCursorUsage)
	{
		Hook::Create(ClipCursor, Renderer::n_ClipCursor, &g_ClipCursor, "ClipCursor");
		Hook::Create(ShowCursor, Renderer::n_ShowCursor, &g_ShowCursor, "ShowCursor");
	}
}

void Renderer::loadConfigParams()
{
	auto* cfg = Preload::Get()->getConfigParser();

	font_scale_expected_to_be_changed = true;

	font_size			= cfg->GetInteger("Settings", "Font_size", 15);
	ImGuiCursorUsage	= cfg->GetBoolean("Settings", "CursorImgui_Impl", false);
	open_window_btn		= cfg->GetInteger("Settings", "OpenClose_window_button", 0x2D);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void Renderer::InitBackend()
{
	ImGui::CreateContext();
	ImPlot::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	mStyle();
	LoadFont();

	if (ImGuiCursorUsage)
		io.MouseDrawCursor = true;

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(p_device.Get(), p_context.Get());
}


void Renderer::ImRenderFrame()
{
	
	GameInput::DisableAllControlsThisFrame();

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	BaseUiWindow::GetInstance()->OnRender();

	//if (ImGui::Begin("tmp window"))
	//{
	//	if (ImGui::DragFloat("font sz", &font_size, 0.1f, 1, 100))
	//	{
	//		font_scale_expected_to_be_changed = true;
	//	}
	//	ImGui::End();
	//}


	//if (ImGui::Begin("tmp window1"))
	//{
	//	if (ImGui::Button("GetFirstPos"))
	//	{
	//		scrInvoke([] { pos1 = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), false); });
	//	}

	//	if (ImGui::Button("GetSecondPos"))
	//	{
	//		scrInvoke([] {pos2 = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), false); });
	//	}

	//	if (ImGui::Checkbox("Draw", &test_flag));

	//	ImGui::ColorEdit4("m_color_edit", col, 0 | ImGuiColorEditFlags_Uint8 | ImGuiColorEditFlags_AlphaBar);

	//	ImGui::End();
	//}
	


	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	
	if (font_scale_expected_to_be_changed) {
		ChangeFontSize();
		font_scale_expected_to_be_changed = false;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void Renderer::Shutdown()
{
	mlogger("shutdown call");
	shutdown_request = true;

	if (!mInitialized) 
		return;	

	isWindowVisible = false;
	while (mRenderState) {};

	BaseUiWindow::Destroy();

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	
	ImPlot::DestroyContext();
	ImGui::DestroyContext();

	Hook::Remove(g_EndFrameAddr);
	Hook::Remove(g_WndProcAddr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

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

void Renderer::mStyle()
{
	ImGuiStyle* style = &ImGui::GetStyle();
	
	style->GrabRounding = 4.0f;
	style->CellPadding = { 0, 3 };
	style->FramePadding = { 4, 4 };

	style->Colors[ImGuiCol_PopupBg] = { 45.0f/255.0f, 45.0f/255.0f, 45.0f/255.0f, 1.0f };
}



void WaitWhileGameIsStarting()
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




void GetScreenSize(u32& width, u32& height)
{
	static gmAddress addr = gmAddress::Scan("44 8B 1D ?? ?? ?? ?? 48 8B 3C D0");
	height = *addr.GetRef(3).To<u32*>();
	width = *addr.GetRef(0xB + 2).To<u32*>();
}


MSAAModeEnum GetMSAA()
{
	static u32* pMode = gmAddress::Scan("E8 ?? ?? ?? ?? 8B 47 4C 89 05", "rage::grcDevice::SetSamplesAndFragments")
		.GetRef(1)
		.GetRef(0x2)
		.To<u32*>();

	return static_cast<MSAAModeEnum>(*pMode);
}


////#if game_version == gameVer3095

////	//static gmAddress gAddr = gmAddress::Scan("48 8B 05 ?? ?? ?? ?? BE 08 00 00 00");
////	//p_context = *gAddr.GetRef(3).To<ID3D11DeviceContext**>();
////	//p_device = *gAddr.GetRef(-19 - 4).To<ID3D11Device**>();

////	p_SwapChain = *g_ResizeBuffersAddr.GetAt(33).GetRef(3).To<IDXGISwapChain**>();
////	//p_SwapChain = *gAddr.GetRef(-194 - 4).To<IDXGISwapChain**>();

////#elif game_version == gameVer2060

////	//static gmAddress gAddr = gmAddress::Scan("48 8D 05 ?? ?? ?? ?? 45 33 C9 48 89 44 24 58 48 8D 85 D0 08 00 00");
////	//p_context = *gAddr.GetRef(3).To<ID3D11DeviceContext**>();		
////	//p_device = *gAddr.GetRef(33).To<ID3D11Device**>();		
////	//p_SwapChain = *gAddr.GetRef(47).To<IDXGISwapChain**>();

////#endif
