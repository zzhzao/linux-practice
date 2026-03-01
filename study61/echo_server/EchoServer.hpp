#ifndef __ECHOSERVER_HPP
#define __ECHOSERVER_HPP
#include <iostream>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include "Logger.hpp"
#include<stdlib.h>
#include<string>
#include<strings.h>
using namespace NS_LOG_MODULE;
const static int default_fd = -1;
const static int default_port = 8888;
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
    UdpServer(const std::string &ip,uint16_t port = default_port)
    :_port(port),
    _ip(ip),
    _sockfd(default_fd)
    {}
    ~UdpServer(){}
    void Init()
    {
        _sockfd = socket(AF_INET,SOCK_DGRAM,0);
        if(_sockfd < 0)
        {
            LOG(LogLevel::FATAL) << "create socket error";
            exit(SOCKET_ERR);
        }
        LOG(LogLevel::INFO) << "socket create success,sockfd: " << _sockfd ;
    
        struct sockaddr_in local;
        bzero(&local,sizeof(local));
        local.sin_family = AF_INET;
        local.sin_port = htons(_port);
        local.sin_addr.s_addr = inet_addr(_ip.c_str());

        int n = bind(_sockfd,(struct sockaddr*)&local,sizeof(local));
        if(n < 0)
        {
            LOG(LogLevel::FATAL) << "bind error";
            exit(BIND_ERR);
        }
        LOG(LogLevel::INFO) << "bind success,ip: " << _ip << " port: " << _port;

    }
    void Start()
    {
         char buffer[1024];
        while(true)
        {
           
            struct sockaddr_in peer;
            socklen_t len = sizeof(peer);
            ssize_t n = recvfrom(_sockfd,buffer,sizeof(buffer)-1,0,(struct sockaddr*)&peer,&len);
            if(n < 0)
            {
                LOG(LogLevel::ERROR) << "recvfrom error";
                continue;
            }
            buffer[n] = '\0';
            LOG(LogLevel::INFO) << "recvfrom success,peer: " << inet_ntoa(peer.sin_addr) << ":" << ntohs(peer.sin_port) << " msg: " << buffer;

            n = sendto(_sockfd,buffer,n,0,(struct sockaddr*)&peer,len);
            if(n < 0)
            {
                LOG(LogLevel::ERROR) << "sendto error";
                continue;
            }
            LOG(LogLevel::INFO) << "sendto success,peer: " << inet_ntoa(peer.sin_addr) << ":" << ntohs(peer.sin_port) << " msg: " << buffer;
        }
    }
private:
    int _sockfd;
    std::string _ip;
    uint16_t _port;
};

#endif