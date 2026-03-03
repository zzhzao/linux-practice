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
#include "InetAddr.hpp"
using namespace NS_LOG_MODULE;
const static int default_fd = -1;
const static int default_port = 8888;

using handler_addr_t = std::function<void(const InetAddr &)>;
using handler_msg_t = std::function<void(int sockfd, std::string msg)>;
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
    UdpServer(uint16_t port = default_port)
        : _port(port),
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

        // struct sockaddr_in local;
        // bzero(&local, sizeof(local));
        // local.sin_family = AF_INET;
        // local.sin_port = htons(_port);
        // local.sin_addr.s_addr = INADDR_ANY;

        InetAddr local(_port);
        int n = bind(_sockfd, (struct sockaddr *)&local,local.Len());
        if (n < 0)
        {
            LOG(LogLevel::FATAL) << "bind error";
            exit(BIND_ERR);
        }
        // LOG(LogLevel::INFO) << "bind success,ip: " << _ip << " port: " << _port;
    }
    void RegisterService(handler_addr_t handler,handler_msg_t handler_msg)
    {
        _handler_addr = handler;
        _handler_msg = handler_msg;
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
                InetAddr clientaddress(peer);
                _handler_addr(clientaddress);
                buffer[n] = '\0';   
                _handler_msg(_sockfd, buffer);
                
            }
        }
    }

private:
    int _sockfd;
    // std::string _ip;
    uint16_t _port;
    handler_addr_t _handler_addr;
    handler_msg_t _handler_msg;
};

#endif