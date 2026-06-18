#include <Windows.h>
#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>
#include <sstream>
#include "Hook_Method.h"
#include "hook_xlog.h"

namespace hook {
    std::string ReadUtf8String(void* address, size_t length) {
        if (!address || length == 0) {
            return "";
        }

        // 读取原始字节数据
        std::vector<char> buffer(length);
        memcpy(buffer.data(), address, length);

        // 直接作为UTF-8字符串返回（因为原始数据就是UTF-8）
        return std::string(buffer.data(), length);
    }

    void MyCallHandler_xLog(CALL_CONTEXT* ctx)
    {
        if (!ctx) return;

        auto param_struct = (void**)ctx->rcx;
        if (!param_struct) return;

        // 读取数据地址和长度
        void* data_address = param_struct[0];  // 第一个QWORD是数据地址
        size_t data_length = (size_t)param_struct[2];  // 第3个QWORD是数据长度

        // 输出基本信息
        //char info_msg[256];
        //sprintf_s(info_msg, "\n[xLog Hook] Data addr: %p, Length: %zu bytes", data_address, data_length);
        //OutputDebugStringA(info_msg);
        //OutputDebugStringA("\n");

        // 验证数据地址是否有效
        if (data_address && data_length > 0 && data_length < 0x10000) { // 合理的长度限制

            std::string ascii_text = ReadUtf8String(data_address, data_length);
            OutputDebugStringA((ascii_text + "").c_str());
            
        }
        else {
        }
    }



}
