#include "httplib.h"
#include "json.hpp"
#include <windows.h>
#include <string>
#include "global.h"
#include "tools.h"

#include "GetSelfProfile.h"


using json = nlohmann::json;


/* =========================
   核心：获取个人资料
   ========================= */



/* =========================
   HTTP 接口
   ========================= */

void RegisterGetSelfProfile(httplib::Server& svr)
{
    svr.Post("/GetSelfProfile", [](const httplib::Request&, httplib::Response& res)
        {
            json resp;

            resp["wxid"] = SelfInfo.wxid;
            resp["alias"] = SelfInfo.alias;
            resp["nickname"] = SelfInfo.nickname;
            resp["email"] = SelfInfo.email;
            resp["qq"] = SelfInfo.qq;
            resp["phone"] = SelfInfo.phone;
            resp["proiv"] = SelfInfo.proiv;
            resp["area"] = SelfInfo.area;
            resp["signinfo"] = SelfInfo.signinfo;
            
            res.set_content(resp.dump(), "application/json");
        });
}
