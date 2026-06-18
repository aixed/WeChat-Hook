#pragma once
#ifndef WX_SEND_XML_H
#define WX_SEND_XML_H

#include <string>
#include <cstdint>

namespace WeixinSendXML
{
    // 初始化函数
    void Initialize();

    // 设置微信DLL基址（如果DLL已注入，会自动获取）
    void SetWeixinBase(uintptr_t base);

    // 设置进程句柄（在DLL中通常不需要）
    void SetProcessHandle(void* handle);

    // 转发XML消息
    bool ForwardXmlMessage(const std::string& wxid, const std::string& xml);

    // 解析XML类型
    enum class XmlType {
        IMAGE,      // 图片
        VIDEO,      // 视频  
        ANIMATION,  // 动图/GIF
        OTHER       // 其他类型
    };

    // 提取XML中的字段
    struct XmlFields {
        std::string cdnbigurl;
        std::string cdnurl;
        std::string aeskey;
        std::string md5;
        int hdlength = 0;
        int length = 0;
        int hevc_mid_size = 0;
        int cdnthumblength = 0;
        int cdnthumbwidth = 0;
        int cdnthumbheight = 0;
        int playlength = 0;
        int type = 0;
        int width = 0;
        int height = 0;
        std::string productid;

        // 清理函数
        void Clear() {
            cdnbigurl.clear();
            cdnurl.clear();
            aeskey.clear();
            md5.clear();
            hdlength = length = hevc_mid_size = cdnthumblength = 0;
            cdnthumbwidth = cdnthumbheight = playlength = type = 0;
            width = height = 0;
            productid.clear();
        }
    };

    // 提取XML字段
    XmlFields ExtractXmlFields(const std::string& xml, XmlType xmlType);
}

#endif // WX_SEND_XML_H