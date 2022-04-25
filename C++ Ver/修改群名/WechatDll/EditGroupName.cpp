#include "pch.h"



//wxid/UNICODE 结构体
struct wxString2
{
	wchar_t* pStr;
	DWORD strLen;
	DWORD strMaxLen;
	DWORD fill;
	DWORD fill2;
};



//修改群名call
void __stdcall Edit_Group_Name(wchar_t* group_id, wchar_t* group_name)
{
	DWORD WeChatWin_DLL = (DWORD)GetModuleHandleA("WeChatWin.dll");
	DWORD callAdd_1 = WeChatWin_DLL + 0x9A060;
	DWORD callAdd_2 = WeChatWin_DLL + 0x494ED0;
	DWORD dbHandle_ = WeChatWin_DLL + 0x222F13C;

	WxBaseStruct pGroupID(group_id);
	WxBaseStruct pGroupName(group_name);

	//Ecx_Struct Ecx_Handle = { 0 };
	Ecx_Struct Ecx_Handle;

	Ecx_Handle.Value = WeChatWin_DLL + 0x222F13C;


	__asm {
		pushad
		call callAdd_1
		lea ebx, pGroupName
		lea esi, pGroupID
		mov edi, esi
		push ebx
		push esi
		lea ecx, Ecx_Handle
		mov ecx, eax
		call callAdd_2
		popad
		
	}
}






