#include "httplib.h"
#include "http_routes.h"
#include "SendTextMsg.h"
#include "GetSelfProfile.h"
#include "ForwardXMLMsg.h"
#include "SendImageMsg.h"
#include "QueryDB.h"

void RegisterRoutes(httplib::Server& svr)
{
    Route_SendTextMsg(svr);
    RegisterGetSelfProfile(svr);
    Route_ForwardXMLMsg(svr);
    Route_SendImageMsg(svr);
    Route_QueryDB(svr);
}
