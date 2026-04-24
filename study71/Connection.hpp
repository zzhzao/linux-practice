#ifndef __CONNECTION_HPP__
#define __CONNECTION_HPP__
#include<iostream>
#include<string>
#include<ctime>
#include<cstdint>
#include<sys/epoll.h>
#include<sys/epoll.h>
#include"InetAddr.hpp"
class Reactor;

class Connection
{
public:
    Connection():_sockfd(-1),_events(EPOLLIN),_active_time(time(nullptr))
    {}
    Connection(int sockfd,uint32_t events):_sockfd(sockfd),_events(events),_active_time(time(nullptr))
    {}
    void Active()
    {
        _active_time = time(nullptr);
    }
    uint32_t Events()
    {
        return _events;
    }
    virtual int Sender() = 0;
    virtual int Recver() = 0;
    virtual int Excepter() = 0;
    
    int Sockfd()
    {
        return _sockfd;
    }
    void InitAddr(const InetAddr& addr)
    {
        _clientaddr = addr;
    }
    ~Connection()
    {}
    InetAddr Addr()
    {
        return _clientaddr;
    }
    void SetEvents(uint32_t events)
    {
        _events = events;
    }
protected:
    int _events;
    int _sockfd;

    std::string _inbuffer;  //接收缓冲区
    std::string _outbuffer;  //发送缓冲区
    time_t _active_time;  //活跃时间

    InetAddr _clientaddr;

public:
    Reactor *R;   //回指指针
};
// class NormalConnection : public Connection
// {
// public:
//     NormalConnection(int sockfd):Connection(sockfd)
//     {}
// };
// class ListenConnection : public Connection
// {
// public:
//     ListenConnection(int sockfd):Connection(sockfd)
//     {}
// };
#endif