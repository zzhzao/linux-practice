#pragma once


#include <unistd.h>
#include <cstdlib>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "Logger.hpp"
#include "InetAddr.hpp"

namespace NS_SOCKET_MODULE
{
    using namespace NS_LOG_MODULE;

    static const int gbacklog = 16;

    enum
    {
        OK = 0,
        SOCKET_ERR,
        BIND_ERR,
        LISTEN_ERR
    };

    // 模版方法模式！
    class Socket
    {
    public:
        ~Socket()
        {
        }

    protected:
        virtual void CreateSocketOrDie() = 0;
        virtual void BindSocketOrDie(uint16_t port) = 0;
        virtual void ListenSocketOrDie() = 0;
        // virtual ssize_t Recv() = 0;
        // virtual void Send() = 0;
    public:
        // virtual std::shared_ptr<Socket> Accepter(InetAddr &addr) = 0;
        virtual int Accepter(InetAddr &addr) = 0;
        virtual int Sockfd() = 0;
        virtual int Recv(std::string *out) = 0;
        virtual int Send(const std::string &in) = 0;
        virtual void Close() = 0;
        virtual bool Connect(InetAddr &addr) = 0;
    public:
        void BuildTcpSocketMethod(uint16_t port) // 模版方法
        {
            CreateSocketOrDie();
            BindSocketOrDie(port);
            ListenSocketOrDie();
        }
        void BuildTcpClientSockMethod()
        {
            CreateSocketOrDie();
        }
        // void BuildUdpSocketMethod()
        // {
        //     CreateSocketOrDie();
        //     BindSocketOrDie();
        // }
    };

    class TcpSocket : public Socket
    {
    public:
        TcpSocket() : _sockfd(0)
        {
        }
        TcpSocket(int sockfd): _sockfd(sockfd)
        {}
        void CreateSocketOrDie() override
        {
            _sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (_sockfd < 0)
            {
                LOG(LogLevel::FATAL) << "create socket error";
                exit(SOCKET_ERR);
            }
            int opt = 1;
            setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        }
        void BindSocketOrDie(uint16_t port) override
        {
            InetAddr addr(port);
            if (bind(_sockfd, addr.NetAddress(), addr.Len()) != 0)
            {
                LOG(LogLevel::FATAL) << "bind socket error";
                exit(BIND_ERR);
            }
        }
        void ListenSocketOrDie() override
        {
            if (listen(_sockfd, gbacklog) != 0)
            {
                LOG(LogLevel::FATAL) << "listen socket error";
                exit(LISTEN_ERR);
            }
        }
        // std::shared_ptr<Socket> Accepter(InetAddr &clientaddr) override
        int Accepter(InetAddr &clientaddr) override
        {
            struct sockaddr_in addr;
            socklen_t len = sizeof(addr);
            int sockfd = accept(_sockfd, CONV(&addr), &len);
            if(sockfd < 0)
            {
                LOG(LogLevel::WARNING) << "accept error";
                // return nullptr;
                return -1;
            }
            clientaddr = addr;
            // return std::make_shared<TcpSocket>(sockfd);
            return sockfd;
        }
        int Sockfd() override
        {
            return _sockfd;
        }

        int Recv(std::string *out) override
        {
            char inbuffer[1024];
            ssize_t n = recv(_sockfd, inbuffer, sizeof(inbuffer)-1, 0);
            if(n > 0)
            {
                inbuffer[n] = 0;
                *out += inbuffer; // 追加写入的
            }
            return n;
        }
        
        int Send(const std::string &in) override
        {
            return send(_sockfd, in.c_str(), in.size(), 0);
        }
        void Close() override
        {
            if(_sockfd>=0)
            {
                close(_sockfd);
                _sockfd = -1;
            }
        }
        bool Connect(InetAddr &addr) override
        {
            int n = connect(_sockfd, addr.NetAddress(), addr.Len());
            if(n < 0)
                return false;
            else
                return true;
        }

        ~TcpSocket() {}

    private:
        int _sockfd;
    };

    // class UdpSocket: public Socket
    // {

    // };

} // namespace name