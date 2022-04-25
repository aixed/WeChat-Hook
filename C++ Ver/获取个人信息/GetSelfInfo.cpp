#include "pch.h"
#include <Windows.h>
#include <stdio.h>
#include "resource.h"
#include "malloc.h"

/*
WeChatWin.dll

WeChatWin.dll+222EBB4		昵称		基址		英文昵称
[WeChatWin.dll+222EBB4]		昵称		基址		中文昵称
WeChatWin.dll+222EBE8		手机
WeChatWin.dll+222ECBC		省/自治区
WeChatWin.dll+222ECD4		城市
WeChatWin.dll+222EF30		国籍
WeChatWin.dll+222F058		登陆手机型号
WeChatWin.dll+222ED30		微信号			如果没设置则为空


[WeChatWin.dll+222EBB4+2E0]+0			头像URL		222EE94
[WeChatWin.dll+222EBB4+46C]+0			微信ID		222F020
[WeChatWin.dll+222EBB4+1C]+1			邮箱

*/


//消息结构体
struct wechatText
{
	wchar_t* pStr;
	int strLen;
	int iStrLen;
};

//wxid/UNICODE 结构体
struct wxString2
{
	wchar_t* pStr;
	int strLen;
	int strMaxLen;
	int fill;
	int	fill2;
};



DWORD 取微信模块基址()
{
	//获取模块句柄(基址)
	HMODULE winAddr = LoadLibrary("WeChatWin.dll");
	return (DWORD)winAddr;
}


wchar_t* UTF8ToUnicode(const char* str)
{
	int    textlen = 0;
	wchar_t* result;
	textlen = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	result = (wchar_t*)malloc((textlen + 1) * sizeof(wchar_t));
	memset(result, 0, (textlen + 1) * sizeof(wchar_t));
	MultiByteToWideChar(CP_UTF8, 0, str, -1, (LPWSTR)result, textlen);
	return    result;
}





//读取内存数据
VOID ReadWeChatData(HWND hwndDlg)
{
	//获取模块基址
	//HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, -1);

	//CreateRemoteThread((HANDLE)-1, NULL, 0, (LPTHREAD_START_ROUTINE)ReadData_Thread, hwndDlg, 0, NULL);
	//CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ReadData_Thread, hwndDlg, 0, NULL);

	//CloseHandle(hProcess);
	HMODULE hModule = GetModuleHandle("WeChatWin.dll");
	CHAR 模块句柄[0x50] = { 0 };
	sprintf_s(模块句柄, "[WeChatWin 句柄] 0x%08X", DWORD(hModule));
	OutputDebugStringA(模块句柄);


	//获取模块基址
	DWORD WeChatWin_DLL = 取微信模块基址();
	CHAR W_HD[0x50] = { 0 };
	sprintf_s(W_HD, "[模块基址] 0x%08X", WeChatWin_DLL);
	OutputDebugStringA(W_HD);


	DWORD 微信号地址 = WeChatWin_DLL + 0x222ED30;
	CHAR 微信号_HD[0x50] = { 0 };
	sprintf_s(微信号_HD, "[微信号地址] 0x%08X", 微信号地址);
	OutputDebugStringA(微信号_HD);


	
	//获取自己进程的句柄
	HANDLE hProcess = GetCurrentProcess();
	
	
	//装微信ID的变量
	CHAR wx_id[0x100] = { 0 };
	INT wx_id_Addr;
	DWORD pWx_id = WeChatWin_DLL + 0x222F020;
	//sprintf_s(wx_id,"s%", *((DWORD*)pWx_id));

	ReadProcessMemory(hProcess, (LPVOID)pWx_id, &wx_id_Addr, 0x4, NULL);
	ReadProcessMemory(hProcess, (LPVOID)wx_id_Addr, wx_id, 0x100, NULL);
	OutputDebugStringA(wx_id);
	SetDlgItemText(hwndDlg,WXID, wx_id);
	


	//微信号
	CHAR Buffer_[30] = { 0 };
	//TCHAR Buffer_[30] = { 0 };
	ReadProcessMemory(hProcess, (LPVOID)微信号地址, Buffer_, 30, NULL);
	OutputDebugStringA(Buffer_);
	SetDlgItemText(hwndDlg, WXACCOUNT, Buffer_);

	
	//微信头像URL
	CHAR headpic[0x100] = { 0 };
	DWORD pPic = WeChatWin_DLL + 0x222EE94;
	INT pPic_Addr;
	ReadProcessMemory(hProcess, (LPVOID)pPic, &pPic_Addr, 0x4, NULL);
	ReadProcessMemory(hProcess, (LPVOID)pPic_Addr, headpic, 0x100, NULL);
	SetDlgItemText(hwndDlg, HEAD_PIC, headpic);
	OutputDebugStringA(headpic);


	//CloseHandle(hProcess);


}


