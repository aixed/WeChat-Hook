#pragma once

#include <string>
#include <thread>
#include <atomic>
#include <memory>


namespace httplib { class Server; } // 前向声明

class HttpServer
{
public:
    HttpServer();
    ~HttpServer();

    bool Start(const std::string& host, int port);
    void Stop();

private:
    void Run(const std::string& host, int port);

private:
    std::thread m_thread;
    std::atomic<bool> m_running{ false };

    // 新增，持有 httplib::Server 实例
    std::unique_ptr<httplib::Server> m_server;

};
