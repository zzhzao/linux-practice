#ifndef __LISTENER_HPP
#define __LISTENER_HPP

#include <iostream>
#include "Connection.hpp"

#include "Logger.hpp"
#include "Socket.hpp"
#include <memory>
#include "Epoller.hpp"
#include "Comm.hpp"
#include <sys/epoll.h>
#include "IOManager.hpp"

using namespace NS_SOCKET_MODULE;
static const int gdefaultport = 8080;
class Listener : public Connection
{
public:
    Listener(on_message_t on_message_helper,uint16_t port = gdefaultport) : 
    _port(port),
    _listensock(std::make_unique<TcpSocket>()),
    _on_message_helper(on_message_helper)
    {
        _listensock->BuildTcpSocketMethod(_port);
        _sockfd = _listensock->Sockfd();
        _events = EPOLLIN | EPOLLET;
        Active();
    }
    ~Listener()
    {
    }
    int Sender() override
    {
    }
    int Recver() override
    {
        LOG(LogLevel::DEBUG) << "事件派发到了Listener";
        while (true)
        {
            InetAddr clientaddr;
            int errcode = 0;
            int sockfd = _listensock->Accepter(clientaddr,&errcode);
            if (sockfd < 0)
            {
                if(errcode == EWOULDBLOCK || errcode == EAGAIN)
                {
                    LOG(LogLevel::INFO) << "accepter all client done";
                    break;
                }
                else if(errcode == EINTR)
                {
                    continue;
                }
                else
                {
                    LOG(LogLevel::WARNING) << "accept error";
                }
            }
            std::shared_ptr<Connection> conn = std::make_shared<IOManager>(sockfd, EPOLLIN | EPOLLET,_on_message_helper);
            // 设置IOManager到reactor中
            // 前向声明Reactor类，解决不完全类型问题
            conn->InitAddr(clientaddr);
            R->AddConnection(conn);
            LOG(LogLevel::INFO) << "获取一个新链接，新链接sockfd: " << sockfd << "client addr" << clientaddr.ToString();
        }
        return 0;
    }
    int Excepter() override
    {
    }

private:
    uint16_t _port;
    std::unique_ptr<Socket> _listensock;
    on_message_t _on_message_helper;
};

#endif
