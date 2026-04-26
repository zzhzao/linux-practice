#ifndef __TCPSERVER_HPP
#define __TCPSERVER_HPP
#include <iostream>
#include "Socket.hpp"
#include "Logger.hpp"
#include <cstdint>
#include <memory>
#include "Epoller.hpp"
#include <unordered_map>
#include <vector>
#include "Connection.hpp"

using namespace NS_LOG_MODULE;
using namespace NS_SOCKET_MODULE;

static const int grevs_num = 64;
class Reactor
{
private:
    bool IsLegalConnection(int sockfd)
    {
        return _connections.find(sockfd) != _connections.end();
    }

public:
    Reactor() : _epoller(std::make_unique<Epoller>())
    {

        // //添加监听fd到epoll
        // _epoller->AddEvent(_listensock->Sockfd(),EPOLLIN);
    }
    ~Reactor()
    {
    }
    void Dispatcher(int n)
    {
        for (int i = 0; i < n; i++)
        {
            int sockfd = revs[i].data.fd;
            uint32_t revents = revs[i].events;
            if (revents & EPOLLERR)
            {
                revents |= (EPOLLIN | EPOLLOUT);
            }
            if (revents & EPOLLHUP)
            {
                revents |= (EPOLLIN | EPOLLOUT);
            }
            if ((revents & EPOLLIN) && IsLegalConnection(sockfd))
            {
                _connections[sockfd]->Recver();
            }
            if ((revents & EPOLLOUT) && IsLegalConnection(sockfd))
            {
                _connections[sockfd]->Sender();
            }
        }
    }
    void LoopOnce(int timeout = 1000)
    {
        int n = _epoller->WaitEvents(revs, grevs_num, timeout);
        switch (n)
        {
        case 0:
            LOG(LogLevel::INFO) << "epoll_wait timeout";
            break;
        case -1:
            LOG(LogLevel::FATAL) << "epoll_wait error";
            exit(EXIT_CODE::EPOLLER_WAIT_FATAL);
            break;
        default:
            LOG(LogLevel::INFO) << "event ready...";
            Dispatcher(n);
            break;
        }
    }
    int CheckExpriedLink()
    {
        // 一个链接如果超过100s没有活动，这个链接我就把它过期，关掉
        int timeout = 100000;
        time_t currenttime = time(nullptr);
        for(auto & conn:_connections)
        {
            time_t last_active = conn.second->lastActiveTime();
            time_t timediff = currenttime - last_active;
            if(timediff > 100000)
            {
                DelConnection(conn.second->Sockfd());
            }
            time_t expiredtime = 100000 - timediff;
            if(timeout > expiredtime)
            {
                timeout = expiredtime;
            }
        }
        return timeout;
    }
    void Loop()
    {
        int timeout = 2000;
        while (true)
        {
            LoopOnce(timeout);
            //DebugPrint();

            //链接管理
            CheckExpriedLink();
        }
    }
    void DebugPrint()
    {
        LOG(LogLevel::DEBUG) << "user in reactor: ";
        for (auto &conn : _connections)
        {
            LOG(LogLevel::DEBUG) << conn.second->Sockfd() << "," << conn.second->Addr().ToString();
        }
    }

public:
    void AddConnection(std::shared_ptr<Connection> conn)
    {
        // 1. 新链接写透到内核中
        _epoller->AddEvent(conn->Sockfd(), conn->Events());
        // 2. 新链接添加到_connections中
        _connections[conn->Sockfd()] = conn;
        // 3. 让conn指向reactor对象
        conn->R = this;
    }
    void EnableReadWrite(int sockfd,bool readable,bool writeable)
    {
        if(IsLegalConnection(sockfd))
        {
            uint32_t events = EPOLLET | (readable?EPOLLIN:0)|(writeable?EPOLLOUT:0);
            _connections[sockfd]->SetEvents(events);
            //写到内核中
            _epoller->ModEvents(_connections[sockfd]->Sockfd(),_connections[sockfd]->Events());
        }
    }
    void DelConnection(int sockfd)
    {
        if(IsLegalConnection(sockfd))
        {
            // epoll中移除，穿透内核
            _epoller->DelEvents(sockfd);
            // 关闭文件描述符
            _connections[sockfd]->Close();
            // _connections 移除
            _connections.erase(sockfd);

        }

    }

private:
    std::unique_ptr<Epoller> _epoller;
    struct epoll_event revs[grevs_num];
    std::unordered_map<int, std::shared_ptr<Connection>> _connections;
};

#endif