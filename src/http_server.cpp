

#define CPPHTTPLIB_WIN32
#define CPPHTTPLIB_THREAD_POOL_COUNT 8

// 减少 Windows select() 的 idle interval，提升并发响应速度
#define CPPHTTPLIB_IDLE_INTERVAL_USECOND 100  // 原来是 10000 (10ms)

#include "httplib.h"
#include "json.hpp"

#include "http_server.h"
#include "http_routes.h"

#include <windows.h> 

#include <string>
#include <thread>
#include <atomic>

using json = nlohmann::json;


HttpServer::HttpServer(){}

HttpServer::~HttpServer()
{
    Stop();
}

bool HttpServer::Start(const std::string& host, int port)
{
    if (m_running)
        return true;

    m_running = true;
    m_thread = std::thread(&HttpServer::Run, this, host, port);
    m_thread.detach();   // DLL 中建议 detach，避免阻塞卸载
    return true;
}

void HttpServer::Stop()
{
    if (m_server)
        m_server->stop(); // httplib::Server 停止 listen
    m_running = false;
}

void HttpServer::Run(const std::string& host, int port)
{
    m_server = std::make_unique<httplib::Server>();

    // 启用线程池，支持并发请求处理
    m_server->new_task_queue = [=] { return new httplib::ThreadPool(CPPHTTPLIB_THREAD_POOL_COUNT); };

    // 减少 idle interval，提升响应速度
    m_server->set_idle_interval(0, 100);  // 100 微秒

    // 禁用 Nagle 算法，减少延迟
    m_server->set_tcp_nodelay(true);

    // 注册路由
    RegisterRoutes(*m_server);

    char buf[256];
    sprintf_s(buf, "[HTTP] Started -> %s:%d (ThreadPool: %d, IdleInterval: 100us)\n", 
              host.c_str(), port, CPPHTTPLIB_THREAD_POOL_COUNT);
    OutputDebugStringA(buf);

    // 0.0.0.0:30001
    m_server->listen(host.c_str(), port);

#ifdef _DEBUG
    OutputDebugStringA("[HTTP] Server stopped\n");
#endif
}
