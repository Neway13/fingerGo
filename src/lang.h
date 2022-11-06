#pragma once
#include <set>

enum Language {
	Chinese, English
};
class Lang {
public:
	Language LNAGUAGE;
	const wchar_t* AUTO_BOOT;
	const wchar_t* ABOUT_TXT;
	const wchar_t* CONFIG;
	const wchar_t* HELP;
	const wchar_t* HELP_DETAIL;
	const wchar_t* ABOUT;
	const wchar_t* EXIT;
	const wchar_t* WIN_TITLE;
	const wchar_t* WIN_WARN;
	const char* TIP_WINLIST;
	const char* WIN_HOTKEYCONFIG;
	const char* WIN_REFRESH;
	const char* WIN_ADD;
	const char* WIN_SAVE;
	const char* WIN_DELETE;
	const char* WIN_NO_OPEN_WARN;
	const char* WIN_KET_LABEL;
	const char* WIN_EXE_LABEL;
	const wchar_t* WARN_REPEAT;
	const wchar_t* WARN_CHECK;
	const wchar_t* WARN_BLANK;
	const wchar_t* WARN_SAVE_SUCCESS;
	const char* WIN_DONATE_TEA;
	const wchar_t* WARN_KEY_OCCUPIED;
	const wchar_t* WARN_NO_EXE;
	const wchar_t* INFO_AUTOBOOT_OFF;
	const wchar_t* INFO_AUTOBOOT_ON;
	const wchar_t* INFO_RUNNING;
	const wchar_t* INFO_CONFIG_TIP;
	const wchar_t* INFO_RUNFAILED_REASON;
	static void setCh();
	static void setEn();
};
// Puts character into the font character set.
const char* _Font(const char *text);
// The current language.
extern Lang ML;
// Font character set.
extern std::set<const char*> fontSet;
 
 
 
 
    
 