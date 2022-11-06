#pragma once
#include <map>
#include <string>

// Mapping information for hotkeys and programs
struct KeyExeMap {
public:
    int mod; // VK of hotkeys.
    std::wstring exepath; // Full path of executable  program file.

    KeyExeMap() = default;
    KeyExeMap(int amod, std::wstring& path) {
        mod = amod;
        exepath = path;
    }

};


// Global variable, which stores user configuration.
extern std::map<int, KeyExeMap> keyConfigMap;

const static wchar_t USERPROFILE[] = L"USERPROFILE";

// User's home path
extern wchar_t USER_DIR[MAX_PATH];

//User's configuration dir 
extern wchar_t USER_CONFIG_DIR[MAX_PATH];

//User's configuration file
extern wchar_t USER_CONFIG_FILE[MAX_PATH];

extern void showMsg(const wchar_t* msg);



class HotKey{
    public:
        // Start the process of registering shortcut keys.
        static bool startHotKeyRegister(HWND& hWnd, bool isRefresh);

        // Handling hotKey messages.
        static void handleHotKeyMsg(WPARAM& wParam);

};