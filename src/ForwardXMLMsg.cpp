#include "json.hpp"
#include "httplib.h"

#include <windows.h>
#include "wx_send_xml.h"

#include "ForwardXMLMsg.h"



using json = nlohmann::json;

void Route_ForwardXMLMsg(httplib::Server& svr)
{
    svr.Post("/ForwardXMLMsg", [](const httplib::Request& req, httplib::Response& res)
        {
            json reqJson;
            json resp;

            try
            {
                reqJson = json::parse(req.body);
            }
            catch (...)
            {
                resp["ret"] = -1;
                resp["msg"] = "invalid json";
                res.set_content(resp.dump(), "application/json");
                return;
            }

            std::string wxid = reqJson.value("to_wxid", "");
            std::string xml = reqJson.value("content", "");


            // 后续调用
            WeixinSendXML::Initialize();
            bool success = WeixinSendXML::ForwardXmlMessage(wxid, xml);

            if (success) {
                resp["ret"] = 0;
                resp["retmsg"] = "success";
            }
            else {
                resp["ret"] = 1;
                resp["retmsg"] = "fail";
            }

            
            

            res.set_content(resp.dump(), "application/json");
        });
}
 