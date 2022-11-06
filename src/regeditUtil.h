#pragma once

#include "framework.h"

class RegeditUtil{

public:
	static bool getMachineGuid(wchar_t* expectedValue, bool is64bitSystem);
	static bool checkRegedit(const wchar_t* regPath, const wchar_t* regKey, const wchar_t* expectedValue);
	static bool addRegedit(const wchar_t* regPath, const wchar_t* regKey, const wchar_t* regValue);
	static bool deleteRegedit(const wchar_t* regPath, const wchar_t* regKey);

};