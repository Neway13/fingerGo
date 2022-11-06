
#include "WindowUtilities.h"
#include <iostream>
#include <sstream>
#include <locale>
#include <codecvt>
using namespace std;

bool WindowUtilities::isMainWindow(WindowInformation& windInfo)
{   
    DWORD visibleStyle = WS_VISIBLE;
    DWORD thickFrameStyle = WS_THICKFRAME;
    bool result = wcslen(windInfo.title) > 0 
                    && (windInfo.style & (visibleStyle)) != 0 
                    && (windInfo.style & (thickFrameStyle)) != 0 
                    && (windInfo.owner == nullptr) ? true : false;
    return result;
}

WindowInformation WindowUtilities::getWindowInformation(HWND& hwnd)
{
    WindowInformation windInfo;
    windInfo.handle = hwnd;
    windInfo.parent = GetParent(hwnd);
    windInfo.owner = GetWindow(hwnd, GW_OWNER);
    windInfo.style = GetWindowLong(hwnd, GWL_STYLE);
    GetWindowText(hwnd, windInfo.title, MAX_PATH);
    GetClassName(hwnd, windInfo.cls, MAX_PATH);
    windInfo.tid = GetWindowThreadProcessId(hwnd, &windInfo.pid);
    return windInfo;
}


BOOL CALLBACK  collectMainWindow(HWND hwnd, LPARAM lpamam) {

    WindowInformation winInfo = WindowUtilities::getWindowInformation(hwnd);
    if (!WindowUtilities::isMainWindow(winInfo)) {
        return true;
    }
    WindowMatchInfo& windowMatchInfo = *(WindowMatchInfo*)lpamam;
    windowMatchInfo.winInfos.push_back(winInfo);
    return true;
}

BOOL CALLBACK  collectWindowMatchByFullPath(HWND hwnd, LPARAM lpamam) {

    WindowInformation winInfo = WindowUtilities::getWindowInformation(hwnd);
    if (!WindowUtilities::isMainWindow(winInfo)) {
        return true;
    }
    WindowMatchInfo& windowMatchInfo = *(WindowMatchInfo*)lpamam;
    HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION, false, winInfo.pid);
    wchar_t exeFullPath[MAX_PATH] ;
    DWORD pathSize = MAX_PATH;

    if (hProcess != NULL && ::QueryFullProcessImageName(hProcess, 0, exeFullPath, &pathSize) && wcsstr(exeFullPath,windowMatchInfo.pathPattern)!=NULL) {
        windowMatchInfo.winInfos.push_back(winInfo);
    }
    return true;
}
WindowMatchInfo WindowUtilities::getWindowByExePath(const wchar_t *targetExePath) {
    WindowMatchInfo windowMatchInfo;
    wcscpy_s(windowMatchInfo.pathPattern,targetExePath );
    ::EnumWindows(collectWindowMatchByFullPath, (LPARAM)&windowMatchInfo);
    return windowMatchInfo;
}

WindowMatchInfo WindowUtilities::getAllRunningMainWindow() {
    WindowMatchInfo windowMatchInfo;
    ::EnumWindows(collectMainWindow, (LPARAM)&windowMatchInfo);
    return windowMatchInfo;
}

void WindowUtilities::activateWindow(const HWND& hwnd) {
    ::SwitchToThisWindow(hwnd, true);
}