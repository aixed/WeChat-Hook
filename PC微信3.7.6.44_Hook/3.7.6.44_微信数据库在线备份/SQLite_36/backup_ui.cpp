#include "pch.h"
#include "backup_ui.h"
#include "pch.h"
#include "offsets.h"
#include "wechat.h"
#include <Windows.h>
#include "resource.h"
#include <CommCtrl.h>
#include "utility.h"
#include "backup.h"

string columns;
VOID ShowUI(HMODULE hModule)
{
	//获取WeChatWin.dll的基址
	while (baseAddress == 0)
	{
		baseAddress = (DWORD)GetModuleHandle(TEXT("WeChatWin.dll"));
		Sleep(100);
	}

	if (CheckVersion() == FALSE)
	{
		string message;
		message.append("微信版本不匹配，请使用");
		message.append(wxVersoin);
		message.append("版本！");

		MessageBoxA(NULL, message.c_str(), "错误", MB_OK);
		WechatUnhook();
		return;
	}

	//在登录微信前注入
	int* loginStatus = (int*)(baseAddress + offset_wechat_login);
	if (*loginStatus != 0)
	{
		if (IDOK == MessageBox(NULL, TEXT("微信已经登录，该功能无法使用！\n是否重新启动微信？"), TEXT("错误"), MB_OKCANCEL | MB_ICONERROR))
		{
			WechatReboot();
		}
		WechatUnhook();
		return;
	}

	//启动窗口
	DialogBox(hModule, MAKEINTRESOURCE(IDD_MAIN), NULL, &DialogProcess);
}

INT_PTR CALLBACK DialogProcess(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	hWinDlg = hwndDlg;
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		//DLL加载后，就启动Inline HOOK
		WechatHook();

		HWND hProgress = GetDlgItem(hWinDlg, IDC_PROGRESS);
		//设置进度条范围
		SendMessageA(hProgress, PBM_SETRANGE, NULL, MAKELPARAM(0, 100));
		//设置进度条进度
		SendMessageA(hProgress, PBM_SETPOS, 0, 0);
		UpdateWindow(hProgress);

		HWND hStatic = GetDlgItem(hWinDlg, IDC_STATIC_PROGRESS);
		//设置文本内容
		SendMessageA(hStatic, WM_SETTEXT, NULL, (LPARAM)("00.00%"));
		UpdateWindow(hStatic);

		SetSQLText("PRAGMA database_list");
		break;
	}
	case WM_CLOSE:
	{
		//恢复HOOK掉的代码...省略unhook
		//释放在内存中创建的函数(lpAddressBackupDB)...省略  
		//从内存中卸载本dll
		WechatUnhook();
		EndDialog(hwndDlg, 0);
		break;
	}
	case WM_COMMAND:
	{
		//执行选择
		if (LOWORD(wParam) == IDC_COMBO_DB)
		{
			if (HIWORD(wParam) == CBN_SELCHANGE)
			{
				//获取数据库句柄
				int index = SendMessageA((HWND)lParam, CB_GETCURSEL, NULL, 0);
				char buf[MAX_PATH] = { 0 };
				SendMessageA((HWND)lParam, CB_GETLBTEXT, index, (LPARAM)buf);

				//清空文本框
				HWND edit = GetDlgItem(hWinDlg, IDC_EDIT_LOG);
				SendMessageA(edit, WM_SETTEXT, NULL, NULL);

				//添加日志
				string text = "在线备份的数据库：\r\n";
				text.append(buf);
				text.append("\r\n");
				AddLogUI(text);

				//获取查询的数据库句柄
				string dbName(buf);
				char hexString[12] = { 0 };
				for (auto& db : wx_db_list)
				{
					string dbNameInList(db.name);
					if (dbNameInList == dbName)
					{
						sprintf_s(hexString, "0x%08X", db.handle);
						text = "数据库句柄:\r\n";
						text.append(hexString);
						text.append("\r\n");
						AddLogUI(text);

						//设置句柄文本框
						SetHandleText(hexString);
						break;
					}
				}
			}
			break;
		}

		//刷新ComboBox
		if (wParam == IDC_BUTTON_REFRESH)
		{
			RefreshComboBox();
			break;
		}

		//执行备份
		if (wParam == IDC_BUTTON_SQLRUN)
		{
			BackupButtonClick();
			break;
		}

		//重启微信
		if (wParam == IDC_BUTTON_WX_REBOOT)
		{
			WechatReboot();
			break;
		}

		//数据库查询
		if (wParam == IDC_BUTTON_SQL)
		{
			QueryButtonClick();
			break;
		}

		break;
	}
	default:
		break;
	}
	return FALSE;
}

/// <summary>
/// 为数据库Handle文本框设置内容
/// </summary>
/// <param name="text"></param>
VOID SetHandleText(std::string text)
{
	HWND edit = GetDlgItem(hWinDlg, IDC_EDIT_SQL_HANDLE);
	SendMessageA(edit, WM_SETTEXT, NULL, (LPARAM)(text.c_str()));
}

VOID SetSQLText(std::string text)
{
	HWND edit = GetDlgItem(hWinDlg, IDC_EDIT_SQL);
	SendMessageA(edit, WM_SETTEXT, NULL, (LPARAM)(text.c_str()));
}

VOID AddLogUI(std::string text)
{
	HWND edit = GetDlgItem(hWinDlg, IDC_EDIT_LOG);

	//获取当前文本框的字符数量
	int count = SendMessageA(edit, WM_GETTEXTLENGTH, NULL, NULL);

	//获取当前文本框的内容
	char* oldChars = new char[count + 1]{ 0 };
	SendMessageA(edit, WM_GETTEXT, (WPARAM)(count + 1), (LPARAM)oldChars);
	string oldText(oldChars);
	delete[] oldChars;

	//添加字符
	oldText.append(text);
	SendMessageA(edit, WM_SETTEXT, NULL, (LPARAM)(oldText.c_str()));
}

VOID RefreshComboBox()
{
	//禁止 刷新按钮
	HWND hRefresh = GetDlgItem(hWinDlg, IDC_BUTTON_REFRESH);
	EnableWindow(hRefresh, FALSE);

	//把“数据库句柄-->数据库名”的链表保存到CombBox中
	HWND combo1 = GetDlgItem(hWinDlg, IDC_COMBO_DB);

	//删除全部内容
	while (SendMessage(combo1, CB_DELETESTRING, 0, 0) > 0) {}

	//添加内容
	for (auto& db : wx_db_list)
	{
		SendMessageA(combo1, CB_ADDSTRING, NULL, (LPARAM)(db.name));
	}
}
VOID QueryButtonClick()
{
	//获取数据库句柄
	HWND edit_handle = GetDlgItem(hWinDlg, IDC_EDIT_SQL_HANDLE);
	char buf[20] = { 0 };
	GetWindowTextA(edit_handle, buf, GetWindowTextLength(edit_handle) + 1);
	if (std::string(buf) == "")
	{
		AddLogUI("请先填写数据库句柄！\r\n");
		return;
	}
	else
	{
		string message = "数据库句柄：\t";
		message.append(buf);
		message.append("\r\n");
		AddLogUI(message);
	}

	//获取SQL语句
	HWND edit_sql = GetDlgItem(hWinDlg, IDC_EDIT_SQL);
	DWORD iLength = GetWindowTextLength(edit_sql);
	char* zBuffer;
	if (iLength != 0)
	{
		zBuffer = (char*)malloc(iLength * 2);
		GetWindowTextA(edit_sql, zBuffer, GetWindowTextLength(edit_sql) + 1);

		string message = "SQL语句：\t";
		message.append(zBuffer);
		message.append("\r\n");
		AddLogUI(message);
	}
	else
	{
		AddLogUI("请先填写要查询的SQL语句！\r\n");
		return;
	}

	// 执行SQL查询
	columns = "";
	char* errMsg = NULL;
	RunSQLByHandle(buf, zBuffer, QueryCallback, NULL, &errMsg);
}

VOID BackupButtonClick()
{
	//清除文本框内容
	HWND edit = GetDlgItem(hWinDlg, IDC_EDIT_LOG);
	SendMessageA(edit, WM_SETTEXT, NULL, NULL);

	//获取数据库句柄
	HWND combo1 = GetDlgItem(hWinDlg, IDC_COMBO_DB);
	int index = SendMessageA(combo1, CB_GETCURSEL, NULL, 0);
	char buf[MAX_PATH] = { 0 };
	SendMessageA(combo1, CB_GETLBTEXT, index, (LPARAM)buf);

	if (std::string(buf) == "")
	{
		AddLogUI("请先选择要在线备份的数据库！\r\n");
		return;
	}

	//添加日志
	std::string text = "数据库：\r\n";
	text.append(buf);
	text.append("\r\n");
	AddLogUI(text);

	//获取查询的数据库句柄
	std::string dbName(buf);
	char hexString[12] = { 0 };
	for (auto& db : wx_db_list)
	{
		std::string dbNameInList(db.name);
		if (dbNameInList == dbName)
		{
			selectedDbHander = db.handle;
			break;
		}
	}

	if (selectedDbHander == 0)
	{
		AddLogUI("请先选择要在线备份的数据库！\r\n");
		return;
	}

	//获取默认文件名
	TCHAR driver[_MAX_DRIVE] = { 0 };
	TCHAR dir[_MAX_DIR] = { 0 };
	TCHAR fname[_MAX_FNAME] = { 0 };
	TCHAR ext[_MAX_EXT] = { 0 };
	TCHAR szFile[MAX_PATH] = { 0 };
	_wsplitpath_s(StringToWString(dbName).c_str(), driver, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);
	wstring fileName(fname);
	fileName.append(ext);
	wmemcpy(szFile, fileName.c_str(), fileName.length());

	//程序当前目录
	TCHAR szCurDir[MAX_PATH];
	GetCurrentDirectory(sizeof(szCurDir), szCurDir);

	// 公共对话框结构。   
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = GetForegroundWindow();
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"SQL(*.db)\0*.db\0All(*.*)\0*.*\0\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = szCurDir;
	ofn.lpstrDefExt = L"db";
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_CREATEPROMPT | OFN_EXTENSIONDIFFERENT;

	// 显示打开选择文件对话框。
	if (GetSaveFileName(&ofn))
	{
		wstring dumpFile(szFile);
		backupFileName2 = WStringToString(dumpFile);
		string text = "备份的数据库:\r\n";
		text.append(backupFileName2);
		text.append("\r\n");
		AddLogUI(text);

		//以线程的方式启动
		//这样界面不会卡死
		HANDLE hANDLE = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)BackupSQLiteDB, NULL, NULL, 0);
		if (hANDLE != 0)
		{
			CloseHandle(hANDLE);
		}
	}
}

VOID SetRefreshButtonEnable()
{
	HWND hRefresh = GetDlgItem(hWinDlg, IDC_BUTTON_REFRESH);
	EnableWindow(hRefresh, TRUE);
}

VOID UpdateProgressBar(int pos, int all)
{
	HWND hProgress = GetDlgItem(hWinDlg, IDC_PROGRESS);
	//设置进度条范围
	//SendMessageA(hProgress, PBM_SETRANGE, NULL, MAKELPARAM(0, 100));
	//设置进度条进度
	SendMessageA(hProgress, PBM_SETPOS, (all - pos) * 100 / all, 0);
	UpdateWindow(hProgress);

	//设置文本内容
	HWND hStatic = GetDlgItem(hWinDlg, IDC_STATIC_PROGRESS);
	char dataPercent[7] = { 0 };
	sprintf_s(dataPercent, 7, "%.2f", (100.0f * (all - pos)) / all);
	string posStr(dataPercent);
	posStr.append("%");
	SendMessageA(hStatic, WM_SETTEXT, NULL, (LPARAM)(posStr.c_str()));
	UpdateWindow(hStatic);
}

int __cdecl QueryCallback(void* para, int nColumn, char** colValue, char** colName)
{
	if (columns == "")
	{
		for (int i = 0; i < nColumn; i++)
		{
			//指针和数组的两种写法
			//printf("%s :%s\n", *(colName + i), colValue[i]);
			columns.append(colName[i]);
			if (i < nColumn - 1)
			{
				columns.append(",");
			}
		}
		columns.append("\r\n");
		//MessageBoxA(NULL, columns.c_str(),"",MB_OK);
		AddLogUI(columns);
	}

	string data;
	for (int i = 0; i < nColumn; i++)
	{
		//指针和数组的两种写法
		//printf("%s :%s\n", *(colName + i), colValue[i]);
		string temp;
		auto aa = colValue[i];
		if (aa == NULL)
		{
			temp = "";
		}
		else
		{
			temp = string(aa);
		}
		data.append(temp);
		if (i < nColumn - 1)
		{
			data.append(",");
		}
	}
	data.append("\r\n");
	//MessageBoxA(NULL, data.c_str(), "", MB_OK);
	AddLogUI(data);
	return 0;
}
