#include "httplib_core.h"

static int HTTP_SERVER_THREAD_POOL_COUNT = 0;

int getHttpLibThreadPoolCount() {
    int defalutCount = (std::max)(8u, std::thread::hardware_concurrency() > 0 ? std::thread::hardware_concurrency() - 1 : 0);
    return (std::max)(defalutCount, HTTP_SERVER_THREAD_POOL_COUNT);
}

void setHttpLibThreadPoolCount(int nCount) {
    HTTP_SERVER_THREAD_POOL_COUNT = nCount;
}