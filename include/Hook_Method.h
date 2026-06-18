#pragma once
#include <cstdint>


struct CALL_CONTEXT
{
    uint64_t r15;     // 0x00  ← rsp
    uint64_t r14;     // 0x08
    uint64_t r13;     // 0x10
    uint64_t r12;     // 0x18
    uint64_t r11;     // 0x20
    uint64_t r10;     // 0x28
    uint64_t r9;      // 0x30
    uint64_t r8;      // 0x38
    uint64_t rdi;     // 0x40
    uint64_t rsi;     // 0x48
    uint64_t rbp;     // 0x50
    uint64_t rbx;     // 0x58
    uint64_t rdx;     // 0x60
    uint64_t rcx;     // 0x68
    uint64_t rax;     // 0x70
    uint64_t rflags;  // 0x78
};



using CallHookHandler = void(*)(CALL_CONTEXT* ctx);
bool Hook_Call_R3(void* call_addr, size_t hook_len, CallHookHandler handler);
bool Hook_Call(void* call_addr, size_t hook_len, CallHookHandler handler);
bool Hook_Inline(void* inline_addr, size_t hook_len, CallHookHandler handler);
bool RemoveCallHook(void* call_addr);
