#pragma once
#include <string>

namespace WeixinSend
{
    void SendImage(const std::string& wxid, const std::string& imgPath);
    void SendText(const std::string& wxidorgid, const std::string& msg);
    void DecodePic(const std::string& enc_pic_path, const std::string& dec_pic_path);
}
