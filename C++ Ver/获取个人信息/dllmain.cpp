// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include <Windows.h>
#include <stdio.h>
#include "resource.h"
#include "GetSelfInfo.h"

INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT   uMsg, WPARAM wParam, LPARAM lParam);

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		DialogBox(hModule, MAKEINTRESOURCE(MAIN), NULL, &DialogProc);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT   uMsg, WPARAM wParam, LPARAM lParam)
{

	switch (uMsg)
	{
	case WM_INITDIALOG:
		break;
	case WM_COMMAND:
		if (wParam == READ_DATA)
		{
			

			ReadWeChatData(hwndDlg);
		}
		if (wParam == WRITE_DATA)
		{

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