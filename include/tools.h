#pragma once
#include <cstdint>
#include <string>
#include <vector>

 
#pragma pack(push, 1)
struct WxStringSSO
{
    union
    {
        char* heap_buf;        // len > 0xF
        char  inline_buf[16];   // len <= 0xF（最多 16 char）
    };
    //uint64_t unk;   // 0x08 未知字段
    uint64_t len;   // 0x10
    uint64_t cap;   // 0x18
};
#pragma pack(pop)

std::string format_string(const char* format, ...);
std::wstring StringToWString(const std::string& str);
std::string WStringToString(const std::wstring& wstr);
std::string ExtractQuotedString(const std::string& str, size_t start_pos);
void InitStandardPaths();

// wechat base + offset
void* WeixinDll_Offset(uintptr_t offset);
void* WeixinExe_Offset(uintptr_t offset);

static bool IsReadable(const void* p, size_t len);
std::string ReadWxString(void* p);
static std::string StringToUtf8(const std::string& str);
bool EndsWith(const std::string& s, const std::string& suffix);

std::string GetCurrentTime_Seconds();
std::string GetCurrentTime_Milliseconds();
uint64_t 时间_取现行时间戳(bool sec10 = false);
uint64_t GetTimestamp13();
uint64_t GetTimestamp10();

std::string 字节集_字节集到十六进制(const std::vector<uint8_t>& data);
std::vector<uint8_t> 字节集_十六进制到字节集(const std::string& hex);
std::vector<uint8_t> 编码_UTF8编码(const std::string& input);
uint64_t 时间_到时间戳(const std::string& datetime);
std::string 时间_时间戳转文本(uint64_t timestamp);
std::string 取运行目录();
bool 文件_是否存在(const std::string& path);
std::vector<uint8_t> 到字节集(const std::string& input);
std::string 编码_BASE64编码(const std::vector<unsigned char>& input);
std::vector<unsigned char> 编码_BASE64解码(const std::string& 编码文本, bool 去除右边空白字节集 = false);
std::string UTF8到文本(const std::vector<uint8_t>& data);
uint64_t 进制_十六到十(const std::string& hexStr);
std::string 进制_十到十六(uint64_t value, bool leadingZero = false);
std::string 取数据摘要(const std::vector<uint8_t>& data);
std::string 到小写(const std::string& str);
std::string 删全部空(const std::string& str);

std::string 取文本左边(const std::string& str, size_t count);
std::string 取文本中间(const std::string& str, size_t start, size_t count);
std::string 取文本右边(const std::string& str, size_t count);
std::string GetDeviceUUID();
uint64_t 文件_取大小(const std::string& filePath);
uint64_t get_md5_check_value(const std::string& sign);
uint64_t 取字节集长度(std::vector<uint8_t> bytes);
std::string 子文本替换(std::string oriText, std::string oldText, std::string newText);
std::string 网址_取指定参数值(const std::string& url, const std::string& paramName);
std::string 编码_URL编码(const std::string& 欲编码的文本, bool 不编码字母和数字);
uint64_t 到整数(std::string value);
std::string 到文本(uint64_t value);
uint64_t 右移(uint64_t value, int bits);
