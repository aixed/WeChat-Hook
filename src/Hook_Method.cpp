#include <Windows.h>
#include <vector>
#include <cstring>
#include "global.h"
#include "Hook_Method.h"
#include "tools.h"


struct CALL_HOOK
{
    void* call_addr;
    uint8_t original[16];
    size_t stolen_len;
    void* trampoline;
};

static std::vector<CALL_HOOK> g_Hooks;

static void* AllocExec(size_t size)
{
    return VirtualAlloc(nullptr, size,
        MEM_RESERVE | MEM_COMMIT,
        PAGE_EXECUTE_READWRITE);
}

// 在 origin 附近 ±2GB 分配可执行内存
void* AllocNear(void* origin, size_t size)
{
    uintptr_t origin_addr = (uintptr_t)origin;

    const uintptr_t ranges[] = {
        0x08000000, // ±128MB
        0x20000000, // ±512MB
        0x40000000, // ±1GB
        0x7FFFFFFF  // ±2GB
    };

    for (uintptr_t range : ranges)
    {
        uintptr_t start = origin_addr > range
            ? origin_addr - range
            : 0;

        uintptr_t end = origin_addr + range;

        start &= ~0xFFFFULL;

        for (uintptr_t addr = start; addr < end; addr += 0x10000)
        {
            void* p = VirtualAlloc(
                (void*)addr,
                size,
                MEM_RESERVE | MEM_COMMIT,
                PAGE_EXECUTE_READWRITE
            );

            if (p)
            {
                //char buf[256];
                //sprintf_s(buf,"[AllocNear] success: origin=%p near=%p diff=%lld\n",origin,p,(long long)((int64_t)p - (int64_t)origin));
                //OutputDebugStringA(buf);

                return p;
            }
        }
    }

    //OutputDebugStringA("[AllocNear] failed\n");
    return nullptr;
}

//InstallCallHook_RetToAfter3
bool Hook_Call_R3(void* call_addr, size_t hook_len, CallHookHandler handler)
{
    if (hook_len < 5 || hook_len > 16)
        return false;

    CALL_HOOK hk{};
    hk.call_addr = call_addr;
    hk.stolen_len = hook_len;
    memcpy(hk.original, call_addr, hook_len);

    uint8_t* real = (uint8_t*)VirtualAlloc(
        nullptr, 512, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!real) return false;

    uint8_t* near_stub = (uint8_t*)AllocNear(call_addr, 32);
    if (!near_stub) return false;

    hk.trampoline = near_stub;

    uint8_t* p = real;

    // ================= 保存寄存器 =================
    *p++ = 0x9C;                         // pushfq
    *p++ = 0x50;                         // push rax
    *p++ = 0x51;                         // push rcx
    *p++ = 0x52;                         // push rdx
    *p++ = 0x53;                         // push rbx
    *p++ = 0x55;                         // push rbp
    *p++ = 0x56;                         // push rsi
    *p++ = 0x57;                         // push rdi
    *p++ = 0x41; *p++ = 0x50;            // push r8
    *p++ = 0x41; *p++ = 0x51;            // push r9
    *p++ = 0x41; *p++ = 0x52;            // push r10
    *p++ = 0x41; *p++ = 0x53;            // push r11
    *p++ = 0x41; *p++ = 0x54;            // push r12
    *p++ = 0x41; *p++ = 0x55;            // push r13
    *p++ = 0x41; *p++ = 0x56;            // push r14
    *p++ = 0x41; *p++ = 0x57;            // push r15

    // ================= shadow space =================
    *p++ = 0x48; *p++ = 0x83; *p++ = 0xEC; *p++ = 0x20;     //sub rsp, 0x20

    // RCX = &CALL_CONTEXT


    // RCX = &CALL_CONTEXT
    *p++ = 0x48;
    *p++ = 0x8D;
    *p++ = 0x8C; // ModRM
    *p++ = 0x24; // SIB
    *(uint32_t*)p = 0x20; // 偏移，指向 rflags
    p += 4;

    // call handler
    *p++ = 0x48; *p++ = 0xB8;
    *(void**)p = handler; p += 8;
    *p++ = 0xFF; *p++ = 0xD0;

    // ================= 恢复 shadow =================
    *p++ = 0x48; *p++ = 0x83; *p++ = 0xC4; *p++ = 0x20;         //add rsp, 0x20

    // ================= 恢复寄存器 =================
    *p++ = 0x41; *p++ = 0x5F; // pop r15
    *p++ = 0x41; *p++ = 0x5E; // pop r14
    *p++ = 0x41; *p++ = 0x5D; // pop r13
    *p++ = 0x41; *p++ = 0x5C; // pop r12
    *p++ = 0x41; *p++ = 0x5B; // pop r11
    *p++ = 0x41; *p++ = 0x5A; // pop r10
    *p++ = 0x41; *p++ = 0x59; // pop r9
    *p++ = 0x41; *p++ = 0x58; // pop r8
    *p++ = 0x5F;              // pop rdi
    *p++ = 0x5E;              // pop rsi
    *p++ = 0x5D;              // pop rbp
    *p++ = 0x5B;              // pop rbx
    *p++ = 0x5A;              // pop rdx
    *p++ = 0x59;              // pop rcx
    *p++ = 0x58;              // pop rax
    *p++ = 0x9D;              // popfq


    // ================= 原 call 3字节 =================
    //memcpy(p, hk.original, 3);
    //p += 3;
     
    // ================= 这必须是跳转到 weixin + 0x2AB0F5 =================
    //*p++ = 0xFF; *p++ = 0x25;
    //*(uint32_t*)p = 0; p += 4;
    //*(void**)p = WeixinDll_Offset(g_h_jmp_addr_1); 


    // ================= 原 call 及 hook 处的原始指令=================
    memcpy(p, hk.original, hook_len);
    p += hook_len;

    //执行原函数 call 后 jmp 回原函数后继续执行
    uintptr_t return_addr = (uintptr_t)call_addr + hook_len;
    *p++ = 0xFF; *p++ = 0x25;
    *(uint32_t*)p = 0; p += 4;
    *(void**)p = (void*)return_addr;

    // ================= near trampoline =================
    uint8_t* n = near_stub;
    *n++ = 0xFF; *n++ = 0x25;
    *(uint32_t*)n = 0;
    *(void**)(n + 4) = real;

    // ================= patch 原 call =================
    DWORD old;
    VirtualProtect(call_addr, hook_len, PAGE_EXECUTE_READWRITE, &old);
    *(uint8_t*)call_addr = 0xE9;
    *(int32_t*)((uint8_t*)call_addr + 1) =
        (int32_t)(near_stub - ((uint8_t*)call_addr + 5));
    memset((uint8_t*)call_addr + 5, 0x90, hook_len - 5);
    VirtualProtect(call_addr, hook_len, old, &old);

    g_Hooks.push_back(hk);
    return true;
}

bool Hook_Call(void* call_addr, size_t hook_len, CallHookHandler handler)
{
    if (hook_len < 5 || hook_len > 16)
        return false;

    CALL_HOOK hk{};
    hk.call_addr = call_addr;
    hk.stolen_len = hook_len;

    //复制出原指令处代码
    memcpy(hk.original, call_addr, hook_len);

    uint8_t* real = (uint8_t*)VirtualAlloc(nullptr, 512, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!real) return false;

    uint8_t* near_stub = (uint8_t*)AllocNear(call_addr, 0x100);
    if (!near_stub) return false;

    hk.trampoline = near_stub;

    uint8_t* p = real;

    // ================= 保存寄存器 =================
    *p++ = 0x9C;                         // pushfq
    *p++ = 0x50;                         // push rax
    *p++ = 0x51;                         // push rcx
    *p++ = 0x52;                         // push rdx
    *p++ = 0x53;                         // push rbx
    *p++ = 0x55;                         // push rbp
    *p++ = 0x56;                         // push rsi
    *p++ = 0x57;                         // push rdi
    *p++ = 0x41; *p++ = 0x50;            // push r8
    *p++ = 0x41; *p++ = 0x51;            // push r9
    *p++ = 0x41; *p++ = 0x52;            // push r10
    *p++ = 0x41; *p++ = 0x53;            // push r11
    *p++ = 0x41; *p++ = 0x54;            // push r12
    *p++ = 0x41; *p++ = 0x55;            // push r13
    *p++ = 0x41; *p++ = 0x56;            // push r14
    *p++ = 0x41; *p++ = 0x57;            // push r15

    // ================= shadow space =================
    *p++ = 0x48; *p++ = 0x83; *p++ = 0xEC; *p++ = 0x20;     //sub rsp, 0x20

    // RCX = &CALL_CONTEXT
    *p++ = 0x48;
    *p++ = 0x8D;
    *p++ = 0x8C; // ModRM
    *p++ = 0x24; // SIB
    *(uint32_t*)p = 0x20; // 偏移，指向 rflags
    p += 4;

    // call handler
    *p++ = 0x48; *p++ = 0xB8;
    *(void**)p = handler; p += 8;
    *p++ = 0xFF; *p++ = 0xD0;

    // ================= 恢复 shadow =================
    *p++ = 0x48; *p++ = 0x83; *p++ = 0xC4; *p++ = 0x20;         //add rsp, 0x20

    // ================= 恢复寄存器 =================
    *p++ = 0x41; *p++ = 0x5F; // pop r15
    *p++ = 0x41; *p++ = 0x5E; // pop r14
    *p++ = 0x41; *p++ = 0x5D; // pop r13
    *p++ = 0x41; *p++ = 0x5C; // pop r12
    *p++ = 0x41; *p++ = 0x5B; // pop r11
    *p++ = 0x41; *p++ = 0x5A; // pop r10
    *p++ = 0x41; *p++ = 0x59; // pop r9
    *p++ = 0x41; *p++ = 0x58; // pop r8
    *p++ = 0x5F;              // pop rdi
    *p++ = 0x5E;              // pop rsi
    *p++ = 0x5D;              // pop rbp
    *p++ = 0x5B;              // pop rbx
    *p++ = 0x5A;              // pop rdx
    *p++ = 0x59;              // pop rcx
    *p++ = 0x58;              // pop rax
    *p++ = 0x9D;              // popfq

    // ================= 跳转到 near_stub + 0x13 =================
    *p++ = 0xFF; *p++ = 0x25;      // jmp qword ptr [rip+0]
    *(uint32_t*)p = 0; p += 4;
    *(void**)p = (void*)((uint8_t*)near_stub + 0x13);

    // ================= near 区代码 =================
    uint8_t* n = near_stub;
    *n++ = 0xFF; *n++ = 0x25;
    *(uint32_t*)n = 0;
    *(void**)(n + 4) = real;
    
    // 第二部分 代码开始 往后 0x11 写入 自己业务处理后 返回 
    n += 0x11;
    
    //计算 call 原始函数地址（绝对）
    uintptr_t orig_call_target = (uintptr_t)call_addr + 5 + *(int32_t*)((uint8_t*)call_addr + 1);

    //调用原函数 旧写法 远跳 
    //*n++ = 0x48; *n++ = 0xB8;                 // mov rax, imm64
    //*(uintptr_t*)n = orig_call_target; n += 8;
    //*n++ = 0xFF; *n++ = 0xD0;                 // call rax

    //调用原函数 新写法 近跳 
    *n++ = 0xE8;
    int32_t rel = (int32_t)(orig_call_target - ((uintptr_t)n + 4));
    *(int32_t*)n = rel;
    n += 4;


    //执行原函数 call 后 jmp 回原函数后继续执行
    uintptr_t return_addr = (uintptr_t)call_addr + hook_len;
    *n++ = 0xFF; *n++ = 0x25;
    *(uint32_t*)n = 0; n += 4;
    *(void**)n = (void*)return_addr;



    // ================= patch 原 call =================
    DWORD old;
    VirtualProtect(call_addr, hook_len, PAGE_EXECUTE_READWRITE, &old);
    *(uint8_t*)call_addr = 0xE9;
    *(int32_t*)((uint8_t*)call_addr + 1) = (int32_t)(near_stub - ((uint8_t*)call_addr + 5));
    memset((uint8_t*)call_addr + 5, 0x90, hook_len - 5);
    VirtualProtect(call_addr, hook_len, old, &old);

    g_Hooks.push_back(hk);
    return true;
}

bool Hook_Inline(void* inline_addr, size_t hook_len, CallHookHandler handler)
{
    if (hook_len < 5 || hook_len > 16)
        return false;

    CALL_HOOK hk{};
    hk.call_addr = inline_addr;
    hk.stolen_len = hook_len;
    memcpy(hk.original, inline_addr, hook_len);

    uint8_t* real = (uint8_t*)VirtualAlloc(nullptr, 512, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!real) 
    {
		//OutputDebugStringA("[Hook_Inline] VirtualAlloc failed\n");
        return false;
    }
    uint8_t* near_stub = (uint8_t*)AllocNear(inline_addr, 32);
    if (!near_stub) 
    {
		//OutputDebugStringA("[Hook_Inline] AllocNear failed\n");
        return false;
    }
    hk.trampoline = near_stub;

    uint8_t* p = real;

    // ================= 保存寄存器 =================
    *p++ = 0x9C;                         // pushfq
    *p++ = 0x50;                         // push rax
    *p++ = 0x51;                         // push rcx
    *p++ = 0x52;                         // push rdx
    *p++ = 0x53;                         // push rbx
    *p++ = 0x55;                         // push rbp
    *p++ = 0x56;                         // push rsi
    *p++ = 0x57;                         // push rdi
    *p++ = 0x41; *p++ = 0x50;            // push r8
    *p++ = 0x41; *p++ = 0x51;            // push r9
    *p++ = 0x41; *p++ = 0x52;            // push r10
    *p++ = 0x41; *p++ = 0x53;            // push r11
    *p++ = 0x41; *p++ = 0x54;            // push r12
    *p++ = 0x41; *p++ = 0x55;            // push r13
    *p++ = 0x41; *p++ = 0x56;            // push r14
    *p++ = 0x41; *p++ = 0x57;            // push r15

    // ================= shadow space =================
    *p++ = 0x48; *p++ = 0x83; *p++ = 0xEC; *p++ = 0x20;     //sub rsp, 0x20

    // RCX = &CALL_CONTEXT


    // RCX = &CALL_CONTEXT
    *p++ = 0x48;
    *p++ = 0x8D;
    *p++ = 0x8C; // ModRM
    *p++ = 0x24; // SIB
    *(uint32_t*)p = 0x20; // 偏移，指向 rflags
    p += 4;

    // call handler
    *p++ = 0x48; *p++ = 0xB8;
    *(void**)p = handler; p += 8;
    *p++ = 0xFF; *p++ = 0xD0;

    // ================= 恢复 shadow =================
    *p++ = 0x48; *p++ = 0x83; *p++ = 0xC4; *p++ = 0x20;         //add rsp, 0x20

    // ================= 恢复寄存器 =================
    *p++ = 0x41; *p++ = 0x5F; // pop r15
    *p++ = 0x41; *p++ = 0x5E; // pop r14
    *p++ = 0x41; *p++ = 0x5D; // pop r13
    *p++ = 0x41; *p++ = 0x5C; // pop r12
    *p++ = 0x41; *p++ = 0x5B; // pop r11
    *p++ = 0x41; *p++ = 0x5A; // pop r10
    *p++ = 0x41; *p++ = 0x59; // pop r9
    *p++ = 0x41; *p++ = 0x58; // pop r8
    *p++ = 0x5F;              // pop rdi
    *p++ = 0x5E;              // pop rsi
    *p++ = 0x5D;              // pop rbp
    *p++ = 0x5B;              // pop rbx
    *p++ = 0x5A;              // pop rdx
    *p++ = 0x59;              // pop rcx
    *p++ = 0x58;              // pop rax
    *p++ = 0x9D;              // popfq

    // ================= 执行原位置代码 =================
    memcpy(p, hk.original, hook_len);
    p += hook_len;

    // ================= 跳转到 weixin + Hook后面的代码继续执行 =================
    uintptr_t return_addr = (uintptr_t)inline_addr + hook_len;
    *p++ = 0xFF; *p++ = 0x25;
    *(uint32_t*)p = 0; p += 4;
    *(void**)p = (void*)return_addr;

    // ================= near trampoline =================
    uint8_t* n = near_stub;
    *n++ = 0xFF; *n++ = 0x25;
    *(uint32_t*)n = 0;
    *(void**)(n + 4) = real;

    // ================= patch 原 call =================
    DWORD old;
    VirtualProtect(inline_addr, hook_len, PAGE_EXECUTE_READWRITE, &old);
    *(uint8_t*)inline_addr = 0xE9;
    *(int32_t*)((uint8_t*)inline_addr + 1) =
        (int32_t)(near_stub - ((uint8_t*)inline_addr + 5));
    memset((uint8_t*)inline_addr + 5, 0x90, hook_len - 5);
    VirtualProtect(inline_addr, hook_len, old, &old);

    g_Hooks.push_back(hk);
    return true;
}

bool RemoveCallHook(void* call_addr)
{
    for (auto it = g_Hooks.begin(); it != g_Hooks.end(); ++it)
    {
        if (it->call_addr == call_addr)
        {
            DWORD old;
            VirtualProtect(call_addr, it->stolen_len,
                PAGE_EXECUTE_READWRITE, &old);
            memcpy(call_addr, it->original, it->stolen_len);
            VirtualProtect(call_addr, it->stolen_len, old, &old);
            VirtualFree(it->trampoline, 0, MEM_RELEASE);
            g_Hooks.erase(it);
            return true;
        }
    }
    return false;
}
