// dllmain.cpp
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include "tools.h"
#include "version_proxy.h"
#include "hideself.h"
#include "global.h"

int g_receive_type = 1;
int g_StartPort = 30001;
int g_MsgSendPort = 80;

static void ParseCmdLine(const std::string& cmd)
{
    const std::string key = "StartPort=";

    size_t pos = cmd.find(key);
    if (pos != std::string::npos)
    {
        pos += key.length();

        int port = 0;
        while (pos < cmd.length())
        {
            char ch = cmd[pos];
            if (ch < '0' || ch > '9')
                break;

            port = port * 10 + (ch - '0');
            pos++;
        }

        if (port > 0 && port <= 65535)
        {
            g_StartPort = port;
        }
    }


    const std::string key_1 = "RecvType=";

    size_t pos_1 = cmd.find(key_1);
    if (pos_1 != std::string::npos)
    {
        pos_1 += key_1.length();

        int receive_type = 0;
        while (pos_1 < cmd.length())
        {
            char ch = cmd[pos_1];
            if (ch < '0' || ch > '9')
                break;

            receive_type = receive_type * 10 + (ch - '0');
            pos_1++;
        }

        if (receive_type > 0 && receive_type <= 2)
        {
            g_receive_type = receive_type;
        }
    }


    // ----------------------------
    // 解析 CallBackURL
    // ----------------------------

    const std::string recv_url = "CallBackURL=";
    size_t recv_pos = cmd.find(recv_url);

    if (recv_pos != std::string::npos)
    {
        recv_pos += recv_url.length();

        std::string url = ExtractQuotedString(cmd, recv_pos);

        if (!url.empty())
        {
            g_CallBack_Url = url;

            // 默认端口
            g_MsgSendPort = 80;

            // 查找协议分隔
            size_t scheme_end = url.find("://");
            size_t host_start = (scheme_end == std::string::npos) ? 0 : scheme_end + 3;

            // 找 host:port 部分结束位置
            size_t path_pos = url.find('/', host_start);
            std::string host_port = (path_pos == std::string::npos)
                ? url.substr(host_start)
                : url.substr(host_start, path_pos - host_start);

            // 查找端口
            size_t colon_pos = host_port.find(':');
            if (colon_pos != std::string::npos)
            {
                std::string port_str = host_port.substr(colon_pos + 1);

                int port = atoi(port_str.c_str());
                if (port > 0 && port <= 65535)
                {
                    g_MsgSendPort = port;
                }
            }
        }
    }

}

static std::string GetCmdLine()
{
    std::wstring wcmd = GetCommandLineW();
    return WStringToString(wcmd); 
}


static bool IsMainWeixinProcess()
{
    std::string cmd = GetCmdLine();
	//OutputDebugStringA(("[CmdLine] " + cmd + "\n").c_str());

    // 子进程特征（只要命中一个，就不是主进程）
    if (cmd.find("--type=") != std::string::npos)
        return false;

    if (cmd.find("--crashpad-handler") != std::string::npos)
        return false;

    ParseCmdLine(cmd);

    return true; // 剩下的就是主进程
}

DWORD WINAPI SelfUnloadThread(LPVOID param)
{
    HMODULE hMod = (HMODULE)param;
    Sleep(100); // 给系统一点时间
    FreeLibraryAndExitThread(hMod, 0);
    return 0;
}


BOOL APIENTRY DllMain(
    HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        g_hModule = hModule;  // 保存模块基址

        DisableThreadLibraryCalls(hModule);

        // 先加载和初始化真实 version.dll
        InitRealDll();

        if (!IsMainWeixinProcess())
        {
            // 子进程：直接卸载自己
            CreateThread(nullptr,0,SelfUnloadThread,hModule,0,nullptr);
            //OutputDebugStringA("[VxHook] not main process!\n");
            return TRUE;
        }
        //OutputDebugStringA("[VxHook] main process!\n");

        HideModuleFromPEB(hModule); // 隐藏本模块
#ifdef _DEBUG
		OutputDebugStringA("[VxHook] HideModuleFromPEB success!\n");
#endif

        // 主进程才执行
        CustomInit(hModule);
    }
    return TRUE;
}
