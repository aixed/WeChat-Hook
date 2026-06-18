#include "wx_send.h"
#include <windows.h>
#include <string>
#include <cstddef>
#include <cstring>
#include <objbase.h>
#include "global.h"
#include "tools.h"

using WeixinCall = __int64(*)(...);


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


namespace WeixinSend
{
    // ============================================================
    // util
    // ============================================================
    uintptr_t GetWeixinDllBase()
    {
        static uintptr_t base = 0;
        if (!base)
            base = (uintptr_t)GetModuleHandleA("Weixin.dll");
        return base;
    }

    static std::string GenGuidString()
    {
        GUID guid;
        CoCreateGuid(&guid);

        char buf[64] = { 0 };
        // 36 chars, no braces
        snprintf(buf, sizeof(buf),
            "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
            guid.Data1,
            guid.Data2,
            guid.Data3,
            guid.Data4[0], guid.Data4[1],
            guid.Data4[2], guid.Data4[3],
            guid.Data4[4], guid.Data4[5],
            guid.Data4[6], guid.Data4[7]
        );

        return std::string(buf);
    }

    static uint64_t CalcWxCapacity(uint64_t len)
    {
        uint64_t need = len + 1;                
        uint64_t cap = (need + 0xF) & ~0xFULL;  
        return cap - 1;                           // 微信风格
    }


    template<typename T>
    T* HeapAlloc_mb(size_t count)
    {
        return (T*)::HeapAlloc(
            ::GetProcessHeap(),
            HEAP_ZERO_MEMORY,
            sizeof(T) * count
        );
    }


#pragma pack(push, 1)

    struct WeixinStringUnicode
    {
        union
        {
            wchar_t inline_buf[8];  
            wchar_t* heap_buf;
        };
        uint64_t length;  
        uint64_t cap;    
    };

    struct WeixinString
    {
        union
        {
            char  inline_buf[16];
            char* heap_buf;
        };
        uint64_t length;
        uint64_t cap; 
    };
    struct TextMessage
    {
        uint64_t pad0[22] = { 0 };          // 0x000
        WeixinString receiver;              // 0x0B0
        uint64_t pad1[1] = { 0 };           // 0x0D0
        uint64_t type;                      // 0x0D8
        uint64_t pad2[21] = { 0 };          // 0x0E0
        uint64_t msg_len;                   // 0x188
        uint64_t pad3[163] = { 0 };         // 0x190
        WeixinString uuid;                  // 0x6A8
        uint64_t pad4[8] = { 0 };           // 0x6C8
        WeixinString content;               // 0x708
        WeixinString atlist;                // 0x728
    };

    static_assert(sizeof(WeixinString) == 0x20);
    static_assert(offsetof(TextMessage, receiver) == 0x0B0);
    static_assert(offsetof(TextMessage, uuid) == 0x6A8);
    static_assert(offsetof(TextMessage, content) == 0x708);

    struct UnknownBlock
    {
        uint64_t vtable;
        uint64_t temp[6];
        uint64_t self;
    };

    struct InnerStruct1
    {
        void* ptr1;      // +0x00
        void* ptr2;      // +0x08
        void* ptr3;      // +0x10
        uint64_t count;  // +0x18
    };
    struct InnerStruct2
    {
        void* vtable;    // +0x00
        void* ptr1;      // +0x08
        uint64_t data1;  // +0x10
        uint64_t data2;  // +0x18
        uint64_t field1; // +0x20
    };


#pragma pack(pop)

    // 新增: 设置 Unicode 字符串
    inline void SetWeixinStringU(WeixinStringUnicode* ws, const std::string& s)
    {
        memset(ws, 0, sizeof(WeixinStringUnicode));

        // 将 std::string (UTF-8) 转换为 std::wstring (Unicode)
        int size = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, nullptr, 0);
        std::wstring wstr(size - 1, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, &wstr[0], size);

        ws->length = wstr.size();  // 设置长度
        if (wstr.size() < 8)
        {
            // 短字符串: 使用内联缓冲区
            memcpy(ws->inline_buf, wstr.data(), wstr.size() * sizeof(wchar_t));
            ws->inline_buf[wstr.size()] = L'\0';
            ws->cap = 0xF;
        }
        else
        {
            // 长字符串: 堆分配
            wchar_t* buf = (wchar_t*)::HeapAlloc(
                ::GetProcessHeap(),
                HEAP_ZERO_MEMORY,
                (wstr.size() + 1) * sizeof(wchar_t)
            );
            memcpy(buf, wstr.data(), (wstr.size() + 1) * sizeof(wchar_t));
            ws->heap_buf = buf;
            ws->cap = CalcWxCapacity(wstr.size());
        }
    }

    inline void SetWeixinString(WeixinString* dst, const std::string& src)
    {
        // 必须先清零
        memset(dst, 0, sizeof(WeixinString));

        dst->length = src.size();

        if (src.size() < 16)
        {
            // inline 模式
            memcpy(dst->inline_buf, src.data(), src.size());
            dst->cap = 0xF;
        }
        else
        {
            // heap 模式
            char* buf = (char*)::HeapAlloc(::GetProcessHeap(), HEAP_ZERO_MEMORY, src.size() + 1);
            //char* buf = (char*)Memory::Allocate(s.size() + 1);
            if (!buf) {
                Memory::Free(buf);
                return;
            }

            memcpy(buf, src.data(), src.size());
            buf[src.size()] = '\0';
            dst->heap_buf = buf;
            dst->cap = CalcWxCapacity(src.size());

        }
    }



    void BuildSendParam1_sub(uint64_t* msgBuf, const std::string& wxid, const std::string& imgPath)
    {
        uint64_t filesize = 文件_取大小(imgPath);
        std::string guid = GenGuidString();

        msgBuf[0] = (uintptr_t)g_hWeixinDll + offset::img_msg_vtbl;
        msgBuf[1] = 0x200000006;
        msgBuf[2] = (uintptr_t)g_hWeixinDll + offset::img_msg_vtb2;


        *(uint64_t*)(msgBuf + 0x40 / 8) = 0xF;
        *(uint64_t*)(msgBuf + 0x60 / 8) = 0xF;
        *(uint64_t*)(msgBuf + 0x80 / 8) = 0xF;
        *(uint64_t*)(msgBuf + 0xA0 / 8) = 0xF;
        *(uint64_t*)(msgBuf + 0xA8 / 8) = 0x0000000100000000;
        *(uint64_t*)(msgBuf + 0xB0 / 8) = 0x0000000700000000;

        // === wxid ===
        SetWeixinString((WeixinString*)(msgBuf + 0xC0 / 8), wxid);
        *(uint64_t*)(msgBuf + 0xD0 / 8) = wxid.size();
        *(uint64_t*)(msgBuf + 0xD8 / 8) = CalcWxCapacity(wxid.size());

        //文件类型 3= png图片   47 = gif
        *(uint64_t*)(msgBuf + 0xE8 / 8) = 3;        


        // === 图片路径 ===
        SetWeixinStringU((WeixinStringUnicode*)(msgBuf + 0xF0 / 8), imgPath);
        *(uint64_t*)(msgBuf + 0x100 / 8) = imgPath.size(); // 图片路径长度
        *(uint64_t*)(msgBuf + 0x108 / 8) = CalcWxCapacity(imgPath.size());


        *(uint64_t*)(msgBuf + 0x130 / 8) = 0xF;
        *(uint64_t*)(msgBuf + 0x150 / 8) = 7;
        *(uint64_t*)(msgBuf + 0x170 / 8) = 0xF;

        //0x178 = filename
		//0x188 & 0x190= filename length & capacity

        *(uint64_t*)(msgBuf + 0x198 / 8) = filesize;         //图片大小
        *(uint64_t*)(msgBuf + 0x1B8 / 8) = 0xF;

        *(uint64_t*)(msgBuf + 0x668 / 8) = 0xF;
        *(uint64_t*)(msgBuf + 0x688 / 8) = 0xF;
        *(uint64_t*)(msgBuf + 0x6B0 / 8) = 0xF;
        SetWeixinString((WeixinString*)(msgBuf + 0x6B8 / 8), guid);
        *(uint64_t*)(msgBuf + 0x6C8 / 8) = guid.size();      // length = 36
        *(uint64_t*)(msgBuf + 0x6D0 / 8) = CalcWxCapacity(guid.size());


        *(uint64_t*)(msgBuf + 0x6F0 / 8) = 0xF;
        *(uint64_t*)(msgBuf + 0x710 / 8) = 0xF;
        *(uint64_t*)(msgBuf + 0x738 / 8) = 0xF;
        *(uint64_t*)(msgBuf + 0x760 / 8) = 0xF;
        *(uint64_t*)(msgBuf + 0x780 / 8) = 0xF;
        *(uint64_t*)(msgBuf + 0x7A0 / 8) = 0xF;
    }

    void BuildSendParam2_Image(uint64_t* out)
    {
        // 获取 Weixin.dll 模块句柄
        uintptr_t hModule = (uintptr_t)GetModuleHandleA("Weixin.dll");

        uint64_t globalValue = *(uint64_t*)(hModule + offset::param2);


        WeixinCall create = (WeixinCall)(hModule + offset::create_param2);

        // 第一个分配: 16字节 (不是 0x10 * 8 = 128字节!)
        uint64_t* buf = (uint64_t*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 0x10);

        // 分配三个 64 字节的结构
        auto p2 = (UnknownBlock*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 64);
        auto p3 = (UnknownBlock*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 64);
        auto p4 = (UnknownBlock*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 64);

        p2->vtable = hModule + offset::param2_1;
        p2->temp[0] = 0;
        p2->temp[1] = 0;
        p2->temp[2] = 0;
        p2->temp[3] = 0;
        p2->temp[4] = 0;
        p2->temp[5] = 0;
        p2->self = (uint64_t)p2;

        p3->vtable = hModule + offset::param2_2;
        p3->temp[0] = 0;
        p3->temp[1] = 0;
        p3->temp[2] = 0;
        p3->temp[3] = 0;
        p3->temp[4] = 0;
        p3->temp[5] = 0;
        p3->self = (uint64_t)p3;

        p4->vtable = hModule + offset::param2_3;
        p4->temp[0] = 0;
        p4->temp[1] = 0;
        p4->temp[2] = 0;
        p4->temp[3] = 0;
        p4->temp[4] = 0;
        p4->temp[5] = 0;
        p4->self = (uint64_t)p4;

        // 调用子函数，参数顺序根据汇编确定
        // rcx = out, rdx = p4, r8 = p3, r9 = p2, [rsp+0x30] = buf, [rsp+0x38] = globalValue
        create(
            (uint64_t)out,      // arg1
            (uint64_t)p2,       // arg2
            (uint64_t)p3,       // arg3
            (uint64_t)p4,       // arg4
            (uint64_t)buf,      // arg5
            globalValue         // arg6 - 从 Weixin.dll+0x8E4C758 读取的值
        );
    }

    InnerStruct2* BuildSendParam1(uint64_t msgStruct)
    {
        // 获取 Weixin.dll 模块句柄
        uintptr_t hModule = (uintptr_t)GetModuleHandleA("Weixin.dll");

        // 分配第一个结构: 0x20 (32字节)
        InnerStruct1* struct1 = (InnerStruct1*)HeapAlloc(GetProcessHeap(), 8, 0x20);

        // 初始化第一个结构
        struct1->ptr1 = (void*)(msgStruct + 0x10);  // 指向消息结构的 +0x10 偏移
        struct1->ptr2 = (void*)(msgStruct); 
        struct1->ptr3 = nullptr;
        struct1->count = 0;

        // 分配第二个结构: 0x28 (40字节)
        InnerStruct2* struct2 = (InnerStruct2*)HeapAlloc(GetProcessHeap(), 8, 0x28);

        // 初始化第二个结构
        struct2->vtable = (void*)(hModule + offset::param1_vtable);  // vtable 指针
        struct2->ptr1 = struct1;                           // 指向第一个结构
        struct2->data1 = (uint64_t)struct1 + 0x10;               // 重复指针
        struct2->data2 = (uint64_t)struct1 + 0x10;               // 重复指针
        struct2->field1 = 1;                                 // 标志位 = 1

        // 返回第二个结构
        return struct2;
    }

    void SendImage(const std::string& wxid, const std::string& imgPath)
    {
        uintptr_t WeixinDLL_baseAddr = GetWeixinDllBase();

        uint64_t* msgStruct = (uint64_t*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 0x7C0);
        if (!msgStruct) {
            return;
        }

        BuildSendParam1_sub(msgStruct, wxid, imgPath);

        uint64_t* struct2 = (uint64_t*)BuildSendParam1((uint64_t)msgStruct);


        uint64_t* paramStruct = (uint64_t*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 0xE8);

        BuildSendParam2_Image(paramStruct);

        WeixinCall send_message = (WeixinCall)(WeixinDLL_baseAddr + offset::send_message);
        send_message((uint64_t)struct2, (uint64_t)paramStruct);

    }



    void BuildTextMessage(uint64_t* ptr, const std::string& text, const std::string& wxid)
    {
        uintptr_t base = GetWeixinDllBase();

        // 对齐原始布局
        ptr[0] = base + offset::txt_message_vtbl;
        ptr[1] = 0x200000005LL;


        WeixinCall init_text_st = (WeixinCall)(base + offset::txt_message_ctr);

        TextMessage* msg = reinterpret_cast<TextMessage*>(ptr + 2);
        init_text_st(reinterpret_cast<uint64_t>(msg));

        SetWeixinString(&msg->receiver, wxid);
        SetWeixinString(&msg->content, text);
        msg->msg_len = text.length();
        msg->type = 1;
    }

    void BuildSendParam2_Text(uint64_t* a1)
    {
        uintptr_t base = GetWeixinDllBase();
        WeixinCall Build_Send_TextImg_Pars2 = (WeixinCall)(base + offset::create_param2);

        uint64_t a2 = base + offset::param2_1;
        uint64_t a3 = base + offset::param2_2;
        uint64_t a4 = base + offset::param2_3;

        uint64_t param2_addr = base + offset::param2;  
         

        uint64_t* buf = HeapAlloc_mb<uint64_t>(16);


        auto p2 = (UnknownBlock*)HeapAlloc_mb<uint64_t>(64);
        auto p3 = (UnknownBlock*)HeapAlloc_mb<uint64_t>(64);
        auto p4 = (UnknownBlock*)HeapAlloc_mb<uint64_t>(64);

        // === 必须清零 ===
        memset(p2, 0, sizeof(UnknownBlock));
        memset(p3, 0, sizeof(UnknownBlock));
        memset(p4, 0, sizeof(UnknownBlock));

        p2->vtable = a2;
        p2->self = (uint64_t)p2;

        p3->vtable = a3;
        p3->self = (uint64_t)p3;

        p4->vtable = a4;
        p4->self = (uint64_t)p4;

        // === 核心：最后一个参数必须解引用 ===
        Build_Send_TextImg_Pars2(
            (uint64_t)a1,
            (uint64_t)p2,
            (uint64_t)p3,
            (uint64_t)p4,
            (uint64_t)buf,
            *(uint64_t*)param2_addr
        );
    }

    void SendText(const std::string& wxidorgid, const std::string& msg)
    {
        uintptr_t base = GetWeixinDllBase();

		//不同版本需要调整 msgBuf 大小，过小会导致发送失败，过大会浪费内存 
        uint64_t* msgBuf = HeapAlloc_mb<uint64_t>(0x768);
        BuildTextMessage(msgBuf, msg, wxidorgid);

        uint64_t* data = HeapAlloc_mb<uint64_t>(0x20);
        data[0] = (uint64_t)(msgBuf + 2);
        data[1] = (uint64_t)(msgBuf);
        data[2] = 0;

        uint64_t* arg1 = HeapAlloc_mb<uint64_t>(0x28);
        arg1[0] = base + offset::param1_vtable;
        arg1[1] = reinterpret_cast<uint64_t>(data);
        arg1[2] = (uint64_t)data + 0x10;
        arg1[3] = (uint64_t)data + 0x10;
        arg1[4] = 1;

        uint64_t* arg2 = HeapAlloc_mb<uint64_t>(0xE8);
        BuildSendParam2_Text(arg2);

        WeixinCall send_message = (WeixinCall)(base + offset::send_message);
        send_message((uint64_t)arg1, (uint64_t)arg2);
    }



    void DecodePic(const std::string& enc_pic_path, const std::string& dec_pic_path)
    {
        uintptr_t base = GetWeixinDllBase();

        uint64_t* arg1 = HeapAlloc_mb<uint64_t>(0x28);
        SetWeixinString((WeixinString*)(arg1), enc_pic_path);

        uint64_t* arg2 = HeapAlloc_mb<uint64_t>(0x28);
        SetWeixinString((WeixinString*)(arg2), dec_pic_path);


        WeixinCall Decode_Pic = (WeixinCall)(base + offset::dec_pic_call);
        Decode_Pic((uint64_t)arg1, (uint64_t)arg2, 1);
    }

}
