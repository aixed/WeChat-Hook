#include "HookManager.h"
#include <MinHook.h>

bool InitializeHookEngine()
{
    return (MH_Initialize() == MH_OK);
}

void UninitializeHookEngine()
{
    MH_Uninitialize();
}

bool Hook_SysApi(
    const wchar_t* moduleName,
    const char* functionName,
    LPVOID pDetour,
    LPVOID* ppOriginal
)
{
    if (!moduleName || !functionName || !pDetour || !ppOriginal)
        return false;

    HMODULE hModule = GetModuleHandleW(moduleName);
    if (!hModule)
    {
        hModule = LoadLibraryW(moduleName);
        if (!hModule)
            return false;
    }

    LPVOID pTarget = GetProcAddress(hModule, functionName);
    if (!pTarget)
        return false;

    if (MH_CreateHook(pTarget, pDetour, ppOriginal) != MH_OK)
        return false;

    if (MH_EnableHook(pTarget) != MH_OK)
    {
        MH_RemoveHook(pTarget);
        return false;
    }

    return true;
}

bool UnHook_SysApi(
    const wchar_t* moduleName,
    const char* functionName
)
{
    HMODULE hModule = GetModuleHandleW(moduleName);
    if (!hModule)
        return false;

    LPVOID pTarget = GetProcAddress(hModule, functionName);
    if (!pTarget)
        return false;

    MH_DisableHook(pTarget);
    MH_RemoveHook(pTarget);

    return true;
}