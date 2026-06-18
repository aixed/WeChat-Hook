#define NOMINMAX
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <iomanip>
#include <windows.h>
#include <ShlObj.h>
#include <iphlpapi.h>
#include <intrin.h>
#include <filesystem>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "crypt32.lib")

#include "global.h"
#include "tools.h"

void InitStandardPaths()
{
    wchar_t path[MAX_PATH] = { 0 };

    // 获取实际的 AppData 路径
    if (SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, path) == S_OK)
    {
        // C:\Users\Administrator\AppData\Roaming
        g_AppDataDir = path;
    }

    // 获取实际的 Documents 路径
    if (SHGetFolderPathW(NULL, CSIDL_MYDOCUMENTS, NULL, 0, path) == S_OK)
    {
        // C:\Users\Administrator\Documents
        g_DocumentDir = path;
    }

    // 获取实际的 Users 路径
    if (SHGetFolderPathW(NULL, CSIDL_PROFILE, NULL, 0, path) == S_OK)
    {
        //C:\Users\Administrator
        g_UsersDir = path;
    }
    
    //C:\Users\Administrator\AppData\Roaming\WxDirDataPath
    g_MyDir = g_AppDataDir + L"\\WxDirDataPath";

    //创建目录
    CreateDirectoryW((g_MyDir + L"\\").c_str(), NULL);

}

std::string format_string(const char* format, ...) {
    char buffer[512];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    return std::string(buffer);
}

// 宽字符字符串转多字节字符串
std::string WStringToString(const std::wstring& wstr)
{
    if (wstr.empty())
        return "";

    int size_needed = WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(),
        NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(),
        &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

// 多字节字符串转宽字符字符串
std::wstring StringToWString(const std::string& str)
{
    if (str.empty())
        return L"";

    int size_needed = MultiByteToWideChar(CP_ACP, 0, &str[0], (int)str.size(),
        NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_ACP, 0, &str[0], (int)str.size(),
        &wstrTo[0], size_needed);
    return wstrTo;
}

// 辅助函数：从字符串中提取引号内的内容
std::string ExtractQuotedString(const std::string& str, size_t start_pos)
{
    if (start_pos >= str.length())
        return "";

    // 查找第一个引号
    size_t quote_start = str.find(L'\"', start_pos);
    if (quote_start == std::string::npos)
    {
        // 如果没有引号，则提取到下一个空格或行尾
        size_t end_pos = str.find(L' ', start_pos);
        if (end_pos == std::string::npos)
            end_pos = str.length();

        return str.substr(start_pos, end_pos - start_pos);
    }

    // 查找结束引号
    size_t quote_end = str.find(L'\"', quote_start + 1);
    if (quote_end == std::string::npos)
        return str.substr(quote_start + 1);

    return str.substr(quote_start + 1, quote_end - quote_start - 1);
}

//2026-03-05 14:23:45
std::string GetCurrentTime_Seconds()
{
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);

    // 使用线程安全的版本
    struct tm timeinfo;

#ifdef _WIN32
    // Windows使用localtime_s
    localtime_s(&timeinfo, &now_time_t);
#else
    // Linux/Unix使用localtime_r
    localtime_r(&now_time_t, &timeinfo);
#endif

    std::stringstream ss;
    ss << std::put_time(&timeinfo, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

//2026-03-05 14:23:45.678
std::string GetCurrentTime_Milliseconds()
{
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);

    // 获取毫秒部分
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    // 使用线程安全的版本
    struct tm timeinfo;

#ifdef _WIN32
    localtime_s(&timeinfo, &now_time_t);
#else
    localtime_r(&now_time_t, &timeinfo);
#endif

    std::stringstream ss;
    ss << std::put_time(&timeinfo, "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

//1772691825
static uint64_t GetUnixTimestamp_Seconds()
{
    auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
}

//1772691825678
static uint64_t GetUnixTimestamp_Milliseconds()
{
    auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
}

//1772691825678
uint64_t GetTimestamp13()
{
    // 使用system_clock获取自1970-01-01以来的毫秒数
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    return static_cast<uint64_t>(ms.count());
}

//1772691825
uint64_t GetTimestamp10()
{
    // 使用system_clock获取自1970-01-01以来的秒数
    auto now = std::chrono::system_clock::now();
    auto sec = std::chrono::duration_cast<std::chrono::seconds>(
        now.time_since_epoch());
    return static_cast<uint64_t>(sec.count());
}

uint64_t 时间_取现行时间戳(bool sec10)
{
    auto now = std::chrono::system_clock::now();

    if (sec10)
    {
        // 10位时间戳（秒）
        return std::chrono::duration_cast<std::chrono::seconds>(
            now.time_since_epoch()).count();
    }
    else
    {
        // 13位时间戳（毫秒）
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()).count();
    }
}

bool EndsWith(const std::string& s, const std::string& suffix)
{
    if (s.size() < suffix.size())
        return false;

    return s.compare(s.size() - suffix.size(), suffix.size(), suffix) == 0;
}

void* WeixinExe_Offset(uintptr_t offset)
{
    return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(g_hWeixinExe) + offset);
}

void* WeixinDll_Offset(uintptr_t offset)
{
    return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(g_hWeixinDll) + offset);
}

static bool IsReadable(const void* p, size_t len)
{
    MEMORY_BASIC_INFORMATION mbi{};
    if (!VirtualQuery(p, &mbi, sizeof(mbi)))
        return false;

    if (mbi.State != MEM_COMMIT)
        return false;

    if (mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD))
        return false;

    uintptr_t start = (uintptr_t)p;
    uintptr_t end = start + len;
    uintptr_t mbi_end = (uintptr_t)mbi.BaseAddress + mbi.RegionSize;

    return end <= mbi_end;
}

std::string ReadWxString(void* p)
{
    if (!p)
        return "";

    auto s = (WxStringSSO*)p;

    if (s->len == 0 || s->len > 0x400)
        return "";


    // ===== SSO（<= 0xF）=====
    if (s->len <= 0xF)
    {
        wchar_t* inline_buf = (wchar_t*)p; // 直接从对象起始读取
        size_t bytes = s->len;


        if (!IsReadable(s->inline_buf, bytes))
        {
            return "";
        }

        return std::string(s->inline_buf, s->len);
    }
    // ===== Heap（> 0xF）=====
    else
    {

        if (!s->heap_buf)
        {
            return "";
        }
        size_t bytes = s->len;


        if (!IsReadable(s->heap_buf, bytes))
        {
            return "";
        }
        std::string  result = std::string(s->heap_buf, s->len);

        return result;
    }
}

static std::string StringToUtf8(const std::string& str)
{
    return str;
}


std::string 字节集_字节集到十六进制(const std::vector<uint8_t>& data)
{
    std::ostringstream oss;
    for (uint8_t b : data)
    {
        oss << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(b);
    }
    return oss.str();
}

std::vector<uint8_t> 字节集_十六进制到字节集(const std::string& hex)
{
    if (hex.length() % 2 != 0)
        throw std::invalid_argument("Hex string length must be even");

    std::vector<uint8_t> bytes;
    bytes.reserve(hex.length() / 2);

    for (size_t i = 0; i < hex.length(); i += 2)
    {
        auto hex_byte = hex.substr(i, 2);
        uint8_t b = static_cast<uint8_t>(std::stoi(hex_byte, nullptr, 16));
        bytes.push_back(b);
    }

    return bytes;
}

std::vector<uint8_t> 编码_UTF8编码(const std::string& input)
{
    // 空字符串直接返回空 vector
    if (input.empty())
        return {};

    // 将 std::string 的每个字符复制到 uint8_t vector
    std::vector<uint8_t> output(input.begin(), input.end());
    return output;
}

std::vector<uint8_t> 到字节集(const std::string& input)
{
    return 编码_UTF8编码(input);
}

uint64_t 时间_到时间戳(const std::string& datetime) {
    std::tm tm = {};
    std::stringstream ss(datetime);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");

    if (ss.fail()) {
        return 0;
    }

    tm.tm_isdst = -1;
    std::time_t t = std::mktime(&tm);

    if (t == -1) {
        return 0;
    }

    // 使用 gmtime_s 替代 gmtime
    std::tm tm_utc = {};
    errno_t err = gmtime_s(&tm_utc, &t);
    if (err != 0) {
        return 0;
    }

    std::time_t t_utc = std::mktime(&tm_utc);
    if (t_utc == -1) {
        return 0;
    }

    if (t_utc < 0) {
        return 0;
    }

    return static_cast<uint64_t>(t_utc);
}

std::string 时间_时间戳转文本(uint64_t timestamp) {
    std::time_t t = static_cast<std::time_t>(timestamp);
    std::tm tm = {};  // 使用栈变量，不用指针

    // Windows 下使用 gmtime_s
    errno_t err = gmtime_s(&tm, &t);
    if (err != 0) {
        return "";
    }

    char buffer[32] = { 0 };
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm);

    return std::string(buffer);
}

std::string 取运行目录()
{
    char path[MAX_PATH] = { 0 };
    GetModuleFileNameA(NULL, path, MAX_PATH);

    std::string fullPath(path);
    size_t pos = fullPath.find_last_of("\\/");
    if (pos != std::string::npos)
        return fullPath.substr(0, pos);

    return "";
}

bool 文件_是否存在(const std::string& path)
{
    DWORD attr = GetFileAttributesA(path.c_str());
    return (attr != INVALID_FILE_ATTRIBUTES &&
        !(attr & FILE_ATTRIBUTE_DIRECTORY));
}

std::string 编码_BASE64编码(const std::vector<unsigned char>& input)
{
    if (input.empty())
        return "";

    // 防止极端情况下 size_t 溢出
    if (input.size() > (SIZE_MAX / 4) * 3)
        return "";

    static const char table[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

    const size_t len = input.size();
    const size_t outLen = ((len + 2) / 3) * 4;

    std::string output;
    output.resize(outLen);

    size_t i = 0;   // input index
    size_t j = 0;   // output index

    while (i < len)
    {
        unsigned char b1 = input[i++];
        unsigned char b2 = (i < len) ? input[i++] : 0;
        unsigned char b3 = (i < len) ? input[i++] : 0;

        output[j++] = table[b1 >> 2];
        output[j++] = table[((b1 & 0x03) << 4) | (b2 >> 4)];

        if (i - 1 <= len)
            output[j++] = (i - 1 < len + 1)
            ? table[((b2 & 0x0F) << 2) | (b3 >> 6)]
            : '=';
        else
            output[j++] = '=';

        if (i <= len)
            output[j++] = (i <= len)
            ? table[b3 & 0x3F]
            : '=';
        else
            output[j++] = '=';
    }

    // 修正 '=' 填充
    size_t mod = len % 3;
    if (mod > 0)
    {
        output[outLen - 1] = '=';
        if (mod == 1)
            output[outLen - 2] = '=';
    }

    return output;
}

std::vector<unsigned char> 编码_BASE64解码(const std::string& 编码文本, bool 去除右边空白字节集)
{
    // Base64 反查表
    static const int decodeTable[256] = {
        /* 初始化为 -1 */
    };

    static bool initialized = false;
    static int table[256];

    if (!initialized)
    {
        for (int i = 0; i < 256; ++i)
            table[i] = -1;

        const std::string chars =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789+/";

        for (size_t i = 0; i < chars.size(); ++i)
            table[(unsigned char)chars[i]] = (int)i;

        initialized = true;
    }


    std::string clean;
    clean.reserve(编码文本.size());

    for (char c : 编码文本)
    {
        if (!std::isspace((unsigned char)c))
            clean.push_back(c);
    }

    if (clean.empty())
        return {};

    size_t len = clean.size();


    if (len % 4 != 0)
    {
        size_t pad = 4 - (len % 4);
        clean.append(pad, '=');
        len = clean.size();
    }


    size_t outLen = (len / 4) * 3;

    std::vector<unsigned char> output;
    output.reserve(outLen);

    for (size_t i = 0; i < len; i += 4)
    {
        int v1 = table[(unsigned char)clean[i]];
        int v2 = table[(unsigned char)clean[i + 1]];
        int v3 = (clean[i + 2] == '=') ? -1 : table[(unsigned char)clean[i + 2]];
        int v4 = (clean[i + 3] == '=') ? -1 : table[(unsigned char)clean[i + 3]];

        if (v1 < 0 || v2 < 0)
            break;

        unsigned char b1 = (v1 << 2) | (v2 >> 4);
        output.push_back(b1);

        if (v3 >= 0)
        {
            unsigned char b2 = ((v2 & 0x0F) << 4) | (v3 >> 2);
            output.push_back(b2);
        }

        if (v4 >= 0 && v3 >= 0)
        {
            unsigned char b3 = ((v3 & 0x03) << 6) | v4;
            output.push_back(b3);
        }
    }

    if (去除右边空白字节集)
    {
        while (!output.empty() && output.back() == 0x00)
            output.pop_back();
    }

    return output;
}

std::string UTF8到文本(const std::vector<uint8_t>& data)
{
    return std::string((char*)data.data(), data.size());
}


// 十六进制字符串转十进制整数
uint64_t 进制_十六到十(const std::string& hexStr) {
    // 使用strtoull处理十六进制转换，更安全且支持大数
    return std::strtoull(hexStr.c_str(), nullptr, 16);
}

std::string 进制_十到十六(uint64_t value, bool leadingZero) {
    std::stringstream ss;

    if (leadingZero) {
        // 8字节 = 16个十六进制字符
        ss << std::hex << std::setfill('0') << std::setw(16) << value;
    }
    else {
        ss << std::hex << value;
    }

    return ss.str();
}

std::string 取数据摘要(const std::vector<uint8_t>& data) {
    HCRYPTPROV hProv = 0;
    HCRYPTHASH hHash = 0;
    BYTE rgbHash[16];
    DWORD cbHash = 16;
    CHAR rgbDigits[] = "0123456789abcdef";

    // 获取加密服务提供者句柄
    if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
        return "";
    }

    // 创建哈希对象
    if (!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash)) {
        CryptReleaseContext(hProv, 0);
        return "";
    }

    // 使用 static_cast 显式转换
    if (!CryptHashData(hHash, data.data(), static_cast<DWORD>(data.size()), 0)) {
        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);
        return "";
    }

    // 获取哈希结果
    std::string result;
    if (CryptGetHashParam(hHash, HP_HASHVAL, rgbHash, &cbHash, 0)) {
        for (DWORD i = 0; i < cbHash; i++) {
            result += rgbDigits[rgbHash[i] >> 4];
            result += rgbDigits[rgbHash[i] & 0xf];
        }
    }

    // 清理资源
    CryptDestroyHash(hHash);
    CryptReleaseContext(hProv, 0);

    return result;
}

std::string 到小写(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return result;
}

// 删除所有空格
std::string 删全部空(const std::string& str) {
    std::string result;
    std::copy_if(str.begin(), str.end(), std::back_inserter(result),
        [](unsigned char c) { return !std::isspace(c); });
    return result;
}


std::string 取文本左边(const std::string& str, size_t count) {
    if (str.empty() || count == 0) {
        return "";
    }

    // 如果count大于字符串长度，返回整个字符串
    if (count >= str.length()) {
        return str;
    }

    // 返回左边指定长度的子串
    return str.substr(0, count);
}

// 重载版本：支持指定起始位置（类似易语言的取文本中间）
std::string 取文本中间(const std::string& str, size_t start, size_t count) {
    if (str.empty() || start >= str.length() || count == 0) {
        return "";
    }

    size_t actualCount = std::min(count, str.length() - start);
    return str.substr(start, actualCount);
}

// 重载版本：取文本右边
std::string 取文本右边(const std::string& str, size_t count) {
    if (str.empty() || count == 0) {
        return "";
    }

    if (count >= str.length()) {
        return str;
    }

    return str.substr(str.length() - count, count);
}


extern "C" unsigned int __stdcall CalcCpuFeatureMask(unsigned int inputMask);

static std::string 系统_取CPU序列号_Hash() {
    unsigned int mask = CalcCpuFeatureMask(0xFFFFFFFF);

    std::ostringstream oss;
    // 默认输出十进制
    oss << mask;

    return oss.str();
}

// 获取MAC地址
static std::string GetMACAddress() {
    PIP_ADAPTER_INFO pAdapterInfo;
    PIP_ADAPTER_INFO pAdapter = NULL;
    DWORD dwSize = 0;
    std::string macAddress;

    pAdapterInfo = (IP_ADAPTER_INFO*)malloc(sizeof(IP_ADAPTER_INFO));
    if (pAdapterInfo == NULL) {
        return "";
    }

    if (GetAdaptersInfo(pAdapterInfo, &dwSize) == ERROR_BUFFER_OVERFLOW) {
        free(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO*)malloc(dwSize);
        if (pAdapterInfo == NULL) {
            return "";
        }
    }

    if (GetAdaptersInfo(pAdapterInfo, &dwSize) == NO_ERROR) {
        pAdapter = pAdapterInfo;
        while (pAdapter) {
            if (pAdapter->AddressLength == 6) {  // 确保是有效的MAC地址
                char mac[18] = { 0 };
                sprintf_s(mac, sizeof(mac), "%02X%02X%02X%02X%02X%02X",
                    pAdapter->Address[0], pAdapter->Address[1],
                    pAdapter->Address[2], pAdapter->Address[3],
                    pAdapter->Address[4], pAdapter->Address[5]);

                if (!macAddress.empty()) {
                    macAddress += "";
                }
                macAddress += mac;
            }
            pAdapter = pAdapter->Next;
        }
    }

    if (pAdapterInfo) {
        free(pAdapterInfo);
    }

    return macAddress;
}



std::string GetDeviceUUID() {
    // 获取CPU序列号前8位并转换为十进制
    std::string cpuidtxt;
    cpuidtxt = 系统_取CPU序列号_Hash();
    //OutputDebugStringA(("CPU序列号: " + cpuidtxt + "\n").c_str());

    // 获取MAC地址
    std::string mac_all;
    try {
        mac_all = GetMACAddress();
        //OutputDebugStringA(("获取的MAC地址: " + mac_all + "\n").c_str());
    }
    catch (...) {
        mac_all = "000000000000";
    }

    // 计算MAC地址的MD5
    std::string macmd5;
    try {
        macmd5 = 取数据摘要(字节集_十六进制到字节集(mac_all));
        macmd5 = 到小写(删全部空(macmd5));
        //OutputDebugStringA(("MAC地址MD5: " + macmd5 + "\n").c_str());
    }
    catch (...) {
        macmd5 = std::string(32, '0'); // 32个0作为默认值
    }

    // 组合MAC MD5和CPU序列号
    std::string dev_md5 = macmd5 + cpuidtxt;
    //OutputDebugStringA(("组合字符串: " + dev_md5 + "\n").c_str());

    try {
        // 将组合字符串转换为十六进制
        dev_md5 = 字节集_字节集到十六进制(到字节集(dev_md5));

        // 再次计算MD5
        dev_md5 = 取数据摘要(字节集_十六进制到字节集(dev_md5));
        dev_md5 = 到小写(删全部空(dev_md5));
        //OutputDebugStringA(("最终MD5: " + dev_md5 + "\n").c_str());

        // 取前15位
        if (dev_md5.length() >= 15) {
            dev_md5 = dev_md5.substr(0, 15);
        }
        else {
            // 如果长度不足，填充
            dev_md5 += std::string(15 - dev_md5.length(), '0');
        }
    }
    catch (const std::exception& ) {
        dev_md5 = "000000000000000";
    }

    // 添加Windows标志
    dev_md5 = "W" + dev_md5;

    return dev_md5;
}

namespace fs = std::filesystem;

uint64_t 文件_取大小(const std::string& filePath) {
    try {
        // 检查文件是否存在
        if (!fs::exists(filePath)) {
            std::cerr << "文件不存在: " << filePath << std::endl;
            return 0;
        }

        // 检查是否是普通文件
        if (!fs::is_regular_file(filePath)) {
            std::cerr << "不是普通文件: " << filePath << std::endl;
            return 0;
        }

        // 获取文件大小
        uintmax_t size = fs::file_size(filePath);

        // 检查是否超出 uint64_t 的范围
        constexpr uintmax_t maxUint64 = std::numeric_limits<uint64_t>::max();
        if (size > maxUint64) {
            std::cerr << "错误: 文件大小超出 uint64_t 范围" << std::endl;
            return 0;
        }

        return static_cast<uint64_t>(size);
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "文件系统错误: " << e.what() << std::endl;
        return 0;
    }
}

uint64_t get_md5_check_value(const std::string& sign) {
    uint64_t v5 = 0;
    size_t len = sign.length();

    for (size_t i = 0; i < len; i++) {
        char v7 = sign[i];
        uint64_t v6 = (v5 << 5);           // v5 << 5
        uint64_t v8 = v6 - v5 + v7;
        v5 = v8 & 0xFFFFFFFF;              // Keep only lower 32 bits
    }

    return v5;
}


uint64_t 取字节集长度(std::vector<uint8_t> bytes) {
    return bytes.size();
}


std::string 子文本替换(std::string oriText, std::string oldText, std::string newText) {
    std::string result = oriText;
    std::string oldValue = "gh_ffe430e2bbdc";

    size_t pos = result.find(oldText);
    if (pos != std::string::npos) {
        result.replace(pos, oldText.length(), newText);
    }
    return result;

}

// 辅助函数：从URL中解析参数
std::string 网址_取指定参数值(const std::string& url, const std::string& paramName) {
    size_t question_pos = url.find('?');
    if (question_pos == std::string::npos) {
        return "";
    }

    std::string query = url.substr(question_pos + 1);
    std::istringstream iss(query);
    std::string pair;

    while (std::getline(iss, pair, '&')) {
        size_t equal_pos = pair.find('=');
        if (equal_pos != std::string::npos) {
            std::string key = pair.substr(0, equal_pos);
            if (key == paramName) {
                return pair.substr(equal_pos + 1);
            }
        }
    }

    return "";
}


std::string 编码_URL编码(const std::string& 欲编码的文本, bool 不编码字母和数字) {
    std::ostringstream encoded;

    for (unsigned char c : 欲编码的文本) {
        // 检查是否需要编码
        bool shouldEncode = true;

        if (不编码字母和数字) {
            // 字母和数字不编码
            if (std::isalnum(c)) {
                shouldEncode = false;
            }
        }
        else {
            // 根据URL编码规范，以下字符不编码：字母、数字、- _ . ~
            if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
                shouldEncode = false;
            }
        }

        if (shouldEncode) {
            // 需要编码的字符转换为 %XX 格式
            encoded << '%' << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(c);
        }
        else {
            // 不编码的字符直接输出
            encoded << c;
        }
    }

    return encoded.str();
}


uint64_t 到整数(std::string value) {
    return std::stoull(value);

}

std::string 到文本(uint64_t value) {
    return std::to_string(value);

}

// Helper function: 右移 (right shift)
uint64_t 右移(uint64_t value, int bits) {
    return value >> bits;
}


template<typename T>
T 取绝对值(T value) {
    return std::abs(value);
}
