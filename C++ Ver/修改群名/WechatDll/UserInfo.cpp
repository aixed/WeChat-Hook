#include "pch.h"
#include <typeinfo>
#include <string>
#include <vector>

#define GetUserInfoCall0Offset 0x6740A000 - 0x67370000
#define GetUserInfoCall1Offset 0x679C9840 - 0x67370000
#define GetUserInfoCall2Offset 0x67A71DC0 - 0x67370000
#define GetUserInfoCall3Offset 0x677724A0 - 0x67370000

#define DeleteUserInfoCacheCall1Offset 0x67775990 - 0x67370000
#define DeleteUserInfoCacheCall2Offset 0x679CA340 - 0x67370000

struct GetUserInfoStruct {
	DWORD message;
	DWORD length;
};

wstring wUserInfo = L"";
GetUserInfoStruct ret = { 0 };

VOID WxUserInfo(DWORD address) {
	vector<DWORD> InfoType{
		address + 0x10,
		address + 0x24,
		address + 0x38,
		address + 0x58,
		address + 0x6C,
		address + 0xFC,
		address + 0x110,
		address + 0x19C,
		address + 0x1B0,
		address + 0x1C4,
		address + 0x1D8,
		address + 0x27C
	};
	vector<wchar_t*> InfoTypeName{
		(WCHAR*)L"\"wxId\"",
		(WCHAR*)L"\"wxNumber\"",
		(WCHAR*)L"\"wxV3\"",
		(WCHAR*)L"\"wxRemark\"",
		(WCHAR*)L"\"wxNickName\"",
		(WCHAR*)L"\"wxBigAvatar\"",
		(WCHAR*)L"\"wxSmallAvatar\"",
		(WCHAR*)L"\"wxSignature\"",
		(WCHAR*)L"\"wxNation\"",
		(WCHAR*)L"\"wxProvince\"",
		(WCHAR*)L"\"wxCity\"",
		(WCHAR*)L"\"wxBackground\"",
	};
	wUserInfo += L"{";
	for (unsigned int i = 0; i < InfoType.size(); i++) {
		wchar_t* wstemp = ((*((DWORD*)InfoType[i])) != 0) ? (WCHAR*)(*((LPVOID*)InfoType[i])) : (WCHAR*)L"null";
		wstring wsrtemp = wreplace(wstemp,L'\"',L"\\\"");
		wUserInfo = wUserInfo + InfoTypeName[i] + L":\"" + wsrtemp + L"\"";
		if (i != InfoType.size() - 1) {
			wUserInfo += L",";
		}
	}
	wUserInfo += L"}";
#ifdef _DEBUG
	wcout.imbue(locale("chs"));
	wcout << wUserInfo.c_str() << endl;
#endif
}


DWORD GetWxUserInfoRemote(LPVOID lparamter) {
	wchar_t* userwxid = (wchar_t*)lparamter;
	
	if (!GetUserInfoByWxId(userwxid)) {
		return 0;
	}
	ret.message = (DWORD)wUserInfo.c_str();
	ret.length = (DWORD)wUserInfo.length();
	return (DWORD)&ret;
}

VOID DeleteUserInfoCacheRemote() {
	if (ret.length) {
		ZeroMemory((wchar_t*)ret.message, ret.length * 2 + 2);
		ret.length = 0;
		wUserInfo = L"";
	}
}

BOOL __stdcall GetUserInfoByWxId(wchar_t* wxid) {
	DWORD WeChatWinBase = GetWeChatWinBase();
	DWORD WxGetUserInfoCall0 = WeChatWinBase + GetUserInfoCall0Offset;
	DWORD WxGetUserInfoCall1 = WeChatWinBase + GetUserInfoCall1Offset;
	DWORD WxGetUserInfoCall2 = WeChatWinBase + GetUserInfoCall2Offset;
	DWORD WxGetUserInfoCall3 = WeChatWinBase + GetUserInfoCall3Offset;
	DWORD DeleteUserInofCacheCall1 = WeChatWinBase + DeleteUserInfoCacheCall1Offset;
	DWORD DeleteUserInofCacheCall2 = WeChatWinBase + DeleteUserInfoCacheCall2Offset;
	char buffer[0x3FC] = { 0 };
	WxBaseStruct pWxid(wxid);
	DWORD address = 0;
	DWORD isSuccess = 0;
	__asm
	{
		pushad;
		call WxGetUserInfoCall0;
		mov edi, eax;
		lea ecx, buffer;
		call WxGetUserInfoCall1;
		lea eax, buffer;
		mov address, eax;
		push eax;
		sub esp, 0x14;
		mov ecx, esp;
		lea esi, pWxid;
		push esi;
		call WxGetUserInfoCall2;
		mov ecx, edi;
		call WxGetUserInfoCall3;
		mov isSuccess, eax;
		popad;
	}
	if(isSuccess)
		WxUserInfo(address);
	__asm {
		pushad;
		lea eax, buffer;
		push eax;
		call DeleteUserInofCacheCall1;
		lea ecx, buffer;
		mov esi, eax;
		call DeleteUserInofCacheCall2;
		popad;
	}
	return isSuccess;
}