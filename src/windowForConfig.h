#pragma once
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>
#include <tchar.h>
#include <map>
#include <string>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "shcore.lib")


// Stores the binding relationship between executable file path and hotkey
struct KeyExeMapA {
public:
    int mod; // VK of hotkey
    std::string exepath; // Executable file path. 

    KeyExeMapA() = default;
    KeyExeMapA(int amod, std::string& path) {
        mod = amod;
        exepath = path;
    }

};

struct TextureInfo {
public:
    int width = 0;
    int height = 0;
    ID3D11ShaderResourceView* texture = NULL;

};
//Status marks whether the imgui window is complete.
static bool done = true;

static ID3D11Device* g_pd3dDevice = NULL;
static ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
static IDXGISwapChain* g_pSwapChain = NULL;
static ID3D11RenderTargetView* g_mainRenderTargetView = NULL;
static HWND imguiHwnd = NULL; // HWND of imgui window. 
static HWND hotKeyHwnd = NULL; // HWND of  main window.



LRESULT WINAPI WndProcForImguie(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Launch window entry for configuring shortcut keys.
int lanchWindowForConfig(const HWND &hWnd);

