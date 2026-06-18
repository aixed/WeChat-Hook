#include <windows.h>
#include <winternl.h>
#include <stdio.h>
#include "hideself.h"

// 声明未文档化的函数
typedef NTSTATUS(NTAPI* pNtQueryInformationProcess)(
    HANDLE ProcessHandle,
    PROCESSINFOCLASS ProcessInformationClass,
    PVOID ProcessInformation,
    ULONG ProcessInformationLength,
    PULONG ReturnLength
    );

// PEB_LDR_DATA 结构定义
typedef struct _PEB_LDR_DATA2 {
    ULONG Length;
    BOOLEAN Initialized;
    PVOID SsHandle;
    LIST_ENTRY InLoadOrderModuleList;
    LIST_ENTRY InMemoryOrderModuleList;
    LIST_ENTRY InInitializationOrderModuleList;
} PEB_LDR_DATA2;

// LDR_DATA_TABLE_ENTRY 部分定义
typedef struct _LDR_DATA_TABLE_ENTRY2 {
    LIST_ENTRY InLoadOrderLinks;
    LIST_ENTRY InMemoryOrderLinks;
    LIST_ENTRY InInitializationOrderLinks;
    PVOID DllBase;
    PVOID EntryPoint;
    ULONG SizeOfImage;
    UNICODE_STRING FullDllName;
    UNICODE_STRING BaseDllName;
    // ... 后续字段不重要
} LDR_DATA_TABLE_ENTRY2, * PLDR_DATA_TABLE_ENTRY2;

// 从 PEB 中隐藏当前 DLL
bool HideModuleFromPEB(HMODULE hModule) {
    // 获取当前线程的 PEB 地址
    pNtQueryInformationProcess NtQueryInformationProcess =
        (pNtQueryInformationProcess)GetProcAddress(GetModuleHandleW(L"ntdll.dll"),
            "NtQueryInformationProcess");

    if (!NtQueryInformationProcess) return false;

    PROCESS_BASIC_INFORMATION pbi;
    ULONG returnLength;

    if (NtQueryInformationProcess(GetCurrentProcess(),
        ProcessBasicInformation,
        &pbi,
        sizeof(pbi),
        &returnLength) != 0) {
        return false;
    }

    PPEB peb = pbi.PebBaseAddress;
    if (!peb) return false;

    // 获取 PEB LDR 数据
    PEB_LDR_DATA2* ldr = (PEB_LDR_DATA2*)peb->Ldr;
    if (!ldr) return false;

    // 禁用 DEP 以允许修改只读内存
    DWORD oldProtect;
    VirtualProtect(ldr, sizeof(PEB_LDR_DATA2), PAGE_READWRITE, &oldProtect);

    // 遍历 InLoadOrderModuleList
    PLIST_ENTRY head = &ldr->InLoadOrderModuleList;
    PLIST_ENTRY entry = head->Flink;

    while (entry != head) {
        LDR_DATA_TABLE_ENTRY2* module = CONTAINING_RECORD(entry, LDR_DATA_TABLE_ENTRY2, InLoadOrderLinks);

        if (module->DllBase == hModule) {
            // 从所有三个链表中移除该模块
            PLIST_ENTRY prev = entry->Blink;
            PLIST_ENTRY next = entry->Flink;

            // 修改内存保护以允许写操作
            DWORD oldProtectEntry;
            VirtualProtect(module, sizeof(LDR_DATA_TABLE_ENTRY2), PAGE_READWRITE, &oldProtectEntry);

            // 从 InLoadOrder 链表中移除
            prev->Flink = next;
            next->Blink = prev;

            // 从 InMemoryOrder 链表中移除
            PLIST_ENTRY memEntry = &module->InMemoryOrderLinks;
            prev = memEntry->Blink;
            next = memEntry->Flink;
            prev->Flink = next;
            next->Blink = prev;

            // 从 InInitializationOrder 链表中移除
            PLIST_ENTRY initEntry = &module->InInitializationOrderLinks;
            prev = initEntry->Blink;
            next = initEntry->Flink;
            prev->Flink = next;
            next->Blink = prev;

            // 恢复内存保护
            VirtualProtect(module, sizeof(LDR_DATA_TABLE_ENTRY2), oldProtectEntry, &oldProtectEntry);

            VirtualProtect(ldr, sizeof(PEB_LDR_DATA2), oldProtect, &oldProtect);
            return true;
        }

        entry = entry->Flink;
    }

    VirtualProtect(ldr, sizeof(PEB_LDR_DATA2), oldProtect, &oldProtect);
    return false;
}
