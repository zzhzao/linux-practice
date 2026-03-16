#pragma once

#include <iostream>
#include <string.h>
#include <unordered_map>
#include <sstream>
#include "Logger.hpp"
#include<fstream>
const std::string linesep = "\r\n";
const std::string spaceseq = " ";
const std::string headersep = ": ";
const std::string g_http_version = "HTTP/1.1";
const std::string g_first_page = "index.html";
const std::string g_wwwroot = "wwwroot";

using namespace NS_LOG_MODULE;
class Util
{
public:
    // 空 ： 没有完整行
    // /r/n : 空行
    // 其他 : 读到了一行内容
    static std::string Readline(std::string &str)
    {
        auto pos = str.find(linesep);
        if (pos == std::string::npos)
        {
            return std::string();
        }
        std::string line = str.substr(0, pos);

        str.erase(0, line.size() + linesep.size());
        if (line.empty())
        {
            return linesep;
        }
        return line;
    }
    static std::string ReadFile(std::string &filename)
    {
        std::ifstream in(filename);
        if(!in.is_open())
        {
            return std::string();
        }
        std::string content;
        std::string line;
        while (std::getline(in,line))
        {
            content += line;
        }

        in.close();
        return content;
    }
};

class HttpRequest
{
private:
    bool ParseReqLine(std::string &httpstr)
    {
        std::string req_line = Util::Readline(httpstr);
        if (req_line.empty() || req_line == linesep)
        {
            return false;
        }
        std::stringstream ss(req_line);
        // 以空格作为分割符，拆分
        ss >> _method >> _uri >> _version;
        if(_uri  == "/")
        {
            _uri += g_first_page;
        }
        _uri = g_wwwroot + _uri;
        return true;
    }

    bool ParseHeaderkv(std::string &httpstr)
    {
        std::string header_line;
        do
        {
            header_line = Util::Readline(httpstr);
            if (header_line.empty())
            {
                return false;
            }
            if (header_line != linesep)
            {
                auto pos = header_line.find(headersep);

                if (pos == std::string::npos)
                {
                    return false;
                }
                std::string key = header_line.substr(0, pos);
                std::string value = header_line.substr(pos + headersep.size());
                _header.insert({key, value});
            }
        } while (header_line != linesep);

        return true;
    }
    bool ParseText(std::string &httpstr)
    {
        if (_header.find("Content-Length") == _header.end())
        {
            _text = "";
        }
        else
        {
            int content_len = std::stoi(_header["Content-Length"]);
            _text += httpstr.substr(0, content_len);
            httpstr.erase(0, content_len);
        }
        return true;
    }

public:
    HttpRequest()
    {
    }
    // 序列化和反序列化
    bool Deserialize(std::string &httpstr)
    {
        // 解析请求行
        bool n = ParseReqLine(httpstr);
        (void)n;

        // LOG(LogLevel::DEBUG) << "method:" << _method << " uri:" << _uri << " version:" << _version;

        n = ParseHeaderkv(httpstr);
        (void)n;
        _blank_line = linesep;

        // 解析正文部分
        n = ParseText(httpstr);
        return true;
    }
    std::string Uri()
    {
        return _uri;
    }
    ~HttpRequest() {}

private:
    std::string _method;
    std::string _uri;
    std::string _version;
    std::unordered_map<std::string, std::string> _header;
    std::string _blank_line;
    std::string _text;
};
class HttpResponse
{
public:
    HttpResponse():_version(g_http_version),_code(0),_blank_line(linesep)
    {
    }
    ~HttpResponse()
    {
    }
    bool Build(HttpRequest &req)
    {
        std::string target_file = req.Uri();
        _text = Util::ReadFile(target_file);
        if(_text.empty())
        {
            _code = 404;
            return false;
        }
        else
        {
            _code = 200;
            _code_desc = "OK";
            return true;
        }
    }
    std::string Serialize()
    {
        std::string respstr;
        respstr += _version + spaceseq + std::to_string(_code) + spaceseq + _code_desc + linesep;
        for(auto &header : _header)
        {
            std::string line = header.first + headersep + header.second + linesep;
            respstr += line;
        }

        respstr += _blank_line;
        respstr += _text;

        return respstr;
    }
private:
    std::string _version;
    int _code;
    std::string _code_desc;
    std::unordered_map<std::string,std::string> _header;
    std::string _blank_line;
    std::string _text;
};
class HttpProtocol
{
public:
    std::string HandlerHttpRequest(std::string &req)
    {
        // 我们在这里忽略粘包问题
        // 反序列化
        HttpRequest http_req;
        http_req.Deserialize(req);

        // 根据请求生成结果化的 resp
        HttpResponse http_resp;
        http_resp.Build(http_req);

        return http_resp.Serialize();

    }

    HttpProtocol() {}
    ~HttpProtocol() {}

private:
};

