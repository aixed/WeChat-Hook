#pragma once
#include<windows.h>

extern "C" __declspec(dllexport) VOID CheckFriendStatusInitRemote();
extern "C" __declspec(dllexport) DWORD CheckFriendStatusRemote(LPVOID lparameter);
extern "C" __declspec(dllexport) VOID CheckFriendStatusFinishRemote();

VOID __stdcall CheckFriendStatus(wchar_t* wxid);