#include "wx_ini_reader.h"
#include "global.h"
#include <windows.h>
#include <Shlwapi.h>   // PathRemoveFileSpecW
#include <string>

#pragma comment(lib, "Shlwapi.lib")

void GetWxRecvUrl()
{
    std::string callbackurl = "";
    if (g_CallBack_Url == "") {
        wchar_t exePath[MAX_PATH] = { 0 };
        GetModuleFileNameW(nullptr, exePath, MAX_PATH);

        // 获取目录
        PathRemoveFileSpecW(exePath);

        // 拼接 wx.ini 路径
        wchar_t iniPath[MAX_PATH] = { 0 };
        swprintf_s(iniPath, L"%s\\wx.ini", exePath);

        wchar_t recvUrlW[1024] = { 0 };

        // 读取 [wx] 区域的 recv_url
        GetPrivateProfileStringW(
            L"wx",
            L"recv_url",
            L"",
            recvUrlW,
            _countof(recvUrlW),
            iniPath
        );

        // 转成 UTF-8 / std::string
        int len = WideCharToMultiByte(CP_UTF8, 0, recvUrlW, -1, nullptr, 0, nullptr, nullptr);
        std::string recvUrl(len - 1, 0);
        WideCharToMultiByte(CP_UTF8, 0, recvUrlW, -1, &recvUrl[0], len, nullptr, nullptr);
        g_CallBack_Url = recvUrl;
        return;
    }
    return;

}
