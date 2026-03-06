#pragma once

#include <iostream>
#include <jsoncpp/json/json.h>
#include <functional>
#include "Logger.hpp"
#include<string.h>

using namespace NS_LOG_MODULE;
// 请求报文

class Request
{
public:
    Request() : _data_x(0),
                _data_y(0),
                _oper(0) {}
    Request(int x, int y, char oper)
        : _data_x(x),
          _data_y(y),
          _oper(oper)
    {
    }
    bool Serialize(std::string *out)
    {
        Json::Value root;
        root["left"] = _data_x;
        root["right"] = _data_y;
        root["oper"] = _oper;
        Json::FastWriter writer;
        *out = writer.write(root);

        return true;
    }
    bool DeSerialize(std::string &in)
    {
        Json::Value root;
        Json::Reader reader;
        bool parsesuccess = reader.parse(in, root);
        if (!parsesuccess)
        {
            std::cout << "解析失败" << std::endl;
            return false;
        }
        _data_x = root["left"].asInt();
        _data_y = root["right"].asInt();
        _oper = root["oper"].asInt();
    }
    ~Request() {}

public:
    int _data_x;
    int _data_y;
    char _oper;
};
// 应答报文
class Response
{
public:
    Response() {}
    Response(int result, int code)
        : _result(result),
          _code(code)
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
    bool DeSerialize(std::string &in)
    {
        Json::Value root;
        Json::Reader reader;
        bool parsesuccess = reader.parse(in, root);
        if (!parsesuccess)
        {
            std::cout << "解析失败" << std::endl;
            return false;
        }
        _result = root["result"].asInt();
        _code = root["code"].asInt();
    }
    ~Response() {}

public:
    int _result;
    int _code;
};
const std::string gsep = "\r\n";

using HandlerRequest_t = std::function<Response(Request &)>;
using HandlerResponse_t = std::function<std::string(Response &)>;
class Protocol
{
public:
    Protocol(HandlerRequest_t handler) : _version("1.0"), _handler_request(handler) {}
    Protocol(HandlerResponse_t handler_resp) : _version("1.0"), _handler_response(handler_resp) {}
    std::string Packet(const std::string &json_string)
    {
        return std::to_string(json_string.size()) + gsep + json_string + gsep;
    }
    int Unpack(std::string &packet, std::string *json_string)
    {
        if (packet.empty())
            return 0;
        if (json_string == nullptr)
            return -1;

        auto pos = packet.find(gsep);
        if (pos == std::string::npos)
            return 0;
        std::string lenstr = packet.substr(0, pos);
        int len = std::stoi(lenstr);

        int total = lenstr.size() + len + 2 * gsep.size();
        if (packet.size() < total)
        {
            return 0;
        }
        *json_string = packet.substr(pos + gsep.size(), len);
        packet.erase(0, total);
        return 1;
    }
    std::string ParseRequest(std::string &inbuffer)
    {
        std::string result;
        while (true)
        {
            std::string json_string;
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
            Request req;
            if (!req.DeSerialize(json_string))
                return std::string();
            Response resp;
            if (_handler_request)
                resp = _handler_request(req);

            std::string resp_json_string;
            resp.Serialize(&resp_json_string);

            result += Packet(resp_json_string);
            return result;
        }
    }
    std::string ParseResponse(std::string &inbuffer)
    {

        while (true)
        {
            std::string json_string;
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
            Response resp;
            if (!resp.DeSerialize(json_string))
                return std::string();

            if (_handler_response)
                _handler_response(resp);
        }
    }
        ~Protocol() {}

    private:
        std::string _version;
        HandlerRequest_t _handler_request;
        HandlerResponse_t _handler_response;
    
};