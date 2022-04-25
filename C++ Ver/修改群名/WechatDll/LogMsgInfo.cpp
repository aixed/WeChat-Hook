#include "pch.h"

#define HookLogMsgInfoAddrOffset 0x103408A4 - 0x0FC40000
#define HookLogMsgInfoNextCallOffset 0x11586DFC - 0x0FC40000
#define HookLogMsgJmpBackOffset 0x103408A9 - 0x0FC40000

DWORD HookLogMsgInfoAddr = GetWeChatWinBase() + HookLogMsgInfoAddrOffset;
DWORD NextCallAddr = GetWeChatWinBase() + HookLogMsgInfoNextCallOffset;
DWORD JmpBackAddr = GetWeChatWinBase() + HookLogMsgJmpBackOffset;

BOOL LogMsgHooked = false;
char LogOldAsmCode[5] = { 0 };

VOID PrintMsg(DWORD msg) {
	if (!msg)
		return;
	string message = (char*)msg;
	cout << message;
	return;
}

__declspec(naked) void doprintmsg(){
	__asm {
		pushad;
		pushfd;
		push eax;
		call PrintMsg;
		add esp, 0x4;
		popfd;
		popad;
		call NextCallAddr;
		jmp JmpBackAddr;
	}
}

VOID HookLogMsgInfo() {
	if (LogMsgHooked)
		return;
	HookAnyAddress(HookLogMsgInfoAddr,(LPVOID)doprintmsg, LogOldAsmCode);
	LogMsgHooked = true;
}

VOID UnHookLogMsgInfo() {
	if (!LogMsgHooked)
		return;
	UnHookAnyAddress(HookLogMsgInfoAddr, LogOldAsmCode);
	LogMsgHooked = false;
}