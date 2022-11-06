// Dear ImGui: application for DirectX 11
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#include <set>
#include "windowForConfig.h";
#include "WindowUtilities.h"
#include "hotKey.h"
#include "stringUtil.h"
#include "fileUtil.h"
#include "lang.h"
#include "framework.h"
#include <Shlobj.h>
#include "./resourse/resource.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "imgUtil.h"
#include "imgui_internal.h"



void getRunningWindowExeInfo(set<string>& exes);
bool saveConfig(std::map<int, KeyExeMapA> configItems);

//Forward declarations of helper functions for imgui wondow. 
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
void helpMarker(const char* desc);

bool showDonateTea(TextureInfo& textureInfo);
bool showExeIco(const char* exePath, TextureInfo& textureInfo);
bool getExeIcon(const char* exePath, ID3D11ShaderResourceView** out_srv, int* width, int* height);

bool getTextureByRcId(int rcid, const wchar_t* type, ID3D11ShaderResourceView** out_srv, int* donateTeaWidth, int* donateTeaHeight);
bool getTextureByImgHandle(HBITMAP& in_hbitmap, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height);
bool getTextureByImgBinaryData(byte** in_imgdata, int in_len, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height);



const char* items[] = { "A", "B", "C", "D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z" };
WNDCLASSEX imguiWc;


// Launch window entry for configuring shortcut keys.
int lanchWindowForConfig(const HWND& mainMWnd)
{
    hotKeyHwnd = mainMWnd;
    // If "done" is false, it means that there is already a configuration window running.
    if (!done) {
        ::SwitchToThisWindow(imguiHwnd, true);
        return 0;
    }
    done = false;

    // Create application window
    ImGui_ImplWin32_EnableDpiAwareness();
    imguiWc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProcForImguie, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("FingerGO Config"), NULL };
    ::RegisterClassEx(&imguiWc);
    imguiHwnd = ::CreateWindow(imguiWc.lpszClassName, ML.WIN_TITLE, WS_OVERLAPPEDWINDOW, 100, 100, 1250, 600, NULL, NULL, imguiWc.hInstance, NULL);

    // Initialize Direct3D
    if (!CreateDeviceD3D(imguiHwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClass(imguiWc.lpszClassName, imguiWc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(imguiHwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(imguiHwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.IniFilename = NULL;
    // Setup Dear ImGui style
    ImGui::StyleColorsLight();
    ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    ImGui::GetStyle().Colors[ImGuiCol_FrameBg] = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
    ImGui::GetStyle().Colors[ImGuiCol_Border] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    
    ImGui::GetStyle().ItemSpacing = ImVec2(8,8);
    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(imguiHwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    // Load Fonts
    if (ML.LNAGUAGE == Chinese) {
        ImVector<ImWchar> ranges;
        ImFontGlyphRangesBuilder builder;
        for (auto it : fontSet) {
            builder.AddText(it);
        }                 
        //builder.AddRanges(io.Fonts->GetGlyphRangesDefault());
        builder.AddRanges(io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
        builder.BuildRanges(&ranges);                          

        char fontPath[MAX_PATH] = { 0 };
        SHGetSpecialFolderPathA(GetDesktopWindow(), fontPath, CSIDL_WINDOWS, FALSE);
        static char fontPostfix[MAX_PATH] = "\\Fonts\\msyh.ttc";
        strcat_s(fontPath , fontPostfix);
        io.Fonts->AddFontFromFileTTF(fontPath, 24.0f, NULL, ranges.Data);
        io.Fonts->Build();                                      
        
    }
    else {
        ImFontConfig config;
        config.SizePixels = 20.0f;
        io.Fonts->AddFontDefault(&config);
    }

    // Program file path of running window.
    static set<string> exes;
    getRunningWindowExeInfo(exes);

    // Copy the old configuration to the current configuration state. For the following reasons, it needs to be converted and stored in configMapStatus:
    // 1. the mod value of KeyExeMap cannot correspond to the items of combo, for example, mod is A, and the index corresponding to item is 0.
    // 2. Wide characters are not supported in imgui.
    std::map<int, KeyExeMapA> configMapStatus;
    for (auto it : keyConfigMap) {
        string exepath = to_byte_string(it.second.exepath);
        configMapStatus[it.first] = KeyExeMapA(int(it.second.mod) - 'A', exepath);

    }

    int configItemIndex = 100; // starting index value of new configuration item, use it to control items.

    TextureInfo donateTeaTexture;
    std::map<string, TextureInfo> iconTextureMap;
    // Main loop
    while (!done)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // See the WndProc() function below for our to dispatch events to the Win32 backend.
        MSG msg;
        while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();


        static ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);

        bool p_open = true;
        ImGui::Begin("Set FingerGo", &p_open, flags);

        ImGui::Dummy(ImVec2(10, 10));
        ImGui::Text(ML.WIN_HOTKEYCONFIG);

        ImGui::SameLine();
        //if (ImGui::Button(ML.WIN_REFRESH))
        //    getActiveWindowExeInfo(exes);

        if (ImGui::Button(ML.WIN_ADD)) {
            static string blankString("");
            configMapStatus[configItemIndex++] = KeyExeMapA(-1, blankString);
        }
        ImGui::SameLine();
        if (ImGui::Button(ML.WIN_SAVE))
            saveConfig(configMapStatus);

        ImGui::SameLine();
        helpMarker(ML.TIP_WINLIST);

        ImGui::Separator();

        //ImGui::Dummy(ImVec2(20, 20));
        if (exes.empty()) {
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), ML.WIN_NO_OPEN_WARN);
        }

        ImGui::BeginChild("list pane", ImVec2(0, 0), true);

        float windowWidth = ImGui::GetWindowWidth();
        for (std::map<int, KeyExeMapA>::iterator iter = configMapStatus.begin(); iter != configMapStatus.end(); ) {

            ImGui::Text(ML.WIN_KET_LABEL);

            ImGui::SameLine();
            ImGui::SetNextItemWidth(100);
            ImGui::Combo((string("##key") + to_string(iter->first)).c_str(), &configMapStatus[iter->first].mod, items, IM_ARRAYSIZE(items));

            ImGui::SameLine();
            ImGui::Dummy(ImVec2(20, 20));

            ImGui::SameLine();
            ImGui::Text(ML.WIN_EXE_LABEL);

            //show icon
            ImGui::SameLine();
            if (configMapStatus[iter->first].exepath=="") {
                ImGui::Dummy(ImVec2(24, 24));
                
            } else {
                if (!iconTextureMap.count(configMapStatus[iter->first].exepath)) {
                    TextureInfo textureInfo;
                    iconTextureMap[configMapStatus[iter->first].exepath] = textureInfo;
                }
                showExeIco(configMapStatus[iter->first].exepath.c_str(), iconTextureMap[configMapStatus[iter->first].exepath]);
            }

            ImGui::SameLine();
            static float finalItemMaxX;
            static float comboItemMaxX;
            ImGui::SetNextItemWidth(windowWidth- ImGui::GetItemRectMax().x -(finalItemMaxX- comboItemMaxX)-20);
            if (ImGui::BeginCombo((string("##exe") + to_string(iter->first)).c_str(), configMapStatus[iter->first].exepath.c_str(), 0)) {
                // Refresh window info every time combo is clicked.
                getRunningWindowExeInfo(exes);
                for (auto exe : exes) {
                    const bool is_selected = configMapStatus[iter->first].exepath == exe;
                    if (ImGui::Selectable(exe.c_str(), is_selected)) {
                        configMapStatus[iter->first].exepath = string(exe);
                    }
                    if (is_selected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            comboItemMaxX = ImGui::GetItemRectMax().x;

            ImGui::SameLine();
            if (ImGui::Button((string(ML.WIN_DELETE) + "##" + to_string(iter->first)).c_str())) {
                configMapStatus.erase(iter++);
            }
            else {
                iter++;
            }
            ImGui::SameLine();
            helpMarker(ML.TIP_WINLIST); 
            finalItemMaxX = ImGui::GetItemRectMax().x;
        }
        // showDonate
        if (configMapStatus.size() > 5) {
            
            showDonateTea(donateTeaTexture);
        }

        ImGui::EndChild();

        ImGui::End();

        // Rendering
        ImGui::Render();

        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        g_pSwapChain->Present(1, 0); // Present with vsync
        //g_pSwapChain->Present(0, 0); // Present without vsync
    }

    if (donateTeaTexture.texture) {
        donateTeaTexture.texture->Release();
    }
    for (auto it : iconTextureMap) {
        if (it.second.texture!=NULL) {
            it.second.texture->Release();
        }
    }
    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(imguiHwnd);
    ::UnregisterClass(imguiWc.lpszClassName, imguiWc.hInstance);

    return 0;
}

 

// Get the currently running window
void getRunningWindowExeInfo(set<string>& exes) {
    exes.clear();
    std::vector<WindowInformation> winInfos = WindowUtilities::getAllRunningMainWindow().winInfos;

    for (auto it : winInfos) {
        // Eliminate the window of fingerGo.
        if (it.handle == imguiHwnd || wcsstr(it.cls, L"ApplicationFrameWindow") != NULL ) {
            continue;
        }
        HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION, false, it.pid);
        char szFullPath[MAX_PATH];
        DWORD dwSize = MAX_PATH;
        if (hProcess != NULL && ::QueryFullProcessImageNameA(hProcess, 0, szFullPath, &dwSize)) {
            exes.insert(szFullPath);
        }
    }

}

// Save configuration to file.
bool saveConfig(std::map<int, KeyExeMapA> configItems) {
    vector<wstring> lines;
    // Used to check whether the key is repeated.
    set<int> keySet;
    bool warn = false;
    for (auto it : configItems) {
        if (it.second.mod > -1 && it.second.exepath != "") {
            if (keySet.insert(it.second.mod + 'A').second) {
                // Convert the index in the combo to the wstring of the key.
                wstring keyString = wstring({ (wchar_t)(it.second.mod + 'A') });
                // The format of the configuration item in file is as follows: A=C://explore.exe .
                lines.push_back(keyString + L"=" + to_wide_string(it.second.exepath));
            }
            else {
                wchar_t key[] = { it.second.mod + 'A','\0' };
                wstring msg = ML.WARN_REPEAT + wstring(key);
                showMsg(msg.c_str());
                return false;
            }

        }
        else {
            showMsg(ML.WARN_CHECK);
            return false;
        }
    }
    /*
    if (lines.size() == 0) {
        showMsg(ML.WARN_BLANK);
        return false;
    }
    */

    FileUtil::writeFile(USER_CONFIG_FILE, lines);
    showMsg(ML.WARN_SAVE_SUCCESS);
    HotKey::startHotKeyRegister(hotKeyHwnd, true);
        
    return true;

}

void helpMarker(const char* desc) {
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { 
        g_pSwapChain->Release(); 
        g_pSwapChain = NULL; 
    }
    if (g_pd3dDeviceContext) { 
        g_pd3dDeviceContext->Release();
        g_pd3dDeviceContext = NULL; 
    }
    if (g_pd3dDevice) { 
        g_pd3dDevice->Release(); 
        g_pd3dDevice = NULL; 
    }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProcForImguie(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
            CreateRenderTarget();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        done = true;
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
// Simple helper function to load an image into a DX11 texture with common settings
bool showDonateTea(TextureInfo& textureInfo) {

    if (textureInfo.texture == NULL) {
        bool ret = getTextureByRcId(IDI_DONATE_TEA, L"JPG", &textureInfo.texture, &textureInfo.width, &textureInfo.height);
    }
    ImGui::Dummy(ImVec2(20, 20));

    // Show WIN_DONATE_TEA.
    static float DONATE_TEA_WIDTH= ImGui::CalcTextSize(ML.WIN_DONATE_TEA, NULL, true).x;
    ImGui::Dummy(ImVec2((ImGui::GetWindowSize().x - DONATE_TEA_WIDTH) * 0.5f, 1)); ImGui::SameLine();
    ImGui::Text(ML.WIN_DONATE_TEA);

    // Show image.
    ImGui::Dummy(ImVec2((ImGui::GetWindowSize().x - textureInfo.width) * 0.5f, 1)); ImGui::SameLine();
    ImGui::Image((void*)textureInfo.texture, ImVec2(textureInfo.width, textureInfo.height));

    return true;
}

bool getTextureByRcId(int rcid, const wchar_t* type, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height) {

    try {
        HMODULE hMod = GetModuleHandle(NULL);
        HRSRC hRes = FindResource(hMod, MAKEINTRESOURCE(rcid), type);
        HGLOBAL hGlobal = LoadResource(hMod, hRes);
        byte* buffer = (byte*)LockResource(hGlobal);
        DWORD dataBytes = SizeofResource(hMod, hRes);
        getTextureByImgBinaryData(&buffer, dataBytes, out_srv, out_width, out_height);
        UnlockResource(hGlobal);
        FreeResource(hGlobal);
    }
    catch (exception e) {
        return false;
    }
    return true;
}

bool showExeIco(const char* exePath, TextureInfo& textureInfo) {
    try {
        if (textureInfo.texture == NULL) {
            bool ret = getExeIcon(exePath, &textureInfo.texture, &textureInfo.width, &textureInfo.height);
        }
        if (textureInfo.texture != NULL) {
            ImGui::Image((void*)textureInfo.texture, ImVec2(textureInfo.width / 2, textureInfo.height / 2));
            return true;
        }

        ImGui::Dummy(ImVec2(24, 24));

    }
    catch (exception e) {
        return false;
    }
    return true;
}

bool getExeIcon(const char* exePath, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height) {

    HICON hIcon = ::ExtractIconA(0, exePath, 0);
    if (hIcon==NULL) {
        *out_srv = NULL;
        return false;
    }
    byte* imgdata = NULL;
    int len = 0;
    if (!ImgUtil::getBinaryDataByIconHandle(hIcon, &imgdata, &len, out_width, out_height)) {
        goto done;
    }
    getTextureByImgBinaryData(&imgdata, len, out_srv, out_width, out_height);
done:
    if (hIcon) {
        DestroyIcon(hIcon);
    }
    if (imgdata != NULL)
        delete[] imgdata;
    return out_srv != NULL;

}



bool getTextureByImgHandle(HBITMAP &in_hbitmap, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height) {

    byte* imgdata = NULL;
    int len=0;
    if (!ImgUtil::getBinaryDataByImgHandle(in_hbitmap, &imgdata, &len, out_width, out_height)) {
        goto done;
    }
    getTextureByImgBinaryData(&imgdata, len, out_srv, out_width, out_height);
done:

    if (imgdata != NULL)
        delete[] imgdata;

    return out_srv != NULL;

}
 

bool getTextureByImgBinaryData(byte** in_imgdata, int in_len,ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height) {

    unsigned char* image_data = stbi_load_from_memory((stbi_uc*)*in_imgdata, in_len, out_width, out_height, NULL, 4);
    if (image_data == NULL)
        return false;

    // Create texture
    D3D11_TEXTURE2D_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.Width = *out_width;
    desc.Height = *out_height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;

    ID3D11Texture2D* pTexture = NULL;
    D3D11_SUBRESOURCE_DATA subResource;
    subResource.pSysMem = image_data;
    subResource.SysMemPitch = desc.Width * 4;
    subResource.SysMemSlicePitch = 0;
    g_pd3dDevice->CreateTexture2D(&desc, &subResource, &pTexture);

    // Create texture view
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = desc.MipLevels;
    srvDesc.Texture2D.MostDetailedMip = 0;

    g_pd3dDevice->CreateShaderResourceView(pTexture, &srvDesc, out_srv);
    pTexture->Release();
    stbi_image_free(image_data); 
    return true;
}