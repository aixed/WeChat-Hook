#include <windows.h>
#include <winhttp.h>
#include <string>

#include "http_post.h"

#pragma comment(lib, "winhttp.lib")

struct HttpPostTask
{
    std::wstring url;
    std::string  json;
};

static DWORD WINAPI HttpPostThread(LPVOID lp)
{
    HttpPostTask* task = (HttpPostTask*)lp;
    if (!task)
        return 0;

    // ===== URL 解析 =====
    URL_COMPONENTSW uc{};
    uc.dwStructSize = sizeof(uc);

    wchar_t host[256] = {};
    wchar_t path[1024] = {};

    uc.lpszHostName = host;
    uc.dwHostNameLength = _countof(host);
    uc.lpszUrlPath = path;
    uc.dwUrlPathLength = _countof(path);

    if (!WinHttpCrackUrl(task->url.c_str(), 0, 0, &uc))
        return 0;

    // ===== Session =====
    HINTERNET hSession = WinHttpOpen(
        L"HTTP/1.1",
        WINHTTP_ACCESS_TYPE_NO_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS,
        0
    );
    if (!hSession)
        return 0;

    // ===== Connect =====
    HINTERNET hConnect = WinHttpConnect(
        hSession,
        uc.lpszHostName,
        uc.nPort,
        0
    );
    if (!hConnect)
    {
        WinHttpCloseHandle(hSession);
        return 0;
    }

    DWORD flags = (uc.nScheme == INTERNET_SCHEME_HTTPS)
        ? WINHTTP_FLAG_SECURE
        : 0;

    // ===== Request =====
    HINTERNET hRequest = WinHttpOpenRequest(
        hConnect,
        L"POST",
        uc.lpszUrlPath,
        nullptr,
        WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        flags
    );
    if (!hRequest)
    {
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return 0;
    }

    const wchar_t* headers =
        L"Content-Type: application/json\r\n";

    DWORD headersLen = (DWORD)wcslen(headers);

    // ===== 关键修复点 =====
    // 先 SendRequest（不带 body）
    BOOL ok = WinHttpSendRequest(
        hRequest,
        headers,
        headersLen,
        WINHTTP_NO_REQUEST_DATA,
        0,
        (DWORD)task->json.size(),   // 告诉 WinHTTP 总长度
        0
    );
    if (!ok)
        goto cleanup;

    // 再 WriteData 写入 body
    if (!task->json.empty())
    {
        DWORD written = 0;
        ok = WinHttpWriteData(
            hRequest,
            task->json.data(),
            (DWORD)task->json.size(),
            &written
        );
        if (!ok || written != task->json.size())
            goto cleanup;
    }

    // 接收响应
    WinHttpReceiveResponse(hRequest, nullptr);

cleanup:
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    return 0;
}

void HttpPostJsonAsync(const std::string& url, const std::string& json)
{
    auto* task = new HttpPostTask;

    // UTF-8 → UTF-16
    int len = MultiByteToWideChar(
        CP_UTF8,
        0,
        url.c_str(),
        -1,
        nullptr,
        0
    );
    if (len <= 1)
    {
        delete task;
        return;
    }

    task->url.resize(len - 1);
    MultiByteToWideChar(
        CP_UTF8,
        0,
        url.c_str(),
        -1,
        &task->url[0],
        len
    );

    task->json = json;

    HANDLE hThread = CreateThread(
        nullptr,
        0,
        HttpPostThread,
        task,
        0,
        nullptr
    );

    if (hThread)
        CloseHandle(hThread);
    else
        delete task;
}

bool HttpPostJsonSync(const std::string& url, const std::string& json)
{
    // ===== URL UTF-8 → UTF-16 =====
    int len = MultiByteToWideChar(CP_UTF8, 0, url.c_str(), -1, nullptr, 0);
    if (len <= 1)
        return false;

    std::wstring wurl(len - 1, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, url.c_str(), -1, &wurl[0], len);

    // ===== URL 解析 =====
    URL_COMPONENTSW uc{};
    uc.dwStructSize = sizeof(uc);

    wchar_t host[256] = {};
    wchar_t path[1024] = {};

    uc.lpszHostName = host;
    uc.dwHostNameLength = _countof(host);
    uc.lpszUrlPath = path;
    uc.dwUrlPathLength = _countof(path);

    if (!WinHttpCrackUrl(wurl.c_str(), 0, 0, &uc))
        return false;

    // ===== Session =====
    HINTERNET hSession = WinHttpOpen(
        L"HTTP/1.1",
        WINHTTP_ACCESS_TYPE_NO_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS,
        0
    );
    if (!hSession) return false;

    // ===== Connect =====
    HINTERNET hConnect = WinHttpConnect(
        hSession,
        uc.lpszHostName,
        uc.nPort,
        0
    );
    if (!hConnect)
    {
        WinHttpCloseHandle(hSession);
        return false;
    }

    DWORD flags = (uc.nScheme == INTERNET_SCHEME_HTTPS) ? WINHTTP_FLAG_SECURE : 0;

    // ===== Request =====
    HINTERNET hRequest = WinHttpOpenRequest(
        hConnect,
        L"POST",
        uc.lpszUrlPath,
        nullptr,
        WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        flags
    );
    if (!hRequest)
    {
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return false;
    }

    const wchar_t* headers = L"Content-Type: application/json\r\n";
    DWORD headersLen = (DWORD)wcslen(headers);

    // ===== 发送请求 =====
    BOOL ok = WinHttpSendRequest(
        hRequest,
        headers,
        headersLen,
        WINHTTP_NO_REQUEST_DATA,
        0,
        (DWORD)json.size(),   // 总长度
        0
    );
    if (!ok) goto cleanup;

    // 写入 body
    if (!json.empty())
    {
        DWORD written = 0;
        ok = WinHttpWriteData(
            hRequest,
            json.data(),
            (DWORD)json.size(),
            &written
        );
        if (!ok || written != json.size()) goto cleanup;
    }

    // 接收响应
    ok = WinHttpReceiveResponse(hRequest, nullptr);

cleanup:
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    return ok != FALSE;
}


static DWORD WINAPI HttpPostThread_(LPVOID lp)
{
    HttpPostTask* task = (HttpPostTask*)lp;
    if (!task)
        return 0;

    URL_COMPONENTSW uc{};
    uc.dwStructSize = sizeof(uc);

    wchar_t host[256] = { 0 };
    wchar_t path[1024] = { 0 };

    uc.lpszHostName = host;
    uc.dwHostNameLength = _countof(host);
    uc.lpszUrlPath = path;
    uc.dwUrlPathLength = _countof(path);

    if (!WinHttpCrackUrl(
        task->url.c_str(),
        0,
        0,
        &uc))
    {
        delete task;
        return 0;
    }

    HINTERNET hSession = WinHttpOpen(
        L"HTTP/1.0",
        WINHTTP_ACCESS_TYPE_NO_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS,
        0
    );
    if (!hSession)
    {
        delete task;
        return 0;
    }

    HINTERNET hConnect = WinHttpConnect(
        hSession,
        uc.lpszHostName,
        uc.nPort,
        0
    );
    if (!hConnect)
    {
        WinHttpCloseHandle(hSession);
        delete task;
        return 0;
    }

    DWORD flags = (uc.nScheme == INTERNET_SCHEME_HTTPS)
        ? WINHTTP_FLAG_SECURE
        : 0;

    HINTERNET hRequest = WinHttpOpenRequest(
        hConnect,
        L"POST",
        uc.lpszUrlPath,
        nullptr,
        WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        flags
    );
    if (!hRequest)
    {
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        delete task;
        return 0;
    }

    const wchar_t* headers = L"Content-Type: application/json\r\n";

    WinHttpSendRequest(
        hRequest,
        headers,
        -1,
        (LPVOID)task->json.data(),
        (DWORD)task->json.size(),
        (DWORD)task->json.size(),
        0
    );

    WinHttpReceiveResponse(hRequest, nullptr);

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);

    delete task;
    return 0;
}


void HttpPostJsonAsync_(const std::string& url, const std::string& json)
{
    auto* task = new HttpPostTask;

    // UTF-8 / ANSI → UTF-16
    int len = MultiByteToWideChar(CP_UTF8, 0, url.c_str(), -1, nullptr, 0);
    task->url.resize(len - 1);
    MultiByteToWideChar(CP_UTF8, 0, url.c_str(), -1, &task->url[0], len);

    task->json = json;

    HANDLE hThread = CreateThread(
        nullptr,
        0,
        HttpPostThread_,
        task,
        0,
        nullptr
    );

    if (hThread)
        CloseHandle(hThread);
}


