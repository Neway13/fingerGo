#include "regeditUtil.h"
#include "string"
void showErrorMsg(std::wstring msg) {
    MessageBox(NULL, msg.c_str(), L"ERROR", MB_OK | MB_ICONINFORMATION);
}


bool RegeditUtil::getMachineGuid(wchar_t* outValue,bool is64bitSystem) {
    bool result = false;
    HKEY hkey;
    long regCode;
    do {
        REGSAM samDesired = is64bitSystem ? KEY_QUERY_VALUE | KEY_WOW64_64KEY : KEY_QUERY_VALUE;
        regCode = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Cryptography", 0, samDesired, &hkey);
        if (regCode != ERROR_SUCCESS) {
            showErrorMsg(L"Failed to check automatic startup:" + std::to_wstring(regCode));
            break;
        }

        DWORD reg_value_size = 128;
        regCode = RegQueryValueEx(hkey, L"MachineGuid", NULL, NULL, (LPBYTE)outValue, &reg_value_size);
        // Automatic startup has been set and the path is correct.
        if (regCode == ERROR_SUCCESS) {
            result = true;
            break;
        }
        else if (regCode == ERROR_FILE_NOT_FOUND) {
            break;
        }

        //showErrorMsg(L"Failed to check automatic startup:" + std::to_wstring(regCode));
        break;
    } while (true);


    if (NULL != hkey) {
        RegCloseKey(hkey);
    }
    return result;
}

bool RegeditUtil::checkRegedit(const wchar_t* regPath, const wchar_t* regKey, const wchar_t* expectedValue) {
    bool result = false;
    HKEY hkey;
    long regCode;
    do {
        regCode = RegOpenKeyEx(HKEY_CURRENT_USER, regPath, 0, KEY_QUERY_VALUE, &hkey);
        if (regCode != ERROR_SUCCESS) {
            showErrorMsg(L"Failed to check automatic startup:" + std::to_wstring(regCode));
            break;
        }

        wchar_t reg_value[MAX_PATH]{ '\0'};
        DWORD reg_value_size = sizeof(reg_value);
        regCode = RegQueryValueEx(hkey, regKey, NULL, NULL, (LPBYTE)reg_value, &reg_value_size);
        // Automatic startup has been set and the path is correct.
        if (regCode == ERROR_SUCCESS) {
            if (reg_value != NULL && wcsstr(expectedValue, reg_value) != NULL) {
                result = true;
            }
            break;
        }
        else if (regCode == ERROR_FILE_NOT_FOUND) {
            break;
        }

        //showErrorMsg(L"Failed to check automatic startup:" + std::to_wstring(regCode));
        break;
    } while (true);


    if (NULL != hkey) {
        RegCloseKey(hkey);
    }
    return result;
}
// Add boot to the registry
bool RegeditUtil::addRegedit(const wchar_t* regPath, const wchar_t* regKey, const wchar_t* regValue) {

    bool result = false;
    HKEY hKey;
    do {
        // Open the registry key or create it if it does not exist
        long regCode = RegCreateKeyEx(HKEY_CURRENT_USER, regPath, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);
        if (regCode == ERROR_SUCCESS)
        {
            //Modify the registry key value, or create it if none exists
            regCode = RegSetValueEx(hKey, regKey, 0, REG_SZ, (BYTE*)regValue, (wcslen(regValue)+1) * sizeof(wchar_t));
            if (regCode == ERROR_SUCCESS) {
                result = true;
                break;
            }
        }
        showErrorMsg(L"Failed to set automatic startup:" + std::to_wstring(regCode));
        break;
    } while (true);

    if (NULL != hKey) {
        RegCloseKey(hKey);
    }
    return result;
}


bool RegeditUtil::deleteRegedit(const wchar_t* regPath, const wchar_t* regKey) {
    bool result = false;
    HKEY hkey;
    do {
        long regCode = RegOpenKeyEx(HKEY_CURRENT_USER, regPath, 0, KEY_ALL_ACCESS | KEY_WOW64_64KEY, &hkey);
        if (regCode == ERROR_SUCCESS) {
            regCode = RegDeleteValue(hkey, regKey);
            if (regCode == ERROR_SUCCESS || regCode == ERROR_FILE_NOT_FOUND) {
                result = true;
                break;
            }
        }
        showErrorMsg(L"Failed to c cancel startup:" + std::to_wstring(regCode));
        break;
    } while (true);

    if (NULL != hkey) {
        RegCloseKey(hkey);
    }
    return result;
}