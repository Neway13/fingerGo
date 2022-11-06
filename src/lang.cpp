
#include "lang.h"

Lang ML;
std::set<const char*> fontSet;

void Lang::setCh() {
	ML.LNAGUAGE = Chinese;
	ML.AUTO_BOOT =L"开机启动";
	ML.ABOUT_TXT = L"FingerGo\n--------------\n作者: Neway\n邮箱: lxwlxc@126.com";
	ML.CONFIG = L"配置";
	ML.HELP = L"帮助";
	ML.HELP_DETAIL = L""
		"📃任务栏图标太多，找程序找的眼睛疼，还找错？\n"
		"🤵被老板发现上班在摸鱼？"
		"\n--------------\n"
		"FingerGo功能：\n"
		"    帮你将按键与程序绑定，按下按键时快速切换到该程序。例如将 \"W\" 与 \"word程序\" 绑定，按下快捷键\"ALT + W\"即可快速切换到\"word程序\"。\n--------------\n"
		"使用场景，下面依然以word程序为例：\n"
		"    场景1：word没有打开——>快捷键将自动帮你打开word；\n"
		"    场景2：已经打开了一个word，但是word程序在后台——>快捷键将自动切换到word；\n"
		"    场景3：已经打开了一个word，前台也是word——>快捷键将自动最小化word，将其隐藏；\n"
		"    场景4：已经打开了多个word，前台也是word——>快捷键将自动在多个word间切换；"
		"\n--------------\n"
		"下拉框找不到目标程序？\n"
		"    FingerGo 会自动扫描打开的程序供你选择，如果找不到，先打开它；\n"
		;
	ML.ABOUT = L"关于";
	ML.EXIT = L"退出";
	ML.WIN_WARN = L"警告";
	ML.WIN_TITLE = L"配置 FingerGo";
	ML.TIP_WINLIST = "为了方便你选择程序，FingerGo会自动搜索已经打开的窗口，如果找不到想要的程序窗口，请先打开目标程序窗口，然后再选择。";
	ML.WIN_HOTKEYCONFIG = _Font("配置快捷键：");
	ML.WIN_REFRESH = _Font("刷新");
	ML.WIN_ADD = _Font("添加");
	ML.WIN_SAVE = _Font("保存");
	ML.WIN_DELETE = _Font("删除");
	ML.WIN_NO_OPEN_WARN = _Font("没有检测到可供选择的窗口程序，如果想要为程序配置快捷键，请先打开该程序。");
	ML.WIN_KET_LABEL = _Font("按键：ALT+");
	ML.WIN_EXE_LABEL = _Font("绑定到：");
	ML.WARN_REPEAT = L"FingerGo: 发现重复按键，请检查按键: ";
	ML.WARN_CHECK = L"FingerGo: 请检查配置是否填写完成。";
	ML.WARN_BLANK = L"FingerGo: 未发现配置项，请配置。";
	ML.WARN_SAVE_SUCCESS = L"FingerGo: 保存成功。";
	ML.WIN_DONATE_TEA = _Font("如果 FingerGo 有帮到你，可以请我喝杯奶茶吗  (*/o＼*)");
	ML.WARN_KEY_OCCUPIED = L"FingerGo: 检测到热键被占用，未生效，请修改热键：[ALT+";
	ML.WARN_NO_EXE = L"FingerGo: 程序启动失败，请检查程序是否存在:\n";
	ML.INFO_AUTOBOOT_ON = L"设置开机自动启动FingerGo成功。";
	ML.INFO_AUTOBOOT_OFF = L"取消开机自动启动FingerGo成功。";
	ML.INFO_RUNNING= L"检测到 FingerGo 已经运行，无需重复启动，右下角托盘区可以看到程序图标。";
	ML.INFO_CONFIG_TIP = L"FingerGo 启动成功，你还没有配置任何快捷键，点击右下角托盘中的FingerGo图标，即可开始配置。";
	ML.INFO_RUNFAILED_REASON= L"\n------------\n常见原因：\n  1. 程序被移动 \n  2. 某些程序升级会导致程序目录变动\n解决：重新为该程序配置快捷键即可。【记得打开目标程序后再选择它】。";
}

void Lang::setEn() {
	ML.LNAGUAGE = English;
	ML.AUTO_BOOT = L"Autostart";
	ML.ABOUT_TXT = L"FingerGo\n--------------\nauthor: Neway\nEmail : lxwlxc@126.com";
	ML.CONFIG = L"Set";
	ML.HELP = L"Help";
	ML.HELP_DETAIL = L"";
	ML.ABOUT = L"About";
	ML.EXIT = L"Exit";
	ML.WIN_WARN = L"Warn";
	ML.WIN_TITLE = L"Set FingerGo";
	ML.TIP_WINLIST = "In order to facilitate you to select the program, FingerGo will automatically search for the opened windows. If the desired program window cannot be found, please open the target program window first, and then select it.";
	ML.WIN_HOTKEYCONFIG = "Set hotKey:";
	ML.WIN_REFRESH = "Refresh ";
	ML.WIN_ADD = "Add";
	ML.WIN_SAVE = "Save";
	ML.WIN_DELETE = "Delete";
	ML.WIN_NO_OPEN_WARN = "No alternative window program was detected. If you want to configure a shortcut key for the program, open the program first.";
	ML.WIN_KET_LABEL = "Key: ALT+";
	ML.WIN_EXE_LABEL = "Bind to:";
	ML.WARN_REPEAT = L"FingerGo: Please check, duplicate hotkey found: ";
	ML.WARN_CHECK = L"FingerGo: Please check whether the configuration is completed.";
	ML.WARN_BLANK = L"FingerGo: No configuration item is found. Configure it.";
	ML.WARN_SAVE_SUCCESS = L"FingerGo: Save successfully, try it.";
	ML.WIN_DONATE_TEA = "If the FingerGo helps you, can you buy me a cup of milk tea? : )";
	ML.WARN_KEY_OCCUPIED = L"FingerGo: The hot key is occupied and does not take effect. Please modify the hot key: [ALT+";
	ML.WARN_NO_EXE = L"FingerGo: Failed to run the program, please check if the file exists:\n";
	ML.INFO_AUTOBOOT_ON = L"Succeeded in setting to automatic start FingerGo upon startup.";
	ML.INFO_AUTOBOOT_OFF = L"Succeeded in cancelling to automatic start FingerGo upon startup.";
	ML.INFO_RUNNING = L"It is detected that FingerGo has been running, and there is no need to start it repeatedly. The program icon can be seen in the tray area in the lower right corner.";
	ML.INFO_CONFIG_TIP = L"FingerGo launches successfully, you haven't configured any shortcuts yet, click the FingerGo icon in the bottom right tray to start configuring.";
	ML.INFO_RUNFAILED_REASON = L"\n------------\nCommon causes: \n  1. Program moved. \n  2. Program directory changed after program upgrade. \nSolution: reconfigure shortcut keys for the program. Remember to open the target program before selecting it";

}
const char* _Font(const char* text) {
	fontSet.insert(text);
	return text;
}