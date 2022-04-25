#include "pch.h"

#define SendImageCall1Offset (0x6740A1C0 - 0x67370000)
#define SendImageCall2Offset (0x67A71DC0 - 0x67370000)
#define SendImageCall3Offset (0x6782E160 - 0x67370000)
#define DeleteSendImageCacheCallOffset (0x67404200 - 0x67370000)

void __stdcall SendImage(wchar_t* receiver, wchar_t* ImagePath) {
	DWORD WeChatWinBase = GetWeChatWinBase();
	DWORD SendImageCall1 = WeChatWinBase + SendImageCall1Offset;
	DWORD SendImageCall2 = WeChatWinBase + SendImageCall2Offset;
	DWORD SendImageCall3 = WeChatWinBase + SendImageCall3Offset;
	DWORD DeleteSendImageCacheCall = WeChatWinBase + DeleteSendImageCacheCallOffset;
	char nullbuffer[0x50] = { 0 };
	char buffer[0x3B0] = { 0 };
	WxBaseStruct pReceiver(receiver);
	WxBaseStruct pImagePath(ImagePath);
	WxString nullStruct = { 0 };
	
	__asm {
		pushad;
		call SendImageCall1;
		sub esp, 0x14;
		mov ebx, eax;
		lea eax, nullStruct;
		mov ecx, esp;
		lea edi, pImagePath;
		push eax;
		call SendImageCall2;
		mov ecx, ebx;
		lea eax, pReceiver;
		push edi;
		push eax;
		lea eax, buffer;
		push eax;
		call SendImageCall3;
		lea ecx, buffer;
		call DeleteSendImageCacheCall;
		popad;
	}
}