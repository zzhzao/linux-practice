#pragma once
#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include "InerAddr.hpp"
#include "Logger.hpp"
namespace NS_SOCKET_MODULE
{
    using namespace NS_LOG_MODULE;
    static const int gbacklog = 5;
    enum
    {
        OK = 0,
        SOCKET_ERR,
        BIND_ERR,
        LISTEN_ERR,
    };
    class Socket
    {
    public:
        Socket() {};
        ~Socket() {};

    protected:
        virtual void CreateSocketOrDie() = 0;
        virtual void BindSocketOrDie(uint16_t port) = 0;
        virtual void ListenSocketOrDie() = 0;

    public:
        virtual int Recv(std::string *out) = 0;
        virtual int Send(const std::string &in) = 0;
        virtual void Close() = 0;
        virtual bool Connect(InetAddr &addr) = 0;
    public:
        virtual std::shared_ptr<Socket> Accepter(InetAddr &clientaddr) = 0;
        void BulidTcpSocketMethod(uint16_t port)
        {
            CreateSocketOrDie();
            BindSocketOrDie(port);
            ListenSocketOrDie();
        }
        void BuildTcpClientSockMethod()
        {
            CreateSocketOrDie();
        }
        // void BulidUdpSocketMethod()
        // {
        //     CreateSocketOrDie();
        //     BindSocketOrDie();
        // }
    };
    class TcpSocket : public Socket
    {
    public:
        TcpSocket()
            : _sockfd(0)
        {
        }
        TcpSocket(int sockfd)
            : _sockfd(sockfd)
        {
        }
        ~TcpSocket() {};

    public:
        void CreateSocketOrDie() override
        {
            _sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (_sockfd < 0)
            {
                LOG(LogLevel::ERROR) << "创建套接字失败";
                exit(SOCKET_ERR);
            }
        }
        void BindSocketOrDie(uint16_t port) override
        {
            InetAddr localaddr(port);
            if (bind(_sockfd, localaddr.NetAddress(), localaddr.Len()) < 0)
            {
                LOG(LogLevel::ERROR) << "绑定套接字失败";
                exit(BIND_ERR);
            }
        }
        void ListenSocketOrDie() override
        {
            if (listen(_sockfd, gbacklog) < 0)
            {
                LOG(LogLevel::ERROR) << "监听套接字失败";
                exit(LISTEN_ERR);
            }
        }
        std::shared_ptr<Socket> Accepter(InetAddr &clientaddr) override
        {
            struct sockaddr_in addr;
            socklen_t len = sizeof(addr);
            int sockfd = accept(_sockfd, CONV(&addr), &len);
            if (sockfd < 0)
            {
                LOG(LogLevel::WARNING) << "accept error";
                return nullptr;
            }
            clientaddr = addr;
            return std::make_shared<TcpSocket>(sockfd);
        }
        int Sockfd()
        {
            return _sockfd;
        }
        int Recv(std::string *out) override
        {
            char inbuffer[1024];
            ssize_t n = recv(_sockfd, inbuffer, sizeof(inbuffer) - 1, 0);
            if (n > 0)
            {
                inbuffer[n] = 0;
                *out = inbuffer;
                return n;
            }
        }
        int Send(const std::string &in) override
        {
            return send(_sockfd, in.c_str(), in.size(), 0);
        }
        void Close() override
        {
            if (_sockfd >= 0)
            {
                close(_sockfd);
                _sockfd = -1;
            }
        }
        bool Connect(InetAddr & addr) override
        {
            int n = connect(_sockfd,addr.NetAddress(),addr.Len());
            if(n < 0)
                return false;
            else
                return true;
        }
    private:
        int _sockfd;
    };
    // class UdpSocket : public Socket
    // {
    // private:
    //     int sockfd_;
    // };
}