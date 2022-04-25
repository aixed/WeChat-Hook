#pragma once
void __stdcall GetFriendList();

extern "C" __declspec(dllexport) DWORD GetFriendListRemote();
extern "C" __declspec(dllexport) int GetFriendListInit();
extern "C" __declspec(dllexport) void GetFriendListFinish();