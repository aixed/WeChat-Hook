#include "pch.h"
#include <vector> 
using namespace std;
#define LeftTreeOffset 0x222F3BC


struct WxFriendStructW {
	DWORD wxIdAddr;
	DWORD wxNumberAddr;
	DWORD wxNickNameAddr;
	DWORD wxRemarkAddr;
	WxFriendStructW(DWORD wxIdAddr, DWORD wxNumberAddr, DWORD wxNickNameAddr, DWORD wxRemarkAddr) {
		this->wxIdAddr = wxIdAddr;
		this->wxNumberAddr = wxNumberAddr;
		this->wxNickNameAddr = wxNickNameAddr;
		this->wxRemarkAddr = wxRemarkAddr;
	}
};

vector<WxFriendStructW> WxFriendList;

int GetFriendListInit() {
	GetFriendList();
#ifdef _DEBUG
	cout << WxFriendList.size() << endl;
#endif
	return WxFriendList.size();
}

DWORD GetFriendListRemote() {
	if (WxFriendList.size() == 0)
		return 0;
#ifdef _DEBUG
	printf("0x%08X\n", (DWORD)&WxFriendList[0]);
	printf("0x%08X\n", (DWORD)&(WxFriendList[0].wxIdAddr));
#endif
	return (DWORD)&WxFriendList[0].wxIdAddr;
}

void GetFriendListFinish() {
	WxFriendList.clear();
	cout << WxFriendList.size() << endl;
}

void __stdcall GetFriendList() {
#ifdef _DEBUG
	wcout.imbue(locale("chs"));
#endif
	DWORD WeChatWinBase = GetWeChatWinBase();
	DWORD LeftTreeAddr = 0;
	DWORD RightTreeAddr = 0;
	DWORD LeftTreeHead = 0;
	DWORD baseAddr = WeChatWinBase + LeftTreeOffset;
	__asm {
		pushad;
		mov eax, dword ptr[baseAddr];
		mov eax, dword ptr[eax];
		mov eax, dword ptr[eax + 0x4C];
		mov ecx, dword ptr[eax];
		mov LeftTreeAddr, ecx;
		mov LeftTreeHead, eax;
		mov ecx, dword ptr[eax + 0x4];
		mov RightTreeAddr, ecx;
		popad;
	}
	
	while (1) {
		DWORD wxIdAddr = 0;
		DWORD wxNumberAddr = 0;
		DWORD wxNickNameAddr = 0;
		DWORD wxRemarkAddr = 0;

		__asm {
			pushad;
			mov eax, dword ptr [LeftTreeAddr];
			mov ecx, eax;
			add ecx, 0x30;
			mov wxIdAddr, ecx;
			mov ecx, eax;
			add ecx, 0x44;
			mov wxNumberAddr, ecx;
			mov ecx, eax;
			add ecx, 0x8C;
			mov wxNickNameAddr, ecx;
			mov ecx, eax;
			add ecx, 0x78;
			mov wxRemarkAddr, ecx;
			mov ecx, dword ptr [eax];
			mov LeftTreeAddr, ecx;
			popad;
		}
		WxFriendStructW p(wxIdAddr, wxNumberAddr, wxNickNameAddr, wxRemarkAddr);
		WxFriendList.push_back(p);
#ifdef _DEBUG
		wcout << (wchar_t*)(*(DWORD*)p.wxIdAddr) << endl;
#endif
		if (LeftTreeAddr == LeftTreeHead) {
			break;
		}
	}
}