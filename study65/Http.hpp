#pragma once

#include<iostream>
#include<string.h>
#include <unordered_map>

const std::string linesep = "\r\n";

class HttpRequest {
public:
    HttpRequest()
    {}
    // 序列化和反序列化
    ~HttpRequest(){}
private:
    std::string _method;
    std::string _uri;
    std::string _version;
    std::unordered_map<std::string, std::string> _header;
    std::string _blank_line;
    std::string _text;
};
class HttpResponse {
public:

    HttpResponse()
    {}
    ~HttpResponse()
    {}
};
class HttpProtocol {
public:
    std::string HandlerHttpRequest(std::string& req)
    {
        return "Hello World";
    }

    HttpProtocol(){}
    ~HttpProtocol(){}
private:

};