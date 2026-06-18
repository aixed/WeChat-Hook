#pragma once

namespace httplib { class Server; }

// 注册 /GetSelfProfile/ 接口
void RegisterGetSelfProfile(httplib::Server& svr);
