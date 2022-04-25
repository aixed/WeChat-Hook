#pragma once
#include<windows.h>
BOOL __stdcall GetUserInfoByWxId(wchar_t* wxid);
extern "C" __declspec(dllexport) DWORD GetWxUserInfoRemote(LPVOID lparamter);
extern "C" __declspec(dllexport) VOID DeleteUserInfoCacheRemote();