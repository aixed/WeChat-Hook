#include "requests.h"

HUrl::HUrl(const std::string& szUrl) {
    size_t pos = 0;
    std::string wurl(szUrl);
    native_url = szUrl;
    pos = wurl.find_first_of("?");
    if (pos != std::string::npos) {
        query_params_str = wurl.substr(pos + 1);
        wurl = wurl.substr(0, pos);
        std::string tmp_params_str(query_params_str);
        while (tmp_params_str.length() > 0) {
            size_t tmp_pos = tmp_params_str.find_first_of('&');
            std::string block_str;
            if (tmp_pos != std::string::npos) {
                block_str = tmp_params_str.substr(0, tmp_pos);
                tmp_params_str = tmp_params_str.substr(tmp_pos + 1);
            }
            else {
                block_str = tmp_params_str;
                tmp_params_str = "";
            }
            size_t equal_pos = block_str.find_first_of('=');
            if (equal_pos != std::string::npos) {
                std::string key = block_str.substr(0, equal_pos);
                std::string value = block_str.substr(equal_pos + 1);
                query_params[key] = value;
            }
            else {
                continue;
            }
        }
    }
    url = wurl;
    pos = wurl.find_first_of(":");
    if (pos != std::string::npos) {
        protocol = wurl.substr(0, pos);
    }
    if (!(protocol == "http" || protocol == "https")) {
        valid = false;
        return;
    }
    wurl = wurl.substr(pos + 3);
    pos = wurl.find_first_of("/");
    std::string host_with_port;
    if (pos != std::string::npos) {
        host_with_port = wurl.substr(0, pos);
        path = wurl.substr(pos);
    }
    else {
        host_with_port = wurl;
        path = "/";
    }
    if (!query_params_str.empty())
        path_with_query = path + "?" + query_params_str;
    else
        path_with_query = path;
    pos = host_with_port.find_first_of(":");
    if (pos != std::string::npos) {
        host = host_with_port.substr(0, pos);
        std::string szPort = host_with_port.substr(pos + 1);
        port = (short)atoi(szPort.c_str());
    }
    else {
        host = host_with_port;
        port = (protocol == "http") ? 80 : 443;
    }
    this->uri = protocol + "://" + host;
    if (!(port == 80 || port == 443)) {
        this->uri = this->uri + ":" + std::to_string(port);
    }
    valid = true;
}

httplib::Client requests::make_client(const std::string uri)
{
    httplib::Client cli(uri);
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT

#ifdef CA_CERT_FILE
    cli.set_ca_cert_path(CA_CERT_FILE);
#endif // CA_CERT_FILE

#ifdef CLIENT_SSL_VERIFY
    cli.enable_server_certificate_verification(CLIENT_SSL_VERIFY);
#else
    cli.enable_server_certificate_verification(false);
#endif // CLIENT_SSL_VERIFY

#endif // CPPHTTPLIB_OPENSSL_SUPPORT
    cli.set_connection_timeout(0, 3000000);
    cli.set_read_timeout(20, 0);
    cli.set_write_timeout(5, 0);
    return cli;
}

httplib::Result requests::get(  const std::string& url,
                                const httplib::Params& params,
                                const httplib::Headers& headers,
                                httplib::Progress progress,
                                httplib::ContentReceiver content_receiver,
                                httplib::ResponseHandler response_handler)
{
    HUrl hUrl(url);
    assert(hUrl.isValid());
    httplib::Client cli = make_client(hUrl.getUri());
    std::string path = hUrl.getPathEx();
    if (progress && content_receiver && response_handler)
        return cli.Get(path, params, headers, response_handler, content_receiver, progress);
    else if (content_receiver && progress)
        return cli.Get(path, params, headers, content_receiver, progress);
    else if (content_receiver && response_handler)
        return cli.Get(path, params, headers, response_handler, content_receiver);
    else if (progress)
        return cli.Get(path, params, headers, progress);
    else if (content_receiver)
        return cli.Get(path, params, headers, content_receiver);
    return cli.Get(path, params, headers);
}

httplib::Result requests::post(const std::string& url)
{
    HUrl hUrl(url);
    assert(hUrl.isValid());
    httplib::Client cli = make_client(hUrl.getUri());
    return cli.Post(hUrl.getPathEx());
}

httplib::Result requests::post( const std::string& url,
                                const httplib::Headers headers)
{
    HUrl hUrl(url);
    assert(hUrl.isValid());
    httplib::Client cli = make_client(hUrl.getUri());
    return cli.Post(hUrl.getPathEx(), headers);
}

httplib::Result requests::post( const std::string& url,
                                const httplib::Headers& headers,
                                const std::string& body,
                                const std::string& content_type)
{
    HUrl hUrl(url);
    assert(hUrl.isValid());
    httplib::Client cli = make_client(hUrl.getUri());
    return cli.Post(hUrl.getPathEx(), headers, body, content_type);
};