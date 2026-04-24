#pragma once

// 自定义协议部分
#include <iostream>
#include <string>
#include <jsoncpp/json/json.h>
#include <functional>
#include "Logger.hpp"

using namespace NS_LOG_MODULE;

// 1. 自己做 - 不建议的！
// 2. 用别人的 - json protobuf xml

// 请求报文
class Request
{
public:
    Request() : _data_x(0), _data_y(0), _oper(0)
    {
    }
    Request(int x, int y, char oper) : _data_x(x), _data_y(y), _oper(oper)
    {
    }
    bool Serialize(std::string *out)
    {
        // 结构化 -> "_data_x _oper _data_y"
        Json::Value root;
        root["left"] = _data_x;
        root["right"] = _data_y;
        root["oper"] = _oper;

        Json::FastWriter writer;
        *out = writer.write(root);
        return true;
    }
    bool Deserialize(std::string &in) // "_data_x _oper _data_y"
    {
        // "_data_x _oper _data_y" -> 结构化
        Json::Value root;
        Json::Reader reader;
        bool parsesuccess = reader.parse(in, root);
        if (!parsesuccess)
            return false;

        _data_x = root["left"].asInt();
        _data_y = root["right"].asInt();
        _oper = root["oper"].asInt();
        return true;
    }
    ~Request()
    {
    }
    // get set

public:
    // 10 20 '-' -> 10 - 20 = ?
    // _data_x _oper _data_y
    int _data_x;
    int _data_y;
    char _oper; // '+' '-' '/' '*' '%'
};

// 应答报文
class Response
{
public:
    Response() : _result(0), _code(0)
    {
    }
    Response(int result, int code) : _result(result), _code(code)
    {
    }
    bool Serialize(std::string *out)
    {
        Json::Value root;
        root["result"] = _result;
        root["code"] = _code;

        Json::FastWriter writer;
        *out = writer.write(root);
        return true;
    }
    bool Deserialize(std::string &in)
    {
        Json::Value root;
        Json::Reader reader;
        bool parsesuccess = reader.parse(in, root);
        if (!parsesuccess)
            return false;

        _result = root["result"].asInt();
        _code = root["code"].asInt();
        return true;
    }
    ~Response()
    {
    }

public:
    int _result; // 结果
    int _code;   // 状态码
};

const std::string gsep = "\r\n";

using HandlerRequest_t = std::function<Response(Request &)>;
using HandlerResponse_t = std::function<void (Response &)>;

class Protocol
{
public:
    Protocol(HandlerRequest_t handler) : _version("1.0"), _handler_request(handler)
    {
    }
    Protocol(HandlerResponse_t handler_response):_version("1.0"), _handler_response(handler_response)
    {
    }
    // {"left": 10, "right": 20, oper: '+'}
    // len\r\n{"left": 10, "right": 20, oper: '+'}\r\n
    std::string Packet(const std::string &json_string)
    {
        return std::to_string(json_string.size()) + gsep + json_string + gsep;
    }
    // len\r\n{"left": 10, "right": 20, oper: '+'}\r\n
    // len\r\n{"left": 10, "right": 20, oper: '+'}\r\nlen\r\n{"left": 10, "right": 20, oper: '+'}\r\n
    // len\r\n{"left": 10, "right": 20, oper: '+'}\r\nlen\r\n{"left": 10,
    // len\r\n{"left": 10, "right": 20, o
    // le
    // ret > 0: no error, json_string != NULL
    // ret == 0: no error, json_string == NULL
    // ret < 0 : error.
    int Unpack(std::string &packet, std::string *json_string)
    {
        if (packet.empty())
            return 0;
        if (json_string == nullptr)
            return -1;

        // 分析报文
        auto pos = packet.find(gsep);
        if (pos == std::string::npos)
            return 0;
        std::string lenstr = packet.substr(0, pos);

        // lenstr 合法性判断，lenstr -> 123 345

        int len = std::stoi(lenstr);
        int total = lenstr.size() + len + 2 * gsep.size();
        if (packet.size() < total)
            return 0;
        // 提取报文
        *json_string = packet.substr(pos + gsep.size(), len);
        packet.erase(0, total);
        return 1;
    }
    // 如果读到半个报文，什么都不做
    // 如果读到一个报文+，循环处理，把所有合法的报文都进行统一处理
    std::string ParseRequest(std::string &inbuffer)
    {
        std::string result;
        while (true)
        {
            std::string json_string;
            // 1. 解包
            int n = Unpack(inbuffer, &json_string);
            if (n < 0)
            {
                LOG(LogLevel::DEBUG) << "no way !!";
                return std::string();
            }
            if (n == 0)
            {
                LOG(LogLevel::INFO) << inbuffer << " parse done";
                return result;
            }
            LOG(LogLevel::DEBUG) << "json_string:\n" << json_string;
            LOG(LogLevel::DEBUG) << "unpack done, inbuffer:\n" << inbuffer;
            // 2. 反序列化
            // 得到一个完整的报文jsonstring
            Request req;
            if (!req.Deserialize(json_string))
                return std::string();

            // 3. 业务计算
            Response resp;
            if (_handler_request)
                resp = _handler_request(req);

            // 4. 应答序列化
            std::string resp_json_string;
            resp.Serialize(&resp_json_string);

            // 5. 添加报头
            result += Packet(resp_json_string);
        }
    }
    std::string ParseResponse(std::string &inbuffer)
    {
        while (true)
        {
            std::string json_string;
            // 1. 解包
            int n = Unpack(inbuffer, &json_string);
            if (n < 0)
            {
                LOG(LogLevel::DEBUG) << "no way !!";
                return std::string();
            }
            if (n == 0)
            {
                LOG(LogLevel::INFO) << inbuffer << " parse done";
                return std::string();
            }
            // 2. 反序列化
            // 得到一个完整的报文jsonstring
            Response resp;
            if (!resp.Deserialize(json_string))
                return std::string();
            
            // 3. 回调处理
            if (_handler_response)
                _handler_response(resp);
        }
    }

    ~Protocol()
    {
    }

private:
    std::string _version;
    HandlerRequest_t _handler_request;
    HandlerResponse_t _handler_response;
};
