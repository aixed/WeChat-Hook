#pragma once
#include<windows.h>

void __stdcall SendFile(wchar_t* receiver, wchar_t* FilePath);
extern "C" __declspec(dllexport) void SendFileRemote(LPVOID lpParamStruct);