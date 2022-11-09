// DLL_Inject.cpp : 定义应用程序的入口点。
//

#include "pch.h"
#include "framework.h"
#include "DLL_Inject.h"
#include <Windows.h>
#include "resource.h"
#include <stdio.h>
#include <direct.h>
#include <TlHelp32.h>


#define WECHAT_PROCESS_NAME "WeChat.exe"

INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT   uMsg, WPARAM wParam, LPARAM lParam);
VOID InjectDll();
VOID UnInject();
char* GetCurrentDirectry();
DWORD GetDllModuleBase(DWORD dwPid, LPCSTR moudleName);



//函数开始
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
   
    DialogBox(hInstance, MAKEINTRESOURCE(ID_MAIN),NULL,&DialogProc);

    return 0;
}


//所有的消息处理函数
INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT   uMsg, WPARAM wParam, LPARAM lParam)
{

	switch (uMsg)
	{
	case WM_INITDIALOG:
		//MessageBox(NULL,"初次加载","请多关照",0);
		break;
	//按钮点击事件 处理
	case WM_COMMAND:
		if (wParam == INJECT_DLL)
		{
			InjectDll();
		}
		if (wParam == UN_DLL)
		{
			UnInject();
		}
		break;
	case WM_CLOSE:
		EndDialog(hwndDlg, 0);
		break;
	default:
		break;
	}
	return FALSE;
}

//Setp1.通过进程名称查找进程PID
DWORD ProcessNameFindPID(LPCSTR processName)
{
	//wchar_t buffText[0x100] = { 0 };
	//创建进程快照

	HANDLE ProcessAll = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	PROCESSENTRY32 processInfo = { 0 };
	processInfo.dwSize = sizeof(PROCESSENTRY32);

	do
	{
		if (strcmp(processName, processInfo.szExeFile) == 0) {
			return processInfo.th32ProcessID;
		}
	} while (Process32Next(ProcessAll, &processInfo));

}

//获取当前路径
char* GetCurrentDirectry()
{
	char* dir = _getcwd(NULL, 0);
	return dir;
}


//Setp2.申请dll路径内存
VOID InjectDll()
{
	CHAR pathStr[0x128] = { 0 };
	char *buffer = NULL;
	//CHAR pathStr[0x128] = "C://Users//Administrator//Desktop//Wx_Project//DLL_Inject//Debug//MyDll.dll";



	//获取当前工作目录
	buffer = GetCurrentDirectry();

	sprintf_s(pathStr, "%s\\MyDll.dll", buffer);



	//查找目标进程PID
	DWORD PID = ProcessNameFindPID(WECHAT_PROCESS_NAME);
	if (PID == 0) {
		MessageBox(NULL, "未找到微信进程", "错误", MB_OK);
		return;
	}
	//打开进程
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
	if (NULL == hProcess) {
		MessageBox(NULL, "进程打开失败", "错误", MB_OK);
		return;
	}
	//申请内存
	//DWORD strSize = strlen(dllPath) * 2;
	//进程打开后我们把我们的dll路径存进去
	//首先申请一片内存用于储存dll路径
	LPVOID dllAddr = VirtualAllocEx(hProcess, NULL, sizeof(pathStr), MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (NULL == dllAddr) {
		MessageBox(NULL, "内存申请失败", "错误", MB_OK);
		return;
	}

	//写入dll路径到微信内存
	if (WriteProcessMemory(hProcess, dllAddr, pathStr, sizeof(pathStr), NULL) == 0) {
		MessageBox(NULL, "DLL路径写入失败", "错误", MB_OK);
		return;
	}
	CHAR test[0x100] = { 0 };
	sprintf_s(test,"写入的地址为：%p",dllAddr);
	OutputDebugString(test);

	//获取欲被执行的函数地址
	HMODULE hModule = GetModuleHandle("Kernel32.dll");
	LPVOID loadAddr = GetProcAddress(hModule, "LoadLibraryA");

	//LoadLibraryA("MyDll.dll");


	//创建远程线程
	HANDLE hRemote = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)loadAddr, dllAddr, 0, NULL);
	if (NULL == hRemote) {
		MessageBox(NULL, "远程执行失败", "错误", MB_OK);
		return;
	}

}



//卸载dll
VOID UnInject()
{
	//1.获取微信进程的PID
	DWORD dwPid = ProcessNameFindPID("WeChat.exe");

	//拿到要卸载模块的基地址
	DWORD dwBase = GetDllModuleBase(dwPid,"MyDll.dll");


	//2.打开微信进程
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
	if (hProcess == NULL)
	{
		MessageBoxA(0, "打开微信进程失败", "Tip", 0);
		return;
	}



	//5.获取FreeLibrary函数地址

	//5.1拿到kernel32的基地址
	HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");
	FARPROC pFreeLibrary = GetProcAddress(hKernel32, "FreeLibrary");
	if (pFreeLibrary == NULL)
	{
		MessageBoxA(0, "获取FreeLibrary函数地址失败", "Tip", 0);
		return;
	}


	//6.用`CreateRemoteThread`卸载dll 传入dll模块的句柄

	HANDLE hRemote = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pFreeLibrary, (LPVOID)dwBase, 0, NULL);
	if (hRemote == NULL)
	{
		MessageBoxA(0, "CreateRemoteThread失败", "Tip", 0);
		return;
	}

	//7.关闭句柄
	CloseHandle(hProcess);

	CloseHandle(hRemote);

}


//获取进程模块基址
DWORD GetDllModuleBase(DWORD dwPid, LPCSTR moudleName)
{
	//获取模块快照
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPid);


	//模块信息结构体
	MODULEENTRY32 ME32 = { 0 };
	ME32.dwSize = sizeof(MODULEENTRY32);


	if (Module32First(hSnap, &ME32))
	{
		do
		{
			if (strcmp(moudleName, ME32.szModule) == 0) {
				return DWORD(ME32.modBaseAddr);
			}

		} while (Module32Next(hSnap, &ME32));
	}

}