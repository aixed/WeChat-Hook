#pragma once
#include <windows.h>
#include <string>

bool Hook_SysApi(
    const wchar_t* moduleName,
    const char* functionName,
    LPVOID pDetour,
    LPVOID* ppOriginal
);

bool UnHook_SysApi(
    const wchar_t* moduleName,
    const char* functionName
);

bool InitializeHookEngine();
void UninitializeHookEngine();