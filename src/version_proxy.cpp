#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include <MinHook.h>
#include "global.h"
#include "tools.h"
#include "Hook_Method.h"
#include "inline_weixin_dll_load.h"
#include "version_proxy.h"

/* =========================
   全局真实 DLL 句柄
   ========================= */
static HMODULE g_hRealVersion = nullptr;

/* =========================
   加载真实 version.dll
   ========================= */
static void LoadRealDll()
{
    if (g_hRealVersion)
        return;

    wchar_t sysDir[MAX_PATH]{};
    GetSystemDirectoryW(sysDir, MAX_PATH);

    std::wstring path = sysDir;
    path += L"\\version.dll";

    g_hRealVersion = LoadLibraryW(path.c_str());
}

/* =========================
   真实函数指针类型定义
   ========================= */
typedef BOOL(WINAPI* PFN_GetFileVersionInfoA)(
    LPCSTR, DWORD, DWORD, LPVOID);
typedef BOOL(WINAPI* PFN_GetFileVersionInfoByHandle)(
    HANDLE, DWORD, DWORD, LPVOID);
typedef BOOL(WINAPI* PFN_GetFileVersionInfoExA)(
    DWORD, LPCSTR, DWORD, DWORD, LPVOID);
typedef BOOL(WINAPI* PFN_GetFileVersionInfoExW)(
    DWORD, LPCWSTR, DWORD, DWORD, LPVOID);
typedef DWORD(WINAPI* PFN_GetFileVersionInfoSizeA)(
    LPCSTR, LPDWORD);
typedef DWORD(WINAPI* PFN_GetFileVersionInfoSizeExA)(
    DWORD, LPCSTR, LPDWORD);
typedef DWORD(WINAPI* PFN_GetFileVersionInfoSizeExW)(
    DWORD, LPCWSTR, LPDWORD);
typedef DWORD(WINAPI* PFN_GetFileVersionInfoSizeW)(
    LPCWSTR, LPDWORD);
typedef BOOL(WINAPI* PFN_GetFileVersionInfoW)(
    LPCWSTR, DWORD, DWORD, LPVOID);
typedef DWORD(WINAPI* PFN_VerFindFileA)(
    DWORD, LPSTR, LPSTR, LPSTR, LPSTR, PUINT, LPSTR, PUINT);
typedef DWORD(WINAPI* PFN_VerFindFileW)(
    DWORD, LPWSTR, LPWSTR, LPWSTR, LPWSTR, PUINT, LPWSTR, PUINT);
typedef DWORD(WINAPI* PFN_VerInstallFileA)(
    DWORD, LPSTR, LPSTR, LPSTR, LPSTR, LPSTR, LPSTR, PUINT);
typedef DWORD(WINAPI* PFN_VerInstallFileW)(
    DWORD, LPWSTR, LPWSTR, LPWSTR, LPWSTR, LPWSTR, LPWSTR, PUINT);
typedef DWORD(WINAPI* PFN_VerLanguageNameA)(
    DWORD, LPSTR, DWORD);
typedef DWORD(WINAPI* PFN_VerLanguageNameW)(
    DWORD, LPWSTR, DWORD);
typedef BOOL(WINAPI* PFN_VerQueryValueA)(
    LPCVOID, LPCSTR, LPVOID*, PUINT);
typedef BOOL(WINAPI* PFN_VerQueryValueW)(
    LPCVOID, LPCWSTR, LPVOID*, PUINT);

/* =========================
   真实函数指针变量（全部17个）
   ========================= */
static PFN_GetFileVersionInfoA       pGetFileVersionInfoA       = nullptr;
static PFN_GetFileVersionInfoByHandle pGetFileVersionInfoByHandle = nullptr;
static PFN_GetFileVersionInfoExA     pGetFileVersionInfoExA     = nullptr;
static PFN_GetFileVersionInfoExW     pGetFileVersionInfoExW     = nullptr;
static PFN_GetFileVersionInfoSizeA   pGetFileVersionInfoSizeA   = nullptr;
static PFN_GetFileVersionInfoSizeExA pGetFileVersionInfoSizeExA = nullptr;
static PFN_GetFileVersionInfoSizeExW pGetFileVersionInfoSizeExW = nullptr;
static PFN_GetFileVersionInfoSizeW   pGetFileVersionInfoSizeW   = nullptr;
static PFN_GetFileVersionInfoW       pGetFileVersionInfoW       = nullptr;
static PFN_VerFindFileA              pVerFindFileA              = nullptr;
static PFN_VerFindFileW              pVerFindFileW              = nullptr;
static PFN_VerInstallFileA           pVerInstallFileA           = nullptr;
static PFN_VerInstallFileW           pVerInstallFileW           = nullptr;
static PFN_VerLanguageNameA          pVerLanguageNameA          = nullptr;
static PFN_VerLanguageNameW          pVerLanguageNameW          = nullptr;
static PFN_VerQueryValueA            pVerQueryValueA            = nullptr;
static PFN_VerQueryValueW            pVerQueryValueW            = nullptr;

/* =========================
   初始化真实导出（全部17个）
   ========================= */
void InitRealDll()
{
    static bool inited = false;
    if (inited)
        return;

    LoadRealDll();
    if (!g_hRealVersion)
        return;

    pGetFileVersionInfoA =
        (PFN_GetFileVersionInfoA)GetProcAddress(g_hRealVersion, "GetFileVersionInfoA");
    pGetFileVersionInfoByHandle =
        (PFN_GetFileVersionInfoByHandle)GetProcAddress(g_hRealVersion, "GetFileVersionInfoByHandle");
    pGetFileVersionInfoExA =
        (PFN_GetFileVersionInfoExA)GetProcAddress(g_hRealVersion, "GetFileVersionInfoExA");
    pGetFileVersionInfoExW =
        (PFN_GetFileVersionInfoExW)GetProcAddress(g_hRealVersion, "GetFileVersionInfoExW");
    pGetFileVersionInfoSizeA =
        (PFN_GetFileVersionInfoSizeA)GetProcAddress(g_hRealVersion, "GetFileVersionInfoSizeA");
    pGetFileVersionInfoSizeExA =
        (PFN_GetFileVersionInfoSizeExA)GetProcAddress(g_hRealVersion, "GetFileVersionInfoSizeExA");
    pGetFileVersionInfoSizeExW =
        (PFN_GetFileVersionInfoSizeExW)GetProcAddress(g_hRealVersion, "GetFileVersionInfoSizeExW");
    pGetFileVersionInfoSizeW =
        (PFN_GetFileVersionInfoSizeW)GetProcAddress(g_hRealVersion, "GetFileVersionInfoSizeW");
    pGetFileVersionInfoW =
        (PFN_GetFileVersionInfoW)GetProcAddress(g_hRealVersion, "GetFileVersionInfoW");
    pVerFindFileA =
        (PFN_VerFindFileA)GetProcAddress(g_hRealVersion, "VerFindFileA");
    pVerFindFileW =
        (PFN_VerFindFileW)GetProcAddress(g_hRealVersion, "VerFindFileW");
    pVerInstallFileA =
        (PFN_VerInstallFileA)GetProcAddress(g_hRealVersion, "VerInstallFileA");
    pVerInstallFileW =
        (PFN_VerInstallFileW)GetProcAddress(g_hRealVersion, "VerInstallFileW");
    pVerLanguageNameA =
        (PFN_VerLanguageNameA)GetProcAddress(g_hRealVersion, "VerLanguageNameA");
    pVerLanguageNameW =
        (PFN_VerLanguageNameW)GetProcAddress(g_hRealVersion, "VerLanguageNameW");
    pVerQueryValueA =
        (PFN_VerQueryValueA)GetProcAddress(g_hRealVersion, "VerQueryValueA");
    pVerQueryValueW =
        (PFN_VerQueryValueW)GetProcAddress(g_hRealVersion, "VerQueryValueW");

    inited = true;
}

/* =========================
   代理实现 — 全部 17 个导出函数
   ========================= */

/* 1  */ BOOL WINAPI My_GetFileVersionInfoA(
    LPCSTR lpFileName, DWORD dwHandle, DWORD dwLen, LPVOID lpData)
{
    InitRealDll();
    return pGetFileVersionInfoA(lpFileName, dwHandle, dwLen, lpData);
}

/* 2  */ BOOL WINAPI My_GetFileVersionInfoByHandle(
    HANDLE hFile, DWORD dwHandle, DWORD dwLen, LPVOID lpData)
{
    InitRealDll();
    return pGetFileVersionInfoByHandle(hFile, dwHandle, dwLen, lpData);
}

/* 3  */ BOOL WINAPI My_GetFileVersionInfoExA(
    DWORD dwFlags, LPCSTR lpFileName, DWORD dwHandle, DWORD dwLen, LPVOID lpData)
{
    InitRealDll();
    return pGetFileVersionInfoExA(dwFlags, lpFileName, dwHandle, dwLen, lpData);
}

/* 4  */ BOOL WINAPI My_GetFileVersionInfoExW(
    DWORD dwFlags, LPCWSTR lpFileName, DWORD dwHandle, DWORD dwLen, LPVOID lpData)
{
    InitRealDll();
    return pGetFileVersionInfoExW(dwFlags, lpFileName, dwHandle, dwLen, lpData);
}

/* 5  */ DWORD WINAPI My_GetFileVersionInfoSizeA(
    LPCSTR lpFileName, LPDWORD lpdwHandle)
{
    InitRealDll();
    return pGetFileVersionInfoSizeA(lpFileName, lpdwHandle);
}

/* 6  */ DWORD WINAPI My_GetFileVersionInfoSizeExA(
    DWORD dwFlags, LPCSTR lpFileName, LPDWORD lpdwHandle)
{
    InitRealDll();
    return pGetFileVersionInfoSizeExA(dwFlags, lpFileName, lpdwHandle);
}

/* 7  */ DWORD WINAPI My_GetFileVersionInfoSizeExW(
    DWORD dwFlags, LPCWSTR lpFileName, LPDWORD lpdwHandle)
{
    InitRealDll();
    return pGetFileVersionInfoSizeExW(dwFlags, lpFileName, lpdwHandle);
}

/* 8  */ DWORD WINAPI My_GetFileVersionInfoSizeW(
    LPCWSTR lpFileName, LPDWORD lpdwHandle)
{
    InitRealDll();
    return pGetFileVersionInfoSizeW(lpFileName, lpdwHandle);
}

/* 9  */ BOOL WINAPI My_GetFileVersionInfoW(
    LPCWSTR lpFileName, DWORD dwHandle, DWORD dwLen, LPVOID lpData)
{
    InitRealDll();
    return pGetFileVersionInfoW(lpFileName, dwHandle, dwLen, lpData);
}

/* 10 */ DWORD WINAPI My_VerFindFileA(
    DWORD dwFlags, LPSTR lpFilename, LPSTR lpWinDir, LPSTR lpAppDir,
    LPSTR lpCurDir, PUINT puCurDirLen, LPSTR lpDestFolder, PUINT puDestFolderLen)
{
    InitRealDll();
    return pVerFindFileA(dwFlags, lpFilename, lpWinDir, lpAppDir,
        lpCurDir, puCurDirLen, lpDestFolder, puDestFolderLen);
}

/* 11 */ DWORD WINAPI My_VerFindFileW(
    DWORD dwFlags, LPWSTR lpFilename, LPWSTR lpWinDir, LPWSTR lpAppDir,
    LPWSTR lpCurDir, PUINT puCurDirLen, LPWSTR lpDestFolder, PUINT puDestFolderLen)
{
    InitRealDll();
    return pVerFindFileW(dwFlags, lpFilename, lpWinDir, lpAppDir,
        lpCurDir, puCurDirLen, lpDestFolder, puDestFolderLen);
}

/* 12 */ DWORD WINAPI My_VerInstallFileA(
    DWORD dwFlags, LPSTR lpSrcFileName, LPSTR lpDestFileName,
    LPSTR lpSrcDir, LPSTR lpDestDir, LPSTR lpCurDir, LPSTR lpTempDir, PUINT puTempFileLen)
{
    InitRealDll();
    return pVerInstallFileA(dwFlags, lpSrcFileName, lpDestFileName,
        lpSrcDir, lpDestDir, lpCurDir, lpTempDir, puTempFileLen);
}

/* 13 */ DWORD WINAPI My_VerInstallFileW(
    DWORD dwFlags, LPWSTR lpSrcFileName, LPWSTR lpDestFileName,
    LPWSTR lpSrcDir, LPWSTR lpDestDir, LPWSTR lpCurDir, LPWSTR lpTempDir, PUINT puTempFileLen)
{
    InitRealDll();
    return pVerInstallFileW(dwFlags, lpSrcFileName, lpDestFileName,
        lpSrcDir, lpDestDir, lpCurDir, lpTempDir, puTempFileLen);
}

/* 14 */ DWORD WINAPI My_VerLanguageNameA(
    DWORD wLangId, LPSTR lpLangName, DWORD nSize)
{
    InitRealDll();
    return pVerLanguageNameA(wLangId, lpLangName, nSize);
}

/* 15 */ DWORD WINAPI My_VerLanguageNameW(
    DWORD wLangId, LPWSTR lpLangName, DWORD nSize)
{
    InitRealDll();
    return pVerLanguageNameW(wLangId, lpLangName, nSize);
}

/* 16 */ BOOL WINAPI My_VerQueryValueA(
    LPCVOID pBlock, LPCSTR lpSubBlock, LPVOID* lplpBuffer, PUINT puLen)
{
    InitRealDll();
    return pVerQueryValueA(pBlock, lpSubBlock, lplpBuffer, puLen);
}

/* 17 */ BOOL WINAPI My_VerQueryValueW(
    LPCVOID pBlock, LPCWSTR lpSubBlock, LPVOID* lplpBuffer, PUINT puLen)
{
    InitRealDll();
    return pVerQueryValueW(pBlock, lpSubBlock, lplpBuffer, puLen);
}

/* =========================
   劫持入口（与 version 一致）
   ========================= */
void CustomInit(HMODULE hModule)
{
    g_hWeixinExe = GetModuleHandleW(L"weixin.exe");
    if (!g_hWeixinExe)
        return;

    if (MH_Initialize() != MH_OK)
        return;

    Evt_WeixinLoad();
}
