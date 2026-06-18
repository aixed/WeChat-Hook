#pragma once
#include <string>

// 异步 POST JSON（线程方式，不阻塞 Hook）
void HttpPostJsonAsync(const std::string& url, const std::string& json);

// 同步 POST JSON
bool HttpPostJsonSync(const std::string& url, const std::string& json);


void HttpPostJsonAsync_(const std::string& url, const std::string& json);

