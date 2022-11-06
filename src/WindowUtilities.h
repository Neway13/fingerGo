#pragma once

#include <Windows.h>
#include <string>
#include <vector>  

using namespace std;



struct WindowInformation
{
    public:
        HWND  handle;
        HWND parent;
        HWND owner;
        LONG style;
        DWORD pid;  // process id
        DWORD tid;  // creator thread id
        wchar_t title[MAX_PATH];
        wchar_t cls[MAX_PATH];      // window class name.
    private:
        std::wstring text;
};

// It is used to pass parameters and results for window matching in method  EnumWindows.
struct   WindowMatchInfo {
    public:
        wchar_t pathPattern[MAX_PATH]; // Parameters: Program's executable file path
        std::vector<WindowInformation> winInfos; // Result: Window information that matche the parameters.
};

class WindowUtilities {

    public:
        // Determine if it is the main window.
        static bool isMainWindow(WindowInformation& windInfo);

        // Get window information by HWND.
        static WindowInformation getWindowInformation(HWND& hwnd);

        // Get the window of the matching program based on the full path of the executable file.
        static WindowMatchInfo getWindowByExePath(const wchar_t *targetExePath);

        // Get all running main windows.
        static WindowMatchInfo getAllRunningMainWindow();

        // activate a window by HWND.
        static void activateWindow(const HWND& hwnd);
};