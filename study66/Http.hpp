#pragma once

#include <iostream>
#include <string.h>
#include <unordered_map>
#include <sstream>
#include "Logger.hpp"
#include <fstream>
#include <cstdio>
const std::string linesep = "\r\n";
const std::string spaceseq = " ";
const std::string headersep = ": ";
const std::string g_http_version = "HTTP/1.1";
const std::string g_first_page = "index.html";
const std::string g_wwwroot = "wwwroot";
const std::string page_404 = "./wwwroot/404.html";
const std::string suffixsep = ".";
const std::string argsep = "?";
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
    // 如果我们要读取二进制文件，建议使用vector<char>
    static std::string ReadFile(const std::string &filename)
    {

        std::ifstream in(filename, std::ios::binary);
        if (!in.is_open())
            return "";
        in.seekg(0, in.end);
        int filesize = in.tellg();
        in.seekg(0, in.beg);
        std::string content;
        content.resize(filesize);
        in.read((char *)content.c_str(), filesize);

        // std::vector<char> content(filesize);

        // in.read(content.data(), filesize);
        in.close();

        return content;

        // std::ifstream in(filename);
        // if (!in.is_open())
        // {
        //     return std::string();
        // }
        // std::string content;
        // std::string line;
        // while (std::getline(in, line))
        // {
        //     content += line;
        // }

        // in.close();
        // return content;
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

        auto pos = _uri.rfind(suffixsep);
        if (!pos == std::string::npos)
        {
            _suffix = ".html";
        }
        else
        {
            _suffix = _uri.substr(pos);
        }

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
        if (strcasecmp(_method.c_str(), "GET") == 0)
        {
            auto pos = _uri.find(argsep);
            if (pos == std::string::npos)
            {
                _text = std::string();
                return true;
            }
            else
            {
                _text = _uri.substr(pos + argsep.size());
                _uri = _uri.substr(0, pos);
            }
        }
        else
        {
            if (_header.find("Content-Length") == _header.end())
            {
                _text = "";
            }
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
    std::string Suffix()
    {
        return _suffix;
    }
    ~HttpRequest() {}
    void DebugPrint()
    {
        std::cout << "_version: " << _version << std::endl;
        std::cout << "_uri: " << _uri << std::endl;
        std::cout << "_method: " << _method << std::endl;

        for (auto &header : _header)
        {
            std::cout << "==>" << header.first << " # " << header.second << std::endl;
        }

        std::cout << _blank_line;

        std::cout << _text << std::endl;
    }
    std::string Text()
    {
        return _text;
    }

private:
    std::string _method;
    std::string _uri;
    std::string _version;
    std::unordered_map<std::string, std::string> _header;
    std::string _blank_line;
    std::string _text;

    std::string _suffix;
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

using service_t = std::function<void(HttpRequest &req, HttpResponse &resp)>;

class MiniType
{
public:
    static std::string Suffix2MimeType(const std::string &suffix)
    {
        auto iter = _mime_map.find(suffix);
        if (iter != _mime_map.end())
            return iter->second;
        else
            return "text/html";
    }

private:
    static std::unordered_map<std::string, std::string> _mime_map;
};
std::unordered_map<std::string, std::string> MiniType::_mime_map = {
    // 文本类型
    {".txt", "text/plain"},
    {".html", "text/html"},
    {".htm", "text/html"},
    {".css", "text/css"},

    // 图片类型
    {".jpg", "image/jpeg"},
    {".jpeg", "image/jpeg"},
    {".png", "image/png"},
    {".gif", "image/gif"},

    // 应用类型
    {".pdf", "application/pdf"},
    {".json", "application/json"},
    {".xml", "application/xml"},

    // 脚本类型
    {".js", "application/javascript"},

    // 其他常用类型
    {".zip", "application/zip"},
    {".mp3", "audio/mpeg"},
    {".mp4", "video/mp4"}};
class HttpProtocol
{
public:
    void RegisterService(const std::string &uri, service_t service)
    {
        std::string key = g_wwwroot + uri;
        _http_services[key] = service;
    }

    bool IsReqService(const std::string &uri)
    {
        auto iter = _http_services.find(uri);
        if (iter == _http_services.end())
        {
            return false;
        }
        else
        {
            return true;
        }
    }

    std::string HandlerHttpRequest(std::string &req)
    {
        // 我们在这里忽略粘包问题
        // 反序列化
        HttpRequest http_req;
        http_req.Deserialize(req);
        HttpResponse http_resp;
        if (IsReqService(http_req.Uri()))
        {
            // 功能路由
            _http_services[http_req.Uri()](http_req, http_resp);
        }
        else
        {

            std::cout << "Suffix: " << http_req.Suffix() << std::endl;
            // 根据请求生成结果化的 resp

            std::string content = http_req.RequestContent();
            if (content.empty())
            {
                // http_resp.SetCode(404);
                // http_resp.SetBody(Util::ReadFile(page_404));
                // http_resp.AddHeader("Content-Length", std::to_string(http_resp.BodeSize()));
                http_resp.SetCode(302);
                http_resp.AddHeader("Location", g_first_page);
            }
            else
            {
                http_resp.SetCode(200);
                http_resp.AddHeader("Content-Length", std::to_string(content.size()));
                http_resp.AddHeader("Content-Type", MiniType::Suffix2MimeType(http_req.Suffix()));
                http_resp.AddHeader("Connection", "close");
                http_resp.SetBody(content);
            }
        }

        return http_resp.Serialize();
    }

    HttpProtocol()
    {
    }
    ~HttpProtocol() {}

private:
    std::unordered_map<std::string, service_t> _http_services;
};
