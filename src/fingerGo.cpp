
#include "framework.h"
#include "fingerGo.h"
#include "ShellAPI.h"  
#include <thread>
#include "hotKey.h"
#include "windowForConfig.h"
#include <map>
#include "lang.h"
#include "string"
#include "regeditUtil.h"
#include <ShellScalingAPI.h>
#include "stringUtil.h"

#define MAX_LOADSTRING 100
#define WM_TRAY (WM_USER + 100)
#define APP_NAME	L"FingerGo"
#define REG_PATH	L"Software\\Microsoft\\Windows\\CurrentVersion\\Run"

wchar_t filePath[MAX_PATH]; // The path of current execution file.
HINSTANCE hInst;
HWND hWndTray;   //Window of tray.
NOTIFYICONDATA nid;		//The tray attributes.
HMENU hMenu;			//The tray menu.
bool is64bit =false;			 

void setLanguage();
ATOM registerMyClass(HINSTANCE hInstance);
BOOL initInstance(HINSTANCE, int);
void handleTrayClick(const HWND& hWnd);
void handleTrayMsg(const LPARAM& lParam, const HWND& hWnd);
LRESULT CALLBACK wndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);
void showTrayMsg(const wchar_t* msg);
void initTray(HINSTANCE hInstance, HWND hWnd);
void toggleAutoboot();
 

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{   
    SetProcessDpiAwareness(PROCESS_SYSTEM_DPI_AWARE);

    setLanguage();

    // Check whether Fingergo is already running.
    HWND existedHwnd = FindWindow(APP_NAME, NULL);
    if (existedHwnd != NULL) {
        MessageBox(existedHwnd, ML.INFO_RUNNING, APP_NAME, MB_OK);
        return  0;
    }


    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);


    // Get the name of the current execution file (including the path)
    wchar_t tempFilePath[MAX_PATH] = {0};
    GetModuleFileName(NULL, tempFilePath, MAX_PATH);
    wcscat_s(filePath, L"\"");
    wcscat_s(filePath,tempFilePath);
    wcscat_s(filePath, L"\"");

    registerMyClass(hInstance);

    // Perform application initialization.
    if (!initInstance(hInstance, nCmdShow)) {
        return FALSE;
    }

    // Register hotKey.
    HotKey::startHotKeyRegister(hWndTray,false);

    MSG msg;
    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, 0, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

void setLanguage() {

    wchar_t localeName[LOCALE_NAME_MAX_LENGTH] = { 0 };
    int ret = GetUserDefaultLocaleName(localeName, LOCALE_NAME_MAX_LENGTH);
    if (ret != 0 && wcscmp(L"zh-CN", localeName) == 0) {
        Lang::setCh();
    }
    else {
        Lang::setEn();
    }
}

ATOM registerMyClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = NULL;
    wcex.lpfnWndProc = wndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = NULL;
    wcex.hCursor = NULL;
    wcex.hbrBackground = NULL;
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = APP_NAME;
    wcex.hIconSm = NULL;

    return RegisterClassExW(&wcex);
}



// Save the instance handle and create the main window
BOOL initInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;

    hWndTray = CreateWindowEx(WS_EX_TOOLWINDOW, APP_NAME, APP_NAME, WS_POPUP, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
    if (!hWndTray)
    {
        return FALSE;
    }

    ShowWindow(hWndTray, nCmdShow);
    UpdateWindow(hWndTray);

    return TRUE;
}

// Initialize the tray.
void initTray(HINSTANCE hInstance, HWND hWnd)
{
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hWnd;
    nid.uID = IDI_TRAY;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_INFO;
    nid.uCallbackMessage = WM_TRAY;
    nid.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TRAY));
    lstrcpy(nid.szTip, APP_NAME);

    //Create tray menu.
    hMenu = CreatePopupMenu();

    //Add options to the tray menu
    if (RegeditUtil::checkRegedit(REG_PATH,APP_NAME, filePath)) {
        AppendMenu(hMenu, MF_STRING | MF_CHECKED, ID_AUTOBOOT, ML.AUTO_BOOT);
    }
    else {
        AppendMenu(hMenu, MF_STRING, ID_AUTOBOOT, ML.AUTO_BOOT);
    }
    AppendMenu(hMenu, MF_STRING, ID_CONFIG, ML.CONFIG);
    AppendMenu(hMenu, MF_STRING, ID_HELP, ML.HELP);
    AppendMenu(hMenu, MF_STRING, ID_ABOUT, ML.ABOUT);
    AppendMenu(hMenu, MF_STRING, ID_EXIT, ML.EXIT);

    Shell_NotifyIcon(NIM_ADD, &nid);
}


LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static UINT WM_TASKBARCREATED;
    switch (message) {
    case WM_CREATE:
        WM_TASKBARCREATED = RegisterWindowMessage(TEXT("TaskbarCreated"));
        initTray(hInst, hWnd);
        break;
    case WM_TRAY:
        handleTrayMsg(lParam, hWnd);
        break;
    case WM_HOTKEY:
        HotKey::handleHotKeyMsg(wParam);
        break;
    case WM_DESTROY:
        Shell_NotifyIcon(NIM_DELETE, &nid);
        PostQuitMessage(0);
        break;
    default:
        if (message == WM_TASKBARCREATED)
            SendMessage(hWnd, WM_CREATE, wParam, lParam);
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


void handleTrayMsg(const LPARAM& lParam, const HWND& hWnd)
{
    switch (lParam) {
    case WM_RBUTTONDOWN:
        handleTrayClick(hWnd);
        break;
    case WM_LBUTTONDOWN:
        handleTrayClick(hWnd);
        break;
    case WM_LBUTTONDBLCLK:
        break;
    }
}

void handleTrayClick(const HWND& hWnd)
{

    POINT pt; GetCursorPos(&pt);

    //Solve the problem that the menu does not disappear when left-clicking outside the menu
    SetForegroundWindow(hWnd);

    //gray out a menu item
    //EnableMenuItem(hMenu, ID_SHOW, MF_GRAYED);	

    //Show and get the selected menu
    int cmd = TrackPopupMenu(hMenu, TPM_RETURNCMD, pt.x, pt.y, NULL, hWnd, NULL);
    if (cmd == ID_AUTOBOOT) {
        toggleAutoboot();
    }
    else if (cmd == ID_CONFIG) {
        lanchWindowForConfig(hWnd);
    }
    else if (cmd == ID_HELP) {
        MessageBox(hWnd, ML.HELP_DETAIL, APP_NAME, MB_OK);
    }
    else if (cmd == ID_ABOUT) {
        MessageBox(hWnd, ML.ABOUT_TXT, APP_NAME, MB_OK);
    }
    else if (cmd == ID_EXIT) {
        PostMessage(hWnd, WM_DESTROY, NULL, NULL);
    }
}



void showTrayMsg(const wchar_t* msg)
{
    lstrcpy(nid.szInfoTitle, APP_NAME);
    lstrcpy(nid.szInfo, msg);
    nid.uTimeout = 1000;
    Shell_NotifyIcon(NIM_MODIFY, &nid);
}
void toggleAutoboot()
{
    if (GetMenuState(hMenu, ID_AUTOBOOT, MF_BYCOMMAND == MF_CHECKED)) {
        if (RegeditUtil::deleteRegedit(REG_PATH, APP_NAME)) {
            CheckMenuItem(hMenu, ID_AUTOBOOT, MF_UNCHECKED);
            MessageBox(NULL, ML.INFO_AUTOBOOT_OFF, L"INFO", MB_OK | MB_ICONINFORMATION);
        }
    }
    else {
        if (RegeditUtil::addRegedit(REG_PATH, APP_NAME, filePath)) {
            CheckMenuItem(hMenu, ID_AUTOBOOT, MF_CHECKED);
            MessageBox(NULL, ML.INFO_AUTOBOOT_ON, L"INFO", MB_OK | MB_ICONINFORMATION);
        }
    }
}
 

 