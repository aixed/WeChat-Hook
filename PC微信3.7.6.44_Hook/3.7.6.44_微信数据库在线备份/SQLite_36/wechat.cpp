#include "pch.h"
#include <Windows.h>
#include <string>
#include <list>
#include "offsets.h"
#include "wechat.h"
#include <strstream>
#include "backup_ui.h"

#include <sstream>
#include <iostream>
#include <iomanip>

std::list<DB_HANDLE_NAME> wx_db_list;
const std::string wxVersoin = "3.7.6.44";
DWORD hookAddress = 0;
DWORD jumpBackAddress = 0;
DWORD index = 0;


BOOL CheckVersion()
{
	WCHAR VersionFilePath[MAX_PATH];
	if (GetModuleFileName((HMODULE)baseAddress, VersionFilePath, MAX_PATH) == 0)
	{
		return FALSE;
	}

	string asVer = "";
	VS_FIXEDFILEINFO* pVsInfo;
	unsigned int iFileInfoSize = sizeof(VS_FIXEDFILEINFO);
	int iVerInfoSize = GetFileVersionInfoSize(VersionFilePath, NULL);
	if (iVerInfoSize != 0) {
		char* pBuf = new char[iVerInfoSize];
		if (GetFileVersionInfo(VersionFilePath, 0, iVerInfoSize, pBuf)) {
			if (VerQueryValue(pBuf, TEXT("\\"), (void**)&pVsInfo, &iFileInfoSize)) {
				//主版本3.7.6.44
				//2
				int s_major_ver = (pVsInfo->dwFileVersionMS >> 16) & 0x0000FFFF;
				//6
				int s_minor_ver = pVsInfo->dwFileVersionMS & 0x0000FFFF;
				//7
				int s_build_num = (pVsInfo->dwFileVersionLS >> 16) & 0x0000FFFF;
				//57
				int s_revision_num = pVsInfo->dwFileVersionLS & 0x0000FFFF;

				//把版本变成字符串
				strstream wxVer;
				wxVer << s_major_ver << "." << s_minor_ver << "." << s_build_num << "." << s_revision_num;
				wxVer >> asVer;
			}
		}
		delete[] pBuf;
	}

	//版本匹配
	if (asVer == wxVersoin)
	{
		return TRUE;
	}

	//版本不匹配
	return FALSE;
}

VOID WechatReboot()
{
	//获取微信程序路径
	TCHAR szAppName[MAX_PATH];
	GetModuleFileName(NULL, szAppName, MAX_PATH);

	//启动新进程
	STARTUPINFO StartInfo;
	ZeroMemory(&StartInfo, sizeof(StartInfo));
	StartInfo.cb = sizeof(StartInfo);

	PROCESS_INFORMATION procStruct;
	ZeroMemory(&procStruct, sizeof(procStruct));
	StartInfo.cb = sizeof(STARTUPINFO);

	if (CreateProcess((LPCTSTR)szAppName, NULL, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &StartInfo, &procStruct))
	{
		CloseHandle(procStruct.hProcess);
		CloseHandle(procStruct.hThread);
	}
	//终止当前进程
	TerminateProcess(GetCurrentProcess(), 0);
}

VOID WechatHook()
{
	hookAddress = baseAddress + 0x1541A05;
	jumpBackAddress = hookAddress + 6;

	BYTE JmpCode[6] = { 0 };
	JmpCode[0] = 0xE9;
	JmpCode[6 - 1] = 0x90;

	//新跳转指令中的数据=跳转的地址-原地址（HOOK的地址）-跳转指令的长度
	*(DWORD*)&JmpCode[1] = (DWORD)WechatHookJump - hookAddress - 5;
	WriteProcessMemory(GetCurrentProcess(), (LPVOID)hookAddress, JmpCode, 6, 0);
}

VOID WechatUnhook()
{
	HMODULE hModule = NULL;

	//GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS 会增加引用计数
	//因此，后面还需执行一次FreeLibrary
	//直接使用本函数（UnInject）地址来定位本模块
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPWSTR)&WechatUnhook, &hModule);

	if (hModule != 0)
	{
		//减少一次引用计数
		FreeLibrary(hModule);
		//从内存中卸载
		FreeLibraryAndExitThread(hModule, 0);
	}
}

/// <summary>
/// //InlineHook完成后，程序在Hook点跳转到这里执行。这里必须是裸函数
/// </summary>
__declspec(naked) VOID WechatHookJump()
{
	//补充代码
	__asm
	{
		//保存寄存器
		pushad

		//参数2，数据库句柄
		lea eax, [EBP + 0x74]
		push [eax]
		//参数1，数据库路径地址，ASCII
		//lea eax, [EBP + 0x80]
		push EDX
		//调用我们的处理函数
		call WechatSQLiteHandle
		add esp, 8

		//恢复寄存器
		popad

		//补充被覆盖的代码
		mov esi, dword ptr ss : [esp + 0x1C]
		test esi, esi

		//跳回去接着执行
		jmp jumpBackAddress
	}
}

VOID WechatSQLiteHandle(int dbAddress, int dbHandle)
{
	DB_HANDLE_NAME db = { 0 };
	db.handle = dbHandle;
	_snprintf_s(db.name, MAX_PATH, "%s", (char*)dbAddress);
	wx_db_list.push_back(db);

	//使能 刷新按钮
	SetRefreshButtonEnable();

	index++;
	string log = to_string(index);
	log.append("\t");

	log.append("0x");
	std::stringstream ioss;
	ioss << std::setfill('0') << std::setw(8) << std::uppercase << std::hex << dbAddress;
	string temp;
	ioss >> temp;
	log.append(temp);

	log.append("\t");
	log.append(db.name);
	log.append("\r\n");
	AddLogUI(log);
}
