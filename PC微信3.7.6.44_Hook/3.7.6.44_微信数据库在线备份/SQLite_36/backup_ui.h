#pragma once
#include <string>
#include <Windows.h>
using namespace std;

extern HWND hWinDlg;
extern DWORD selectedDbHander;

VOID ShowUI(HMODULE hModule);
INT_PTR CALLBACK DialogProcess(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
/// <summary>
/// 回调函数，更新进度
/// </summary>
/// <param name="pos"></param>
/// <param name="all"></param>
VOID UpdateProgressBar(int pos, int all);
VOID AddLogUI(std::string text);
VOID RefreshComboBox();
VOID BackupButtonClick();
VOID SetRefreshButtonEnable();
VOID QueryButtonClick();
VOID SetHandleText(std::string text);
int QueryCallback(void*, int, char**, char**);
VOID SetSQLText(std::string text);
