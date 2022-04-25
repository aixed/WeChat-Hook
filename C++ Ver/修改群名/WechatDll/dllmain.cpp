// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "resource.h"

DWORD WINAPI ThreadProc(
    _In_ HMODULE hModule
);

INT_PTR CALLBACK DialogProc(
    _In_ HWND   hwndDlg,
    _In_ UINT   uMsg,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam
);

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
#ifdef _DEBUG
        CreateConsole();
        printf("SendImage 0x%08X\n", (DWORD)SendImage);
        printf("SendText 0x%08X\n", (DWORD)SendText);
        printf("SendTextAt 0x%08X\n", (DWORD)SendTextAt);
        printf("SendFile 0x%08X\n", (DWORD)SendFile);
        printf("GetFriendList 0x%08X\n", (DWORD)GetFriendList);
        printf("GetUserInfoByWxId 0x%08X\n", (DWORD)GetUserInfoByWxId);
        printf("SendArticle 0x%08X\n", (DWORD)SendArticle);
        printf("SendCard 0x%08X\n", (DWORD)SendCard);
        printf("CheckFriendStatus 0x%08X\n", (DWORD)CheckFriendStatus);
        HookLogMsgInfo();
#endif
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProc, hModule, 0, NULL);
        break;
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH: {
        break;
    }
    }
    return TRUE;
}

DWORD WINAPI ThreadProc(
	_In_ HMODULE hModule
)
{
	DialogBox(hModule, MAKEINTRESOURCE(IDD_WeChatRot), NULL, &DialogProc);
	return TRUE;
}

INT_PTR CALLBACK DialogProc(
	_In_ HWND   hwndDlg,
	_In_ UINT   uMsg,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
)
{
    wchar_t groupId[0x100] = { 0 };
    wchar_t message[0x100] = { 0 };

    wchar_t setRoomWxid[0x200] = { 0 };
    wchar_t setMessage[0x200] = { 0 };
	switch (uMsg)
	{
	case WM_INITDIALOG:
        swprintf_s(setRoomWxid, L"24409622234@chatroom");
        swprintf_s(setMessage, L"牛逼的群");

        SetDlgItemText(hwndDlg, IDC_EDIT_ROOMID, setRoomWxid);
        SetDlgItemText(hwndDlg, IDC_EDIT_MESSAGE, setMessage);

		break;
	case WM_CLOSE:
		EndDialog(hwndDlg, 0);
		break;
	case WM_COMMAND:

        //修改群名
        if (wParam == IDC_BTN_CGN)
        {
            GetDlgItemText(hwndDlg, IDC_EDIT_ROOMID, groupId, std::size(groupId));
            GetDlgItemText(hwndDlg, IDC_EDIT_MESSAGE, message, std::size(message));
            
            CHAR TMP[0x50] = { 0 };
            sprintf_s(TMP, "0x%08X", &Edit_Group_Name);
            OutputDebugStringA(TMP);
            //MessageBoxA(hwndDlg, TMP, "[修改群名] 函数地址", NULL);

            Edit_Group_Name(groupId, message);
        }
		
		break;
	default:
		break;
	}
	return FALSE;
}