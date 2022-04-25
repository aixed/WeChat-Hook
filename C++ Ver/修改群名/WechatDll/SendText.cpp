#include "pch.h"

#define SendTextCallOffset 0x6782E7B0 - 0x67370000
#define DeleteTextCacheCallOffset 0x67404200 - 0x67370000

void __stdcall SendText(wchar_t* wsWxId, wchar_t* wsTextMsg) {
    WxBaseStruct wxWxid(wsWxId);
    WxBaseStruct wxTextMsg(wsTextMsg);
    wchar_t** pWxmsg = &wxTextMsg.buffer;
    char buffer[0x3B0] = { 0 };

    WxString wxNull = { 0 };
    DWORD dllBaseAddress = GetWeChatWinBase();
    DWORD callAddress = dllBaseAddress + SendTextCallOffset;
    DWORD DeleteTextCacheCall = dllBaseAddress + DeleteTextCacheCallOffset;

    __asm {
        pushad;
        lea eax, wxNull;
        push 0x1;
        push eax;
        mov edi, pWxmsg;
        push edi;
        lea edx, wxWxid;
        lea ecx, buffer;
        call callAddress;
        add esp, 0xC;
        lea ecx, buffer;
        call DeleteTextCacheCall;
        popad;
    }
}