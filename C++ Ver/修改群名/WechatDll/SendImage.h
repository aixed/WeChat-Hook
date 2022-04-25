#pragma once
#include<windows.h>

void __stdcall SendImage(wchar_t* receiver, wchar_t* ImagePath);
extern "C" __declspec(dllexport) void SendImageRemote(LPVOID lpParamStruct);