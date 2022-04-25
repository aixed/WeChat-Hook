#include "pch.h"

#define CheckFriendStatusCall1Offset 0x78861210 - 0x787A0000
#define CheckFriendStatusCall2Offset 0x03521CD0 - 0x02E20000
#define CheckFriendStatusCall3Offset 0x03521DC0 - 0x02E20000
#define CheckFriendStatusCall4Offset 0x0321FB90 - 0x02E20000

#define CheckFriendStatusParamOffset 0x0504F3BC - 0x02E20000

#define CheckFriendStatusHookOffset 0x5E0830B3 - 0x5DB60000
#define CheckFriendStatusNextCallOffset 0x5E083150 - 0x5DB60000
#define CheckFriendStatusHookJmpBackOffset 0x5E0830B8 - 0x5DB60000

DWORD CheckFriendStatusNextCallAddress = GetWeChatWinBase() + CheckFriendStatusNextCallOffset;
DWORD CheckFriendStatusHookJmpBackAddress = GetWeChatWinBase() + CheckFriendStatusHookJmpBackOffset;

char OldAsmCode[5] = { 0 };
BOOL CheckFriendStatusHooked = false;
DWORD LocalFriendStatus = 0x0;

struct FriendStatusParamStruct {
	DWORD fill0 = 0x0;
	DWORD fill1 = 0x0;
	DWORD fill2 = -0x1;
	DWORD fill3 = 0x0;
	DWORD fill4 = 0x0;
	DWORD fill5 = 0xF;
	char nullbuffer[0xC] = { 0 };
};

void dealVerifyUserResult(DWORD result) {
	if (result < 0xB0 || result > 0xB5)
		return;
	LocalFriendStatus = result;
}

__declspec(naked) void doHookVerifyUserResult() {
	__asm {
		pushfd;
		pushad;
		mov eax, [esi];
		push eax;
		call dealVerifyUserResult;
		add esp, 0x4;
		popad;
		popfd;
		call CheckFriendStatusNextCallAddress;
		jmp CheckFriendStatusHookJmpBackAddress;
	}
}

VOID HookFriendStatusCode(){
	if (CheckFriendStatusHooked)
		return;
	DWORD WeChatWinBase = GetWeChatWinBase();
	DWORD dwHookAddress = WeChatWinBase + CheckFriendStatusHookOffset;
	HookAnyAddress(dwHookAddress, doHookVerifyUserResult, OldAsmCode);
	CheckFriendStatusHooked = true;
}

VOID UnHookFriendStatusCode() {
	if (!CheckFriendStatusHooked)
		return;
	DWORD WeChatWinBase = GetWeChatWinBase();
	DWORD dwHookAddress = WeChatWinBase + CheckFriendStatusHookOffset;
	UnHookAnyAddress(dwHookAddress,OldAsmCode);
	CheckFriendStatusHooked = false;
}

VOID CheckFriendStatusInitRemote() {
	HookFriendStatusCode();
}

DWORD CheckFriendStatusRemote(LPVOID lparameter) {
	CheckFriendStatus((wchar_t*)lparameter);
	return LocalFriendStatus;
}

VOID CheckFriendStatusFinishRemote() {
	UnHookFriendStatusCode();
}

VOID __stdcall CheckFriendStatus(wchar_t* wxid) {
	LocalFriendStatus = 0x0;
	DWORD WeChatWinBase = GetWeChatWinBase();
	DWORD CheckFriendStatusCall1 = WeChatWinBase + CheckFriendStatusCall1Offset;
	DWORD CheckFriendStatusCall2 = WeChatWinBase + CheckFriendStatusCall2Offset;
	DWORD CheckFriendStatusCall3 = WeChatWinBase + CheckFriendStatusCall3Offset;
	DWORD CheckFriendStatusCall4 = WeChatWinBase + CheckFriendStatusCall4Offset;
	DWORD CheckFriendStatusParam = WeChatWinBase + CheckFriendStatusParamOffset;

	WxBaseStruct pwxid(wxid);
	FriendStatusParamStruct FriendStatusParam;

	char* swxid = new char[wcslen(wxid) + 1];
	ZeroMemory(swxid, wcslen(wxid) + 1);
	WideCharToMultiByte(CP_ACP, 0, wxid, -1, swxid, wcslen(wxid), NULL, NULL);
	pwxid.fill1 = (DWORD)swxid;
	pwxid.fill2 = wcslen(wxid);

	wchar_t* message = (WCHAR*)L"我是";

	__asm {
		pushad;
		pushfd;
		mov edi, 0x6;
		mov esi, 0x0;
		sub esp, 0x18;
		mov eax, esp;
		mov dword ptr[eax], 0x0;
		mov dword ptr[eax + 0x14], 0xF;
		mov dword ptr[eax + 0x10], 0x0;
		sub esp, 0x18;
		lea eax, FriendStatusParam;
		mov ecx, esp;
		push eax;
		call CheckFriendStatusCall1;
		push esi;
		push edi;
		mov edi, message;
		sub esp, 0x14;
		mov ecx, esp;
		push -0x1;
		mov eax, edi;
		push eax;
		call CheckFriendStatusCall2;
		// 这里改成0x2就是添加好友，0x1是请求好友状态
		push 0x1;
		lea eax, pwxid;
		sub esp, 0x14;
		mov ecx, esp;
		push eax;
		call CheckFriendStatusCall3;
		mov eax, [CheckFriendStatusParam];
		mov eax, [eax];
		mov ecx, eax;
		call CheckFriendStatusCall4;
		popfd;
		popad;
	}
	while (!LocalFriendStatus && CheckFriendStatusHooked) {
		Sleep(10);
	}
#ifdef _DEBUG
	printf("wxid:%ws,status:0x%02X\n", wxid,LocalFriendStatus);
#endif
	delete[] swxid;
	swxid = NULL;
	return;
}