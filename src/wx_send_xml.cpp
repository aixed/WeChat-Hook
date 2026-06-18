#include "wx_send_xml.h"
#include <windows.h>
#include <memory>
#include <cstring>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <vector>
#include "global.h"

// Windows API类型定义
typedef int(__fastcall* WeixinCall)(...);

namespace WeixinSendXML
{
    // ===== 全局变量 =====
    static uintptr_t g_weixinBase = 0;
    static void* g_processHandle = nullptr;
    static bool g_initialized = false;

    // ===== 内存操作辅助函数 =====
    namespace Memory
    {
        // 分配内存（在当前进程）
        inline void* Allocate(size_t size)
        {
            return VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
        }

        // 释放内存
        inline void Free(void* ptr)
        {
            if (ptr) VirtualFree(ptr, 0, MEM_RELEASE);
        }

        // 写入内存
        template<typename T>
        inline void Write(void* address, T value)
        {
            if (address) {
                *reinterpret_cast<T*>(address) = value;
            }
        }

        // 写入字节集
        inline void WriteBytes(void* address, const void* data, size_t size)
        {
            if (address && data) {
                memcpy(address, data, size);
            }
        }

        // 写入字符串
        inline void WriteString(void* address, const std::string& str)
        {
            if (address) {
                memcpy(address, str.c_str(), str.length() + 1);
            }
        }
    }

    // ===== 字符串处理函数 =====
    namespace StringUtils
    {
        // 提取中间文本
        std::string ExtractBetween(const std::string& source,
            const std::string& start,
            const std::string& end)
        {
            size_t startPos = source.find(start);
            if (startPos == std::string::npos) return "";

            startPos += start.length();
            size_t endPos = source.find(end, startPos);
            if (endPos == std::string::npos) return "";

            return source.substr(startPos, endPos - startPos);
        }

        // 十进制转十六进制字符串
        std::string DecToHex(uint64_t value, bool prefix = false)
        {
            std::stringstream ss;
            if (prefix) ss << "0x";
            ss << std::hex << std::uppercase << value;
            return ss.str();
        }

        // 十六进制字符串转十进制
        uint64_t HexToDec(const std::string& hex)
        {
            uint64_t value;
            std::stringstream ss;
            ss << std::hex << hex;
            ss >> value;
            return value;
        }

        // UTF-8编码转换（简化版）
        std::string AnsiToUtf8(const std::string& ansi)
        {
            // 简化处理：实际需要Windows API转换
            return ansi; // 这里假设已经是UTF-8
        }
    }


    // ===== 初始化函数 =====
    void Initialize()
    {
        if (!g_weixinBase) {
            g_weixinBase = reinterpret_cast<uintptr_t>(GetModuleHandleA("Weixin.dll"));
        }
        g_processHandle = GetCurrentProcess(); // DLL中为当前进程
        g_initialized = (g_weixinBase != 0);
    }

    void SetWeixinBase(uintptr_t base)
    {
        g_weixinBase = base;
    }

    void SetProcessHandle(void* handle)
    {
        g_processHandle = handle;
    }

    // ===== XML字段提取 =====
    XmlFields ExtractXmlFields(const std::string& xml, XmlType xmlType)
    {
        XmlFields fields;
        fields.Clear();

        using namespace StringUtils;

        switch (xmlType) {
        case XmlType::IMAGE:
            fields.cdnbigurl = ExtractBetween(xml, "cdnbigimgurl=\"", "\"");
            fields.cdnurl = ExtractBetween(xml, "cdnthumburl=\"", "\"");
            fields.aeskey = ExtractBetween(xml, "aeskey=\"", "\"");
            fields.md5 = ExtractBetween(xml, "md5=\"", "\"");
            fields.hdlength = std::stoi(ExtractBetween(xml, "hdlength=\"", "\""));
            fields.length = std::stoi(ExtractBetween(xml, "length=\"", "\""));
            fields.hevc_mid_size = std::stoi(ExtractBetween(xml, "hevc_mid_size=\"", "\""));
            fields.cdnthumblength = std::stoi(ExtractBetween(xml, "cdnthumblength=\"", "\""));
            fields.cdnthumbwidth = std::stoi(ExtractBetween(xml, "cdnthumbwidth=\"", "\""));
            fields.cdnthumbheight = std::stoi(ExtractBetween(xml, "cdnthumbheight=\"", "\""));
            break;

        case XmlType::VIDEO:
            fields.cdnurl = ExtractBetween(xml, "cdnthumburl=\"", "\"");
            fields.aeskey = ExtractBetween(xml, "aeskey=\"", "\"");
            fields.md5 = ExtractBetween(xml, "md5=\"", "\"");
            fields.playlength = std::stoi(ExtractBetween(xml, "playlength=\"", "\""));
            fields.length = std::stoi(ExtractBetween(xml, "length=\"", "\""));
            fields.cdnthumblength = std::stoi(ExtractBetween(xml, "cdnthumblength=\"", "\""));
            fields.cdnthumbwidth = std::stoi(ExtractBetween(xml, "cdnthumbwidth=\"", "\""));
            fields.cdnthumbheight = std::stoi(ExtractBetween(xml, "cdnthumbheight=\"", "\""));
            break;

        case XmlType::ANIMATION:
            fields.md5 = ExtractBetween(xml, "md5=\"", "\"");
            fields.length = std::stoi(ExtractBetween(xml, "len=\"", "\""));
            fields.type = std::stoi(ExtractBetween(xml, "type=\"", "\""));
            fields.width = std::stoi(ExtractBetween(xml, "width=\"", "\""));
            fields.height = std::stoi(ExtractBetween(xml, "height=\"", "\""));
            fields.productid = ExtractBetween(xml, "productid=\"", "\"");
            break;

        default:
            break;
        }

        return fields;
    }

    // ===== 链表结构构建 =====
    struct ListNode
    {
        void* next;
        void* prev;
        uint8_t data[160 - 16]; // 160字节减去两个指针的大小
    };

    void* BuildLinkedList(size_t count = 10)
    {
        // 分配链表头部 + 所有节点
        size_t nodeSize = 0xA0;   // 每个节点160字节（0xA0）
        size_t totalSize = nodeSize * count;

        void* memory = Memory::Allocate(totalSize);
        if (!memory) return nullptr;

        // 计算各个部分的地址
        uintptr_t headerAddr = reinterpret_cast<uintptr_t>(memory);
        uintptr_t firstNodeAddr = headerAddr + nodeSize;
        uintptr_t lastNodeAddr = firstNodeAddr + nodeSize * (count - 1);

        // 写入链表头部
        Memory::Write<uint64_t>(reinterpret_cast<void*>(headerAddr + 0x0), firstNodeAddr);
        Memory::Write<uint64_t>(reinterpret_cast<void*>(headerAddr + 0x8), lastNodeAddr);
        Memory::Write<uint64_t>(reinterpret_cast<void*>(headerAddr + 0x10), 0xABABABABABABABAB);

        // 构建链表，最后一个节点指向头部而不是第一个节点
        for (size_t i = 0; i < count; i++) {
            uintptr_t currentNodeAddr = firstNodeAddr + nodeSize * i;

            // 对于双向循环链表，计算前一个和后一个节点
            uintptr_t prevNodeAddr, nextNodeAddr;

            if (i == 0) {
                // 第一个节点：prev 指向头部
                prevNodeAddr = headerAddr;
                nextNodeAddr = currentNodeAddr + nodeSize;
            }
            else if (i == count - 1) {
                // 最后一个节点：next 指向头部
                prevNodeAddr = currentNodeAddr - nodeSize;
                nextNodeAddr = headerAddr;
            }
            else {
                // 中间节点
                prevNodeAddr = currentNodeAddr - nodeSize;
                nextNodeAddr = currentNodeAddr + nodeSize;
            }

            // 写入节点指针
            Memory::Write<uint64_t>(reinterpret_cast<void*>(currentNodeAddr + 0x0), nextNodeAddr);   // next
            Memory::Write<uint64_t>(reinterpret_cast<void*>(currentNodeAddr + 0x8), prevNodeAddr);   // prev

            // 设置第5个节点的特殊数据（从0开始计数，所以是i=4）
            if (i == 4) {
                Memory::Write<uint64_t>(reinterpret_cast<void*>(currentNodeAddr + 0x10), 0x6469727673); // "svrid"
                Memory::Write<uint64_t>(reinterpret_cast<void*>(currentNodeAddr + 0x20), 5);
                Memory::Write<uint64_t>(reinterpret_cast<void*>(currentNodeAddr + 0x28), 0xF);
            }
        }

        return memory; // 返回链表头部地址
    }

    // ===== 图片结构构建 =====
    void* BuildImageStructure(const XmlFields& fields)
    {
        // 分配XML字段结构内存
        void* xmlFieldPtr = Memory::Allocate(0x400); // 1KB
        if (!xmlFieldPtr) return nullptr;

        // 分配字符串内存并写入
        auto allocAndWrite = [&](const std::string& str) -> void* {
            if (str.empty()) return nullptr;
            void* ptr = Memory::Allocate(str.length() + 1);
            Memory::WriteString(ptr, str);
            return ptr;
            };

        void* md5Ptr = allocAndWrite(fields.md5);
        void* aeskeyPtr1 = allocAndWrite(fields.aeskey);
        void* aeskeyPtr2 = allocAndWrite(fields.aeskey);
        void* cdnbigurlPtr = allocAndWrite(fields.cdnbigurl);
        void* cdnurlPtr1 = allocAndWrite(fields.cdnurl);
        void* cdnurlPtr2 = allocAndWrite(fields.cdnurl);

        // 填充字段结构
        uintptr_t base = reinterpret_cast<uintptr_t>(xmlFieldPtr);

        // 写入字符串指针和长度
        if (aeskeyPtr1) {
            Memory::Write<void*>(reinterpret_cast<void*>(base + 0x170), aeskeyPtr1);
            Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x180), fields.aeskey.length());
            Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x188), fields.aeskey.length());
        }

        if (md5Ptr) {
            Memory::Write<void*>(reinterpret_cast<void*>(base + 0x1B0), md5Ptr);
            Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x1C0), fields.md5.length());
            Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x1C8), fields.md5.length());
        }

        if (cdnbigurlPtr && !fields.cdnbigurl.empty()) {
            Memory::Write<void*>(reinterpret_cast<void*>(base + 0x1D0), cdnbigurlPtr);
            Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x1E0), fields.cdnbigurl.length());
            Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x1E8), fields.cdnbigurl.length());
            Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x1F0), static_cast<uint64_t>(fields.hdlength));
        }

        if (cdnurlPtr1) {
            Memory::Write<void*>(reinterpret_cast<void*>(base + 0x1F8), cdnurlPtr1);
            Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x208), fields.cdnurl.length());
            Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x210), fields.cdnurl.length());
        }

        Memory::Write<uint32_t>(reinterpret_cast<void*>(base + 0x218), fields.length);
        Memory::Write<uint32_t>(reinterpret_cast<void*>(base + 0x21C), fields.hevc_mid_size);

        if (cdnurlPtr2) {
            Memory::Write<void*>(reinterpret_cast<void*>(base + 0x220), cdnurlPtr2);
            Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x230), fields.cdnurl.length());
            Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x238), fields.cdnurl.length());
        }

        if (aeskeyPtr2) {
            Memory::Write<void*>(reinterpret_cast<void*>(base + 0x240), aeskeyPtr2);
            Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x250), fields.aeskey.length());
            Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x258), fields.aeskey.length());
        }

        Memory::Write<uint32_t>(reinterpret_cast<void*>(base + 0x260), fields.cdnthumblength);
        Memory::Write<uint32_t>(reinterpret_cast<void*>(base + 0x264), fields.cdnthumbwidth);
        Memory::Write<uint32_t>(reinterpret_cast<void*>(base + 0x268), fields.cdnthumbheight);

        // 构建链表
        void* listHead = BuildLinkedList();
        if (listHead) {
            Memory::Write<void*>(reinterpret_cast<void*>(base + 0x30), listHead);
        }

        // 写入虚表指针等固定值
        Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x0), g_weixinBase + Offsets::IMAGE_FIELD_VTABLE);
        Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x8), 0x100000006);
        Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x10), g_weixinBase + Offsets::IMAGE_FIELD_VTABLE2);
        Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x18), 1);
        Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x20), 0);
        Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x28), 0x3F800000);
        Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x38), 0xA);

        return xmlFieldPtr;
    }

    // ===== 视频结构构建 =====
    void* BuildVideoStructure(const XmlFields& fields)
    {
        void* xmlFieldPtr = Memory::Allocate(0x400);
        if (!xmlFieldPtr) return nullptr;

        // 分配字符串内存
        auto allocAndWrite = [&](const std::string& str) -> void* {
            if (str.empty()) return nullptr;
            void* ptr = Memory::Allocate(str.length() + 1);
            Memory::WriteString(ptr, str);
            return ptr;
            };

        void* md5Ptr = allocAndWrite(fields.md5);
        void* aeskeyPtr1 = allocAndWrite(fields.aeskey);
        void* aeskeyPtr2 = allocAndWrite(fields.aeskey);
        void* cdnurlPtr1 = allocAndWrite(fields.cdnurl);
        void* cdnurlPtr2 = allocAndWrite(fields.cdnurl);

        uintptr_t base = reinterpret_cast<uintptr_t>(xmlFieldPtr);

        // 填充字段
        if (aeskeyPtr1) {
            Memory::Write<void*>(reinterpret_cast<void*>(base + 0x170), aeskeyPtr1);
            Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x180), fields.aeskey.length());
            Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x188), fields.aeskey.length());
        }

        if (md5Ptr) {
            Memory::Write<void*>(reinterpret_cast<void*>(base + 0x1A0), md5Ptr);
            Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x1B0), fields.md5.length());
            Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x1B8), fields.md5.length());
        }

        Memory::Write<uint32_t>(reinterpret_cast<void*>(base + 0x190), fields.playlength);
        Memory::Write<uint32_t>(reinterpret_cast<void*>(base + 0x198), 1);
        Memory::Write<uint32_t>(reinterpret_cast<void*>(base + 0x1E0), fields.length);

        if (cdnurlPtr1) {
            Memory::Write<void*>(reinterpret_cast<void*>(base + 0x1E8), cdnurlPtr1);
            Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x1F8), fields.cdnurl.length());
            Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x200), fields.cdnurl.length());
        }

        if (cdnurlPtr2) {
            Memory::Write<void*>(reinterpret_cast<void*>(base + 0x270), cdnurlPtr2);
            Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x280), fields.cdnurl.length());
            Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x288), fields.cdnurl.length());
        }

        if (aeskeyPtr2) {
            Memory::Write<void*>(reinterpret_cast<void*>(base + 0x290), aeskeyPtr2);
            Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x2A0), fields.aeskey.length());
            Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x2A8), fields.aeskey.length());
        }

        Memory::Write<uint32_t>(reinterpret_cast<void*>(base + 0x2B0), fields.cdnthumblength);
        Memory::Write<uint32_t>(reinterpret_cast<void*>(base + 0x2B4), fields.cdnthumbwidth);
        Memory::Write<uint32_t>(reinterpret_cast<void*>(base + 0x2B8), fields.cdnthumbheight);

        // 构建链表
        void* listHead = BuildLinkedList();
        if (listHead) {
            Memory::Write<void*>(reinterpret_cast<void*>(base + 0x30), listHead);
        }

        // 写入虚表指针
        Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x0), g_weixinBase + Offsets::VIDEO_FIELD_VTABLE);
        Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x8), 0x100000006);
        Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x10), g_weixinBase + Offsets::VIDEO_FIELD_VTABLE2);
        Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x18), 1);
        Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x20), 0);
        Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x28), 0x3F800000);
        Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x38), 0xA);

        return xmlFieldPtr;
    }

    // ===== 动图结构构建 =====
    void* BuildAnimationStructure(const XmlFields& fields)
    {
        void* xmlFieldPtr = Memory::Allocate(0x400);
        if (!xmlFieldPtr) return nullptr;

        // 分配字符串内存
        void* md5Ptr = nullptr;
        void* productidPtr = nullptr;

        if (!fields.md5.empty()) {
            md5Ptr = Memory::Allocate(fields.md5.length() + 1);
            Memory::WriteString(md5Ptr, fields.md5);
        }

        if (!fields.productid.empty()) {
            std::string utf8ProductId = StringUtils::AnsiToUtf8(fields.productid);
            productidPtr = Memory::Allocate(utf8ProductId.length() + 1);
            Memory::WriteString(productidPtr, utf8ProductId);
        }

        uintptr_t base = reinterpret_cast<uintptr_t>(xmlFieldPtr);

        // 填充字段
        if (md5Ptr) {
            Memory::Write<void*>(reinterpret_cast<void*>(base + 0x198), md5Ptr);
            Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x1A8), fields.md5.length());
            Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x1B0), fields.md5.length());
        }

        Memory::Write<uint32_t>(reinterpret_cast<void*>(base + 0x190), fields.length);
        Memory::Write<uint32_t>(reinterpret_cast<void*>(base + 0x258), fields.type);
        Memory::Write<uint32_t>(reinterpret_cast<void*>(base + 0x25C), fields.width);
        Memory::Write<uint32_t>(reinterpret_cast<void*>(base + 0x260), fields.height);

        if (productidPtr) {
            Memory::Write<void*>(reinterpret_cast<void*>(base + 0x1B8), productidPtr);
            size_t productidLen = fields.productid.length();
            Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x1C8), productidLen);
            Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x1D0), productidLen);
        }

        // 构建链表
        void* listHead = BuildLinkedList();
        if (listHead) {
            Memory::Write<void*>(reinterpret_cast<void*>(base + 0x30), listHead);
        }

        // 写入虚表指针
        Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x0), g_weixinBase + Offsets::ANIMATION_FIELD_VTABLE);
        Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x8), 0x100000006);
        Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x10), g_weixinBase + Offsets::ANIMATION_FIELD_VTABLE2);
        Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x18), 1);
        Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x20), 0);
        Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x28), 0x3F800000);
        Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x38), 0xA);

        return xmlFieldPtr;
    }

    // ===== 构建XML数据结构 =====
    void* BuildXmlDataStructure(XmlType xmlType)
    {
        void* xmlDataPtr = Memory::Allocate(0x100);
        if (!xmlDataPtr) return nullptr;

        uintptr_t base = reinterpret_cast<uintptr_t>(xmlDataPtr);

        switch (xmlType) {
        case XmlType::IMAGE:
            Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x0), g_weixinBase + Offsets::IMAGE_DATA_VTABLE);
            Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x8), 0x100000005);
            Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x10), g_weixinBase + Offsets::IMAGE_DATA_VTABLE2);
            Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x18), 0x300000064);
            break;

        case XmlType::VIDEO:
            Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x0), g_weixinBase + Offsets::IMAGE_DATA_VTABLE);
            Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x8), 0x100000005);
            Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x10), g_weixinBase + Offsets::IMAGE_DATA_VTABLE2);
            Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x18), 0x2B00000064);
            break;

        case XmlType::ANIMATION:
            Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x0), g_weixinBase + Offsets::IMAGE_DATA_VTABLE);
            Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x8), 0x100000005);
            Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x10), g_weixinBase + Offsets::IMAGE_DATA_VTABLE2);
            Memory::Write<uint64_t>(reinterpret_cast<void*>(base + 0x18), 0x2F00000000);
            break;

        default:
            Memory::Free(xmlDataPtr);
            return nullptr;
        }

        return xmlDataPtr;
    }

    // ===== 主转发函数 =====
    bool ForwardXmlMessage(const std::string& wxid, const std::string& xml)
    {
        // 初始化检查
        if (!g_initialized) {
            Initialize();
            if (!g_initialized) return false;
        }

        // 检查wxid和xml
        if (wxid.empty() || xml.empty()) {
            return false;
        }

        // 判断XML类型
        XmlType xmlType = XmlType::OTHER;
        if (xml.find("<img ") != std::string::npos) {
            xmlType = XmlType::IMAGE;
        }
        else if (xml.find("<videomsg ") != std::string::npos) {
            xmlType = XmlType::VIDEO;
        }
        else if (xml.find("<emoji ") != std::string::npos) {
            xmlType = XmlType::ANIMATION;
        }

        if (xmlType == XmlType::OTHER) {
            // 不支持的类型
            return false;
        }

        // 提取XML字段
        XmlFields fields = ExtractXmlFields(xml, xmlType);

        // 构建XML字段结构
        void* xmlFieldPtr = nullptr;
        void* xmlDataPtr = nullptr;

        switch (xmlType) {
        case XmlType::IMAGE:
            xmlFieldPtr = BuildImageStructure(fields);
            break;
        case XmlType::VIDEO:
            xmlFieldPtr = BuildVideoStructure(fields);
            xmlDataPtr = BuildXmlDataStructure(xmlType);
            break;
        case XmlType::ANIMATION:
            xmlFieldPtr = BuildAnimationStructure(fields);
            xmlDataPtr = BuildXmlDataStructure(xmlType);
            break;
        default:
            return false;
        }

        if (!xmlFieldPtr) {
            return false;
        }

        // 构建消息结构
        void* messageStruct = Memory::Allocate(0x400);
        if (!messageStruct) {
            Memory::Free(xmlFieldPtr);
            if (xmlDataPtr) Memory::Free(xmlDataPtr);
            return false;
        }

        // 分配wxid字符串内存
        void* wxidPtr = Memory::Allocate(wxid.length() + 1);
        if (!wxidPtr) {
            Memory::Free(messageStruct);
            Memory::Free(xmlFieldPtr);
            if (xmlDataPtr) Memory::Free(xmlDataPtr);
            return false;
        }
        Memory::WriteString(wxidPtr, wxid);

        uintptr_t msgBase = reinterpret_cast<uintptr_t>(messageStruct);

        // 填充消息结构
        Memory::Write<uint64_t>(reinterpret_cast<void*>(msgBase + 0x0), g_weixinBase + Offsets::MESSAGE_STRUCT_VTABLE);
        Memory::Write<uint64_t>(reinterpret_cast<void*>(msgBase + 0x8), 0x100000003);
        Memory::Write<uint64_t>(reinterpret_cast<void*>(msgBase + 0x10), g_weixinBase + Offsets::MESSAGE_STRUCT_VTABLE2);

        // 设置消息类型
        uint64_t msgType = 0;
        switch (xmlType) {
        case XmlType::IMAGE: msgType = 0x3; break;
        case XmlType::VIDEO: msgType = 0x2B; break;
        case XmlType::ANIMATION: msgType = 0x2F; break;
        default: msgType = 0x31; break;
        }
        Memory::Write<uint64_t>(reinterpret_cast<void*>(msgBase + 0xD8), msgType);

        // 设置wxid
        Memory::Write<void*>(reinterpret_cast<void*>(msgBase + 0xA8), wxidPtr);
        Memory::Write<uint64_t>(reinterpret_cast<void*>(msgBase + 0xB8), wxid.length());
        Memory::Write<uint64_t>(reinterpret_cast<void*>(msgBase + 0xC0), wxid.length());

        // 设置XML字段
        Memory::Write<void*>(reinterpret_cast<void*>(msgBase + 0x318), reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(xmlFieldPtr) + 0x10));
        Memory::Write<void*>(reinterpret_cast<void*>(msgBase + 0x320), xmlFieldPtr);

        // 设置XML数据
        if (xmlDataPtr) {
            Memory::Write<void*>(reinterpret_cast<void*>(msgBase + 0x340), reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(xmlDataPtr) + 0x10));
            Memory::Write<void*>(reinterpret_cast<void*>(msgBase + 0x348), xmlDataPtr);
        }

        // 构建列表结构
        void* listPtr = Memory::Allocate(0x20);
        if (listPtr) {
            Memory::Write<void*>(listPtr, reinterpret_cast<void*>(msgBase + 0x10));
            Memory::Write<void*>(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(listPtr) + 8), messageStruct);
        }

        // 构建参数结构
        void* paramPtr = Memory::Allocate(0x28);
        if (paramPtr) {
            Memory::Write<uint64_t>(paramPtr, g_weixinBase + Offsets::MESSAGE_PARAM_VTABLE);
            Memory::Write<void*>(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(paramPtr) + 8), listPtr);
            Memory::Write<void*>(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(paramPtr) + 0x10), reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(listPtr) + 0x10));
            Memory::Write<void*>(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(paramPtr) + 0x18), reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(listPtr) + 0x10));
        }

        // 构建调用参数
        void* rcx = Memory::Allocate(0x40);
        void* rdx = Memory::Allocate(0x100);
        uint64_t r8 = (uint64_t)rdx + 0xC0;

        if (rcx && rdx) {
            // 设置rcx参数
            Memory::Write<uint64_t>(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(rcx) + 0x38), reinterpret_cast<uintptr_t>(paramPtr));

            // 设置rdx参数
            Memory::Write<uint64_t>(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(rdx) + 0x20), g_weixinBase + 0x367849);

            // 调用转发函数
            WeixinCall forwardCall = (WeixinCall)(g_weixinBase + Offsets::FORWARD_XML_CALL);


            forwardCall((uint64_t)rcx, (uint64_t)rdx, (uint64_t)r8);

        }

        // 清理内存 这里清理的话会崩溃
        //if (rcx) Memory::Free(rcx);
        //if (rdx) Memory::Free(rdx);
        //if (paramPtr) Memory::Free(paramPtr);
        //if (listPtr) Memory::Free(listPtr);
        //if (wxidPtr) Memory::Free(wxidPtr);
        //if (xmlDataPtr) Memory::Free(xmlDataPtr);
        //if (xmlFieldPtr) Memory::Free(xmlFieldPtr);
        //if (messageStruct) Memory::Free(messageStruct);

        return true;
    }
}