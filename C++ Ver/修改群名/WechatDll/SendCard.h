#pragma once
#include<windows.h>
extern "C" __declspec(dllexport) VOID SendCardRemote(LPVOID lparameter);
BOOL __stdcall SendCard(wchar_t* receiver, wchar_t* sharedwxid, wchar_t* nickname);