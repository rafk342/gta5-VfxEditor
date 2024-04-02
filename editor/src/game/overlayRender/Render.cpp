#include <chrono>
#include <thread>

#include "Render.h"
#include "app/logger.h"
#include "app/compiler/compiler.h"


void WaitWhileGameIsStarting();

namespace 
{
	gmAddress	game_ResizeBuffersAddr;
	gmAddress	game_PresentImageAddr;
	gmAddress	game_WndProcAddr;

	bool shutdown_request = false;
}

HWND					mRender::window = nullptr;
ID3D11Device*			mRender::p_device = nullptr;
ID3D11DeviceContext*	mRender::p_context = nullptr;
IDXGISwapChain*			mRender::p_SwapChain = nullptr;

int		mRender::open_window_btn = 0;
bool	mRender::show_window = false;
bool	mRender::mInitialized = false;
bool    mRender::ImGuiCursorUsage = false;
bool	mRender::mRenderState = false;


void mRender::CreateDevice()
{
	game_ResizeBuffersAddr = gmAddress::Scan("48 89 5C 24 ?? 48 89 74 24 ?? 57 48 81 EC 90 00 00 00 48 8B F1 48 8D 0D");	
	game_PresentImageAddr = gmAddress::Scan("40 55 53 56 57 41 54 41 56 41 57 48 8B EC 48 83 EC 40 48 8B 0D");	// EndFrame
	window = *game_ResizeBuffersAddr.GetRef(54 + 3).To<HWND*>();

#if game_version == gameVer3095

	game_WndProcAddr = gmAddress::Scan("48 8D 05 ?? ?? ?? ?? 33 C9 89 75 20").GetRef(3);

#elif game_version == gameVer2060
	
	game_WndProcAddr = gmAddress::Scan("85 C0 BF 00 00 CA 00").GetAt(80).GetAt(3).GetRef();

#endif 

////#if game_version == gameVer3095

////	//static gmAddress gAddr = gmAddress::Scan("48 8B 05 ?? ?? ?? ?? BE 08 00 00 00");
////	//p_context = *gAddr.GetRef(3).To<ID3D11DeviceContext**>();
////	//p_device = *gAddr.GetRef(-19 - 4).To<ID3D11Device**>();

////	p_SwapChain = *game_ResizeBuffersAddr.GetAt(33).GetRef(3).To<IDXGISwapChain**>();
////	//p_SwapChain = *gAddr.GetRef(-194 - 4).To<IDXGISwapChain**>();

////#elif game_version == gameVer2802

////	static gmAddress gAddr = gmAddress::Scan("48 8B C4 55 53 56 57 41 54 41 55 41 56 41 57 48 8D A8 38 F7");
////	//p_device = *gAddr.GetRef(0x622 + 0x3).To<ID3D11Device**>();
////	//p_context = *gAddr.GetRef(0x602 + 0x3).To<ID3D11DeviceContext**>();
	
////	p_SwapChain = *game_ResizeBuffersAddr.GetAt(33).GetRef(3).To<IDXGISwapChain**>();
////	//p_SwapChain = *gAddr.GetRef(0x631 + 0x3).To<IDXGISwapChain**>();
	
////#elif game_version == gameVer2060
	
////	//static gmAddress gAddr = gmAddress::Scan("48 8D 05 ?? ?? ?? ?? 45 33 C9 48 89 44 24 58 48 8D 85 D0 08 00 00");
////	//p_context = *gAddr.GetRef(3).To<ID3D11DeviceContext**>();		
////	//p_device = *gAddr.GetRef(33).To<ID3D11Device**>();		
////	//p_SwapChain = *gAddr.GetRef(47).To<IDXGISwapChain**>();

////#endif

	p_SwapChain = *game_ResizeBuffersAddr.GetAt(33).GetRef(3).To<IDXGISwapChain**>();

	if (SUCCEEDED(p_SwapChain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(&p_device))))
	{
		p_device->GetImmediateContext(&p_context);
	}

	p_context->AddRef();
	p_device->AddRef();
	p_SwapChain->AddRef();
}




void (*orig_ClipCursor)(LPRECT);
void mRender::n_ClipCursor(LPRECT rect) 
{
	if (!show_window)
		orig_ClipCursor(rect);
}

int (*orig_ShowCursor)(bool);
int mRender::n_ShowCursor(bool visible) 
{
	if (!show_window)
		orig_ShowCursor(visible);
	
	return visible ? 0 : -1; 
}

void mRender::SetMouseVisible(bool visible)
{
	if (visible)
		while (orig_ShowCursor(true) < 0);
	else
		while (orig_ShowCursor(false) >= 0);
}

void ClipCursorToWindowRect(HWND handle, bool clip)
{
	RECT rect;
	GetWindowRect(handle, &rect);
	orig_ClipCursor(clip ? &rect : NULL);
}




LRESULT(*orig_WndProc)(HWND, UINT, WPARAM, LPARAM);
LRESULT mRender::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (GetAsyncKeyState(open_window_btn) & 1) 
	{
		show_window = !show_window;
		
		if (!ImGuiCursorUsage) 
			ClipCursorToWindowRect(window, !show_window);
	}
	
	if (GetAsyncKeyState(VK_HOME) && show_window)
		return true;
	
	if (!ImGuiCursorUsage) 
		SetMouseVisible(show_window); 

	if (show_window)
	{
		//it handles mouse input even when ui isn't displayed, so i placed it under "show_window" flag
		if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) 
			return true;
	}
	
	/*return*/ orig_WndProc(hWnd, uMsg, wParam, lParam);
	return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}


void(*orig_PresentImage)();
void mRender::PresentImage()
{
	mRenderState = true;

	if (!mInitialized)
	{
		InitBackend();
		BaseUiWindow::Create();
		ScriptHook::Start(); // <-- call from gta thread

		mInitialized = true;
	}
	if (mInitialized && show_window)
	{
		ImRenderFrame();
	}
	orig_PresentImage();
	
	mRenderState = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void mRender::Init()
{
	WaitWhileGameIsStarting();

	if (shutdown_request)
		return;

	CClock::Init();	
	CreateDevice();
	Hook::Create(game_PresentImageAddr,	mRender::PresentImage,	&orig_PresentImage,	"swapChainPresent");
	Hook::Create(game_WndProcAddr,		mRender::WndProc,		&orig_WndProc,		"WndProc");

	if (!ImGuiCursorUsage)
	{
		Hook::Create(ClipCursor, mRender::n_ClipCursor, &orig_ClipCursor, "ClipCursor");
		Hook::Create(ShowCursor, mRender::n_ShowCursor, &orig_ShowCursor, "ShowCursor");
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void mRender::InitBackend()
{	
	ImGui::CreateContext();
	ImPlot::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	mStyle();
	LoadWindowsFont();

	if (ImGuiCursorUsage)
		io.MouseDrawCursor = true;

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(p_device, p_context);
}


void mRender::ImRenderFrame()
{
	GameInput::DisableAllControlsThisFrame();

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	BaseUiWindow::GetInstance()->OnRender();
	
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void mRender::Shutdown()
{
	shutdown_request = true;

	if (!mInitialized) 
		return;	

	show_window = false;
	while (mRenderState) {};

	BaseUiWindow::Destroy();

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	
	ImPlot::DestroyContext();
	ImGui::DestroyContext();

	if (p_context)
	{
		p_context->Release(); p_context = nullptr;
	}
	if (p_device)
	{
		p_device->Release(); p_device = nullptr;
	}
	if (p_SwapChain)
	{
		p_SwapChain->Release(); p_SwapChain = nullptr;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void mRender::LoadWindowsFont()
{
	ImGuiIO& io = ImGui::GetIO();

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

	ImFontConfig regularConfig{};
	{
		regularConfig.RasterizerMultiply = 1.3f;
		regularConfig.OversampleH = 2;
		regularConfig.OversampleV = 1;

		//regularConfig.RasterizerDensity = 1.2f;
	}

	io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\calibri.ttf", 15.0f, &regularConfig, fontRange);
}


void mRender::mStyle()
{
	ImGuiStyle* style = &ImGui::GetStyle();
	
	style->GrabRounding = 4.0f;
	style->CellPadding = { 0, 3 };
	style->FramePadding = { 4, 4 };

	style->Colors[ImGuiCol_PopupBg] = { 45.0f/255.0f, 45.0f/255.0f, 45.0f/255.0f, 1.0f };
}


void mRender::SetCursorImguiUsage(bool state)
{
	ImGuiCursorUsage = state;
}

void mRender::SetOpenWindowBtn(int btn)
{
	open_window_btn = btn;
}


void WaitWhileGameIsStarting()
{
#if !am_version && game_version == gameVer3095
	static bool* is_game_rendering = gmAddress::Scan("48 83 c4 ?? 5f c3 cc 40 53 48 83 ec ?? 0f 29 74 24 ?? e8")
		.GetRef(-7 - 4)
		.To<bool*>();

	while (*is_game_rendering == 0)
	{
		if (shutdown_request)	// gets true in render::shutdown
			break;

		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
#endif
}
