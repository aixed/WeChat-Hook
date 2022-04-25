// pch.cpp: 与预编译标头对应的源文件

#include "pch.h"

// 当使用预编译的头时，需要使用此源文件，编译才能成功。
BOOL CreateConsole(void) {
    if (AllocConsole()) {
        AttachConsole(GetCurrentProcessId());
        FILE* retStream;
        freopen_s(&retStream, "CONOUT$", "w", stdout);
        if (!retStream) throw std::runtime_error("Stdout redirection failed.");
        freopen_s(&retStream, "CONOUT$", "w", stderr);
        if (!retStream) throw std::runtime_error("Stderr redirection failed.");
        return 0;
    }
    return 1;
}

DWORD GetWeChatWinBase() {
    return (DWORD)GetModuleHandleA("WeChatWin.dll");
}

void Wchar_tToString(std::string& szDst, wchar_t* wchar)
{
    wchar_t* wText = wchar;
    DWORD dwNum = WideCharToMultiByte(CP_OEMCP, NULL, wText, -1, NULL, 0, NULL, FALSE);// WideCharToMultiByte的运用
    char* psText; // psText为char*的临时数组，作为赋值给std::string的中间变量
    psText = new char[dwNum];
    WideCharToMultiByte(CP_OEMCP, NULL, wText, -1, psText, dwNum, NULL, FALSE);// WideCharToMultiByte的再次运用
    szDst = psText;// std::string赋值
    delete[]psText;// psText的清除
}

void HookAnyAddress(DWORD dwHookAddr, LPVOID dwJmpAddress,char* originalRecieveCode)
{
    //组装跳转数据
    BYTE jmpCode[5] = { 0 };
    jmpCode[0] = 0xE9;

    //计算偏移
    *(DWORD*)&jmpCode[1] = (DWORD)dwJmpAddress - dwHookAddr - 5;

    // 保存以前的属性用于还原
    DWORD OldProtext = 0;

    // 因为要往代码段写入数据，又因为代码段是不可写的，所以需要修改属性
    VirtualProtect((LPVOID)dwHookAddr, 5, PAGE_EXECUTE_READWRITE, &OldProtext);
    ReadProcessMemory(GetCurrentProcess(), (LPVOID)dwHookAddr, originalRecieveCode, 5, 0);

    //写入自己的代码
    memcpy((void*)dwHookAddr, jmpCode, 5);

    // 执行完了操作之后需要进行还原
    VirtualProtect((LPVOID)dwHookAddr, 5, OldProtext, &OldProtext);
}

void UnHookAnyAddress(DWORD dwHookAddr, char* originalRecieveCode)
{
    DWORD OldProtext = 0;
    VirtualProtect((LPVOID)dwHookAddr, 5, PAGE_EXECUTE_READWRITE, &OldProtext);
    WriteProcessMemory(GetCurrentProcess(), (LPVOID)dwHookAddr, originalRecieveCode, 5, 0);
    VirtualProtect((LPVOID)dwHookAddr, 5, OldProtext, &OldProtext);
}

void UnHookAll() {
    UnHookLogMsgInfo();
    return;
}

wstring wreplace(wstring source, wchar_t replaced, wstring replaceto) {
    wstring temp = L"";
    wchar_t* buffer = (wchar_t*)source.c_str();
    for (unsigned int i = 0; i < source.length(); i++) {
        if (buffer[i] == replaced) {
            temp += replaceto;
            continue;
        }
        temp += buffer[i];
    }
    return temp;
}