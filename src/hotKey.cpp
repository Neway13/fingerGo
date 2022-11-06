
#include <locale>
#include <iostream>
#include <Windows.h>
#include <map>
#include <vector>
#include <algorithm>
#include "WindowUtilities.h"
#include "fileUtil.h"
#include "hotKey.h"
#include "lang.h"

using namespace std;

// The key is the program file path, value records the HWND of the last activated window of the program.
map<const wchar_t*, HWND> hadActivatedMap;
HWND hWndHotKey;
//Mapping information for hotkeys and programs.
std::map<int, KeyExeMap> keyConfigMap = {};
wchar_t USER_DIR[MAX_PATH] = { 0 };
wchar_t USER_CONFIG_FILE[MAX_PATH] = { 0 };
wchar_t USER_CONFIG_DIR[MAX_PATH] = { 0 };
void loadUserConfig(bool isRefresh);
bool initConfigFile();
bool registerMyHotKey(HWND& hWnd);
void unregisterALLMyHotKey(HWND& hWnd);
inline bool cmpWinInfo(WindowInformation w1, WindowInformation w2);
HWND getNextActivatedWindow(const wchar_t* pattern, vector<WindowInformation>& winInfos);
void activateOrRunWindow(WindowMatchInfo& windowMatchInfo, const wchar_t* pattern, bool ifRun);


bool HotKey::startHotKeyRegister(HWND& hWnd,bool isRefresh){
    hWndHotKey = hWnd;
    unregisterALLMyHotKey(hWnd);

    if (!initConfigFile()) return 0;

    loadUserConfig(isRefresh);

    return registerMyHotKey(hWnd);

}

void showMsg(const wchar_t* msg) {
    MessageBox(NULL, msg, L"FingerGo", MB_OK | MB_SYSTEMMODAL);
}


// Register hotKey. use virtual key code as hotkey id, use 'alt' as modifier.
bool registerMyHotKey(HWND& hWnd) {
    for (auto& it : keyConfigMap) {
        if (0 == RegisterHotKey(hWnd, it.second.mod, MOD_ALT, it.second.mod)) {
            wstring msg = ML.WARN_KEY_OCCUPIED + wstring({ wchar_t(it.second.mod)}) + L"] " ;
            showMsg(msg.c_str());
            continue;
        }
    }
    return true;

}

// Load the configuration from the user's configuration file. 
// The format of the configuration item in file is as follows: E=C://explore.exe
void loadUserConfig(bool isRefresh) {
    vector<wstring>  lines;
    FileUtil::readFileLines( USER_CONFIG_FILE ,lines);
    keyConfigMap.clear();
    for(auto it:lines){
        size_t pos = it.find(L"=");
        if(pos!=it.npos) {
            int key = it.at(0);
            wstring path = it.substr(pos + 1, it.size());
            keyConfigMap[key] = KeyExeMap(key, path);
        }
    }
    if (!isRefresh && lines.size() == 0) {
        showMsg(ML.INFO_CONFIG_TIP);
    }
}

// Initializes the configuration file. if it does not exist, create it. 
bool initConfigFile(){
    // Obtaining the user's home directory.
    GetEnvironmentVariable(USERPROFILE, USER_DIR, 128);

    // Directory for configuration files.
    wcscpy_s(USER_CONFIG_DIR, USER_DIR);
    wcscat_s(USER_CONFIG_DIR, L"\\.fingerGo");

    // The full path to the configuration file
    wcscpy_s(USER_CONFIG_FILE,USER_DIR);
    wcscat_s(USER_CONFIG_FILE,L"\\.fingerGo\\fingerGo.ini");

    // Create user's configuration directories and configuration files. If they exist, do nothing.
    return FileUtil::createDir(USER_CONFIG_DIR) && FileUtil::createFile(USER_CONFIG_FILE);

}

// Delete all hotkeys.In this program, use letters as hotkey ids.
void unregisterALLMyHotKey(HWND& hWnd) {
    for (int i = 'A'; i <= 'Z'; i++) {
        UnregisterHotKey(hWnd, i);
    }
}

// This method is executed when the hotkey fires. The parameter is the ID of the hotkey.
void HotKey::handleHotKeyMsg(WPARAM& wParam) {
    auto  it = keyConfigMap.find(wParam);
    if (it != keyConfigMap.end()) {
        WindowMatchInfo windowMatchInfo = WindowUtilities::getWindowByExePath((it->second.exepath).c_str());
        activateOrRunWindow(windowMatchInfo, (it->second.exepath).c_str(), true);
    }
}

/* Activate window.
@Param windowMatchInfo: The available window that matched.
@Param pattern: Full file path of program.
@Param ifRun: Whether to start the program, if window of program is not exist.
*/
void activateOrRunWindow(WindowMatchInfo& windowMatchInfo, const wchar_t* pattern, bool ifRun = false) {

    if (windowMatchInfo.winInfos.empty()) {
        if (ifRun) {
            HINSTANCE hins = ShellExecute(GetDesktopWindow(), L"", pattern, L"", L"", SW_SHOWNORMAL);
            if ((int)hins <= 32) {
                wstring msg = ML.WARN_NO_EXE + wstring(pattern) + ML.INFO_RUNFAILED_REASON;
                showMsg(msg.c_str());
            }
        }
    }
    else if (windowMatchInfo.winInfos.size() == 1 ) {
        if (windowMatchInfo.winInfos.at(0).handle == GetForegroundWindow()) {
            ShowWindow(windowMatchInfo.winInfos.at(0).handle, SW_MINIMIZE);
        }
        else {
            WindowUtilities::activateWindow(windowMatchInfo.winInfos.at(0).handle);
        }
        
    }
    else {
        HWND next = getNextActivatedWindow(pattern, windowMatchInfo.winInfos);
        hadActivatedMap[pattern] = next;
        WindowUtilities::activateWindow(next);
    }


}

// Gets the next window to activate.
HWND getNextActivatedWindow(const wchar_t* pattern, vector<WindowInformation>& winInfos) {
    HWND fgHwnd = GetForegroundWindow();
    HWND hadActivatedHwnd = hadActivatedMap[pattern];

    // Mark whether the target program is already active.
    bool alreadyActivated = false;
    // Mark whether the target program had been switched and the window still exists.
    bool hadActivated = false;
    for (auto& it : winInfos) {
        if (it.handle == fgHwnd) {
            alreadyActivated = true;
        }
        else if (hadActivatedHwnd != nullptr && it.handle == hadActivatedHwnd) {
            hadActivated = true;
        }
    }

    if (!alreadyActivated && hadActivated) {
        return hadActivatedHwnd;
    }

    HWND  nextWin = NULL;
    // Sort, easy to find the next window according to the order.
    sort(winInfos.begin(), winInfos.end(), cmpWinInfo);
    for (auto& it : winInfos) {
        if (it.handle > fgHwnd) {
            nextWin = it.handle;
            break;
        }
    }

    // If the currently active window is already the target program, and there's another one, then switch to the next window of the program.
    if (alreadyActivated && nextWin) {
        return nextWin;
    } else {
        // Returns the first window after sorting.
        return winInfos.front().handle;
    }

}

// Sort by HWND value from smallest to largest.
inline bool cmpWinInfo(WindowInformation w1, WindowInformation w2) {
    return w1.handle < w2.handle;
}