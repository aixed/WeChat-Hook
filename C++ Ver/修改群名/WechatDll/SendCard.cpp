#include "pch.h"

#define SendCardCallOffset 0x644FE7B0 - 0x64040000
#define DeleteCardCacheCallOffset 0x640D4200 - 0x64040000

struct SendCardStruct {
	DWORD receiver;
	DWORD sharedwxid;
	DWORD nickname;
};

VOID SendCardRemote(LPVOID lparameter) {
	SendCardStruct* scs = (SendCardStruct*)lparameter;
	wchar_t* receiver = (WCHAR*)scs->receiver;
	wchar_t* sharedwxid = (WCHAR*)scs->sharedwxid;
	wchar_t* nickname = (WCHAR*)scs->nickname;
	SendCard(receiver,sharedwxid,nickname);
}

BOOL __stdcall SendCard(wchar_t* receiver, wchar_t* sharedwxid, wchar_t* nickname) {
	DWORD WeChatWinBase = GetWeChatWinBase();
	DWORD SendCardCall = WeChatWinBase + SendCardCallOffset;
	DWORD DeleteCardCacheCall = WeChatWinBase + DeleteCardCacheCallOffset;
	wchar_t* xml = new wchar_t[0x2000];
	ZeroMemory(xml, 0x2000 * 2);
	swprintf_s(xml, 0x2000,L"<?xml version=\"1.0\"?><msg bigheadimgurl=\"\" smallheadimgurl=\"\" username=\"%ws\" nickname=\"%ws\" fullpy=\"?\" shortpy=\"\" alias=\"%ws\" imagestatus=\"3\" scene=\"17\" province=\"北京\" city=\"中国\" sign=\"\" sex=\"2\" certflag=\"0\" certinfo=\"\" brandIconUrl=\"\" brandHomeUrl=\"\" brandSubscriptConfigUrl= \"\" brandFlags=\"0\" regionCode=\"CN_BeiJing_BeiJing\" />", 
		sharedwxid, nickname, sharedwxid);
	WxBaseStruct pReceiver(receiver);
	WxBaseStruct pXml(xml);
	char buffer[0x2D0] = { 0 };
	DWORD isSuccess = 0x1;

	__asm {
		pushad;
		push 0x2A;
		lea eax, pXml;
		lea edx, pReceiver;
		push 0x0;
		push eax;
		lea ecx, buffer;
		call SendCardCall;
		add esp, 0xC;
		lea ecx, buffer;
		call DeleteCardCacheCall;
		mov isSuccess, eax;
		popad;
	}
	delete[] xml;
	xml = NULL;
	return isSuccess;
}