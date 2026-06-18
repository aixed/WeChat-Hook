#pragma once
#include "httplib_core.h"

typedef std::map<std::string, std::string> HttpParams;

typedef struct HttpUrlTag {
    std::string protocol;
    std::string host;
    short port = 0;
    std::string path;
    std::string path_with_query;
    std::string query_params_str;
    HttpParams query_params;
    std::string url;
    std::string uri;
    std::string native_url;
    bool valid = false;
}HTTPURL, * LPHTTPURL;

class HUrl : private HttpUrlTag {
public:
    const std::string& getProtocol() const { return this->protocol; }
    const std::string& getHost() const { return this->host; }
    short getPort() { return this->port; }
    const std::string& getPath() const { return this->path; }
    const std::string& getPathEx() const { return this->path_with_query; }
    const std::string& getQueryParamsStr() const { return this->query_params_str; }
    const HttpParams& getQueryParams() const { return this->query_params; }
    const std::string& getUrl() const { return this->url; }
    const std::string& getNativeUrl() const { return this->native_url; }
    const std::string& getUri() const { return this->uri; }
    bool isValid() { return this->valid; }
public:
    HUrl(const std::string& szUrl);
};

class requests {
public:
    static httplib::Client make_client(const std::string uri);
    static httplib::Result get(const std::string& url,
                               const httplib::Params& params = httplib::Params(),
                               const httplib::Headers& headers = httplib::Headers(),
                               httplib::Progress progress = nullptr,
                               httplib::ContentReceiver content_receiver = nullptr,
                               httplib::ResponseHandler response_handler = nullptr);

    static httplib::Result post(const std::string& url);
    static httplib::Result post(const std::string& url,
                                const httplib::Headers headers);
    static httplib::Result post(const std::string& url,
                                const httplib::Headers& headers,
                                const std::string& body,
                                const std::string& content_type);
};