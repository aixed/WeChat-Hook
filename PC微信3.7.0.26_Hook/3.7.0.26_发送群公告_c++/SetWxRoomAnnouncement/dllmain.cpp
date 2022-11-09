// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include <string>
#include <vector>
using namespace std;
#include <windows.h>

DWORD GetWeChatWinBase()
{
	return (DWORD)GetModuleHandle(TEXT("WeChatWin.dll"));
}

typedef struct RawWxStringW {
	int vptr;
	int iLen;
	int iMaxLen;
	int f1;
	int f2;
}RawWxStringW;

void ConvertToWxStringW(RawWxStringW* dst, std::wstring data)
{
	auto func_addr = (DWORD)GetModuleHandle(TEXT("WeChatWin.dll")) + 0x770980;
	using NewWxStringW_t = void(__fastcall*)(RawWxStringW* dst, int edx, RawWxStringW* src);
	auto NewWxStringW = (NewWxStringW_t)(func_addr);

	RawWxStringW src = {};
	src.vptr = (int)data.data();
	src.iLen = (int)data.size();
	src.iMaxLen = (int)data.size();
	NewWxStringW(dst, 0, &src);
}

typedef struct RawStlVector {
	int begin;
	int end;
	int capacity;
}RawStlVector;

//0x90
typedef struct RawWxRoomInfo
{
	int vptr = 0x6655F1F0; //vptr
	RawWxStringW room_wxid;
	RawWxStringW announcement;
	RawWxStringW account_wxid;
	int v40;
	int timestamp;
	int v48;
	int v4c;
	RawWxStringW xml;
	int v64 = 0x00000000;
	int v68 = 0x00000000;
	int v6C = 0x00000000;
	int v70 = 0x00000000;
	int v74 = 0x00000000;
	int v78 = 0x00000000;
	int v7C = 0x00000000;
	int v80 = 0x00000000;
	int v84 = 0x00000000;
	int v88 = 0x00000000;
	int v8C = 0x00000000;
}RawWxRoomInfo; 

typedef struct RawStlLinkNode {
	RawStlLinkNode* prev;
	RawStlLinkNode* next;
}RawStlLinkNode;


typedef struct RawStlLink {
	RawStlLinkNode* head;
	int size;
}RawStlLink;


typedef struct RawWxAnnouncementSource {
	int v4 = 0x00000001;
	int v0 = 0x0; 
	RawWxStringW hash;
	RawWxStringW room_wxid;
	RawWxStringW account_wxid;
	int timestamp;
	RawStlLink link;
}RawWxAnnouncementSource;



void SetWxRoomAnnouncement(std::wstring chatroomwxid, std::wstring Announcement)
{
	DWORD vptr1 = GetWeChatWinBase() + 0x1F6F1F0;
	DWORD vptr2 = GetWeChatWinBase() + 0x6655E70C - 0x646E0000;
	DWORD dwCall = GetWeChatWinBase() + 0x447ED0;

	RawWxStringW tRoomid = {};
	RawWxStringW tNoticeText = {};
	RawWxStringW tMyWxid = {};
	ConvertToWxStringW(&tRoomid, chatroomwxid);
	ConvertToWxStringW(&tNoticeText, Announcement);
	ConvertToWxStringW(&tMyWxid, L"你自己的微信id");

	sizeof(RawWxRoomInfo);
	RawWxRoomInfo room_info = {};
	room_info.vptr = vptr1;
	room_info.announcement = tNoticeText;
	room_info.room_wxid = tRoomid;
	room_info.account_wxid = tMyWxid;
	room_info.xml = tMyWxid;

	RawWxAnnouncementSource announcement_source = {};
	announcement_source.v0 = vptr2;
	announcement_source.hash = {};
	announcement_source.room_wxid = tRoomid;
	announcement_source.account_wxid = tMyWxid;

	RawStlLinkNode node = {};
	node.next = (RawStlLinkNode*)&node;
	node.prev = (RawStlLinkNode*)&node;

	announcement_source.link.head = (RawStlLinkNode*)&node;
	announcement_source.link.size = 0;

	RawStlVector raw_roominfo_vector = {};
	raw_roominfo_vector.begin = (int)&room_info;
	raw_roominfo_vector.end = raw_roominfo_vector.begin + sizeof(room_info);
	raw_roominfo_vector.capacity = raw_roominfo_vector.end;

	int asm_raw_roominfo_vector = (int)&raw_roominfo_vector;
	int asm_raw_announcement_source = (int)&announcement_source;
	__asm
	{
		pushad
		push asm_raw_announcement_source
		push asm_raw_roominfo_vector
		call dwCall
		popad
	}
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
	{
		OutputDebugStringA("发送群公告dll注入成功");
		SetWxRoomAnnouncement((wchar_t*)L"填你的群号", (wchar_t*)L"这是公告");
	}
	break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

