#pragma once

#include <iostream>
#include <string.h>
#include <unordered_map>
#include <sstream>
#include "Logger.hpp"
#include <fstream>
const std::string linesep = "\r\n";
const std::string spaceseq = " ";
const std::string headersep = ": ";
const std::string g_http_version = "HTTP/1.1";
const std::string g_first_page = "index.html";
const std::string g_wwwroot = "wwwroot";
const std::string page_404 = "./wwwroot/404.html";


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
    static std::string ReadFile(const std::string &filename)
    {
        std::ifstream in(filename);
        if (!in.is_open())
        {
            return std::string();
        }
        std::string content;
        std::string line;
        while (std::getline(in, line))
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
        if (_uri == "/")
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
    std::string RequestContent()
    {
        return Util::ReadFile(_uri);
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
private:
    std::string CodeToDesc(int code)
    {
        switch (_code)
        {
        case 100:
            return "Continue";

        case 200:
            return "OK";

        case 404:
            return "Not Found";

        default:
            return "Unknown";
        }
    }

public:
    HttpResponse() : _version(g_http_version), _code(0), _blank_line(linesep)
    {
    }
    void SetCode(int code)
    {
        _code = code;
        _code_desc = CodeToDesc(_code);
    }
    void AddHeader(std::string key, std::string value)
    {
        _header[key] = value;
    }
    void SetBody(const std::string &content)
    {
        _text = content;
    }
    int BodeSize()
    {
        return _text.size();

    }
    std::string Serialize()
    {
        std::string respstr;
        respstr += _version + spaceseq + std::to_string(_code) + spaceseq + _code_desc + linesep;
        for (auto &header : _header)
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
    std::unordered_map<std::string, std::string> _header;
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
        std::string content = http_req.RequestContent();
        if(content.empty())
        {
            // http_resp.SetCode(404);
            // http_resp.SetBody(Util::ReadFile(page_404));
            // http_resp.AddHeader("Content-Length", std::to_string(http_resp.BodeSize()));
            http_resp.SetCode(302);
            http_resp.AddHeader("Location", g_first_page);
        }
        else{
            http_resp.SetCode(200);
            http_resp.AddHeader("Content-Length", std::to_string(content.size()));
            http_resp.AddHeader("Connection", "close");
            http_resp.SetBody(content);
        }


        return http_resp.Serialize();
    }

    HttpProtocol() {}
    ~HttpProtocol() {}

private:
};
