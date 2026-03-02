#ifndef __ECHOSERVER_HPP
#define __ECHOSERVER_HPP
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Logger.hpp"
#include <stdlib.h>
#include <string>
#include<functional>
#include <strings.h>
using namespace NS_LOG_MODULE;
const static int default_fd = -1;
const static int default_port = 8888;

using callback_t = std::function<std::string(std::string)>;


enum
{
    SUCCESS = 0,
    SOCKET_ERR,
    USAGE_ERR,
    BIND_ERR,
};
class UdpServer
{
public:
    UdpServer(uint16_t port = default_port,callback_t cb = nullptr)
        : _port(port),
          _cb(cb),
          //_ip(ip),
          _sockfd(default_fd)
    {
    }
    ~UdpServer() { close(_sockfd); }
    void Init()
    {
        _sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (_sockfd < 0)
        {
            LOG(LogLevel::FATAL) << "create socket error";
            exit(SOCKET_ERR);
        }
        LOG(LogLevel::INFO) << "socket create success,sockfd: " << _sockfd;

        struct sockaddr_in local;
        bzero(&local, sizeof(local));
        local.sin_family = AF_INET;
        local.sin_port = htons(_port);
        local.sin_addr.s_addr = INADDR_ANY;

        int n = bind(_sockfd, (struct sockaddr *)&local, sizeof(local));
        if (n < 0)
        {
            LOG(LogLevel::FATAL) << "bind error";
            exit(BIND_ERR);
        }
        // LOG(LogLevel::INFO) << "bind success,ip: " << _ip << " port: " << _port;
    }
    void Start()
    {
        char buffer[1024];
        while (true)
        {

            struct sockaddr_in peer;
            socklen_t len = sizeof(peer);
            ssize_t n = recvfrom(_sockfd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&peer, &len);
            
            if (n <= 0)
            {
                LOG(LogLevel::ERROR) << "recvfrom error";
                continue;
            }
            else
            {
                uint16_t client_port = ntohs(peer.sin_port);
                std::string client_ip = inet_ntoa(peer.sin_addr);
                std::string client_address = "['" + client_ip + ":" + std::to_string(client_port) + "']";
                buffer[n] = '\0';
                //LOG(LogLevel::DEBUG) << client_address << " say: " << buffer;
                // std::string echo_msg = "server echo: " + std::string(buffer);
                // n = sendto(_sockfd, echo_msg.c_str(), echo_msg.size(), 0, (struct sockaddr *)&peer, len);
                // LOG(LogLevel::INFO) << "sendto success,peer: " << inet_ntoa(peer.sin_addr) << ":" << ntohs(peer.sin_port) << " msg: " << buffer;
                std::string result = _cb(buffer);
                sendto(_sockfd, result.c_str(), result.size(), 0, (struct sockaddr *)&peer, len);
            }
        }
    }

private:
    int _sockfd;
    // std::string _ip;
    uint16_t _port;
    callback_t _cb;
};

#endif