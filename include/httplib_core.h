#pragma once
#ifndef CPPHTTPLIB_OPENSSL_SUPPORT
// #define CPPHTTPLIB_OPENSSL_SUPPORT
#endif

int getHttpLibThreadPoolCount();
void setHttpLibThreadPoolCount(int nCount);

#ifndef CPPHTTPLIB_THREAD_POOL_COUNT
#define CPPHTTPLIB_THREAD_POOL_COUNT getHttpLibThreadPoolCount()
#endif
#include <httplib/httplib.h>