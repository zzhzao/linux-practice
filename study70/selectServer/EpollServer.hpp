#pragma once
#include<iostream>
#include<sys/epoll.h>
#include<memory>

#include"Socket.hpp"
#include "Logger.hpp"
#include "InetAddr.hpp"

using namespace NS_SOCKET_MODULE;
using namespace NS_LOG_MODULE;

static const uint16_t defaultport = 8080;
static const int ign_size = 256;
static const int revs_num = 1000;
class EpollServer
{
public:
    EpollServer(uint16_t port = defaultport)
    :_port(port)
    ,_listensock(std::make_unique<TcpSocket>())
    ,_epfd(-1)
    ,_quit(false)
    {
        _listensock->BuildTcpSocketMethod(_port);
        LOG(LogLevel::INFO) << "create listen socket success ,socket : " << _listensock->Sockfd();

        //创建epoll模型
        _epfd = epoll_create(ign_size);
        if(_epfd < 0)
        {
            LOG(LogLevel::FATAL) << "epoll_create1 failed";
            return;
        }
        LOG(LogLevel::INFO) << "create epoll model success ,epfd : " << _epfd;
        // 把listensockfd添加到epoll模型中
        struct epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.fd = _listensock->Sockfd();
        int n = epoll_ctl(_epfd,EPOLL_CTL_ADD,_listensock->Sockfd(),&ev);
        if(n == 0)
        {
            LOG(LogLevel::INFO) << "add listen socket to epoll model success";
        }
    }
    void Listener()
    {
        InetAddr clientaddr;
        int newsockfd = _listensock->Accepter(clientaddr);
        if(newsockfd < 0)
        {
            LOG(LogLevel::ERROR) << "accept failed";
            return;
        }
        LOG(LogLevel::INFO) << "accept success ,newsockfd : " << newsockfd;
        //不能直接读取数据，要托管给epoll,添加到rb中
        struct epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.fd = newsockfd;
        int n = epoll_ctl(_epfd,EPOLL_CTL_ADD,newsockfd,&ev);
        if(n == 0)
        {
            LOG(LogLevel::INFO) << "add new socket to epoll model success";
        }
    }
    void IOService(int sockfd)
    {
        // 处理其他sockfd的事件
        char inbuffer[1024];
        ssize_t n = recv(sockfd, inbuffer, sizeof(inbuffer), 0);
        if(n > 0)
        {
            inbuffer[n] = 0;
            LOG(LogLevel::INFO) << "recv from client : " << inbuffer;
            std::string echo_string = "echo # ";
            echo_string += inbuffer;
            send(sockfd,echo_string.c_str(),echo_string.size(),0);  // 可以直接发
        }
        else if(n == 0)
        {
            LOG(LogLevel::INFO) << " client close";
            // 1. 不让epoll关心fd
            int n = epoll_ctl(_epfd,EPOLL_CTL_DEL,sockfd,NULL);
            (void)n;
            close(sockfd);
        }
        else
        {
            LOG(LogLevel::ERROR) << "recv failed";
            int n = epoll_ctl(_epfd,EPOLL_CTL_DEL,sockfd,NULL);
            (void)n;
            close(sockfd);
        }

    }
    void HanderEvent(struct epoll_event revs[],int n)
    {
        for(int i = 0;i < n;i++)
        {
            uint32_t revents = revs[i].events;
            int sockfd = revs[i].data.fd;
            if(revents & EPOLLIN)
            {
                // 读事件就绪
                if(sockfd == _listensock->Sockfd())
                {
                    // 处理listensockfd的事件
                    Listener();
                }
                else
                {
                    // 处理其他sockfd的事件
                    IOService(sockfd);
                }
            }
            // else if(revents & EPOLLOUT)
            // {
            //     // 处理listensockfd的事件
            // }
            // else if(revents & EPOLLERR)
            // {
            //     // 处理listensockfd的事件
            // }

        }
    }
    void Loop()
    {
        int timeout = -1;
        while(!_quit)
        {
            struct epoll_event events[revs_num];
            int n = epoll_wait(_epfd,events,revs_num,timeout);
            if(n > 0)
            {
                LOG(LogLevel::INFO) << "event Ready!";
                HanderEvent(events,n);
            }
            else if(n == 0)
            {
                LOG(LogLevel::INFO) << "epoll_wait timeout";
            }
            else {
                LOG(LogLevel::ERROR) << "epoll_wait failed";
            }
        }
    }


    ~EpollServer()
    {
    }
private:
    std::unique_ptr<Socket> _listensock;
    uint16_t _port;
    int _epfd;
    bool _quit;
};
