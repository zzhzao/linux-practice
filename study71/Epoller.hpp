#ifndef __EPOLLER_HPP__
#define __EPOLLER_HPP__
#include<iostream>
#include<cstdlib>
#include<sys/epoll.h>
#include"Logger.hpp"
#include"Comm.hpp"
using namespace NS_LOG_MODULE;

//#define READ_EVENT EPOLLIN


class Epoller
{
private:
    bool EpollCtl(int sockfd,int oper,uint32_t events)
    {
        if(oper == EPOLL_CTL_DEL)
        {
            int n = epoll_ctl(_epfd,oper,sockfd,nullptr);
            (void)n;
        }
        else {
            struct epoll_event ev;
            ev.events = events;
            ev.data.fd = sockfd;
            int n = epoll_ctl(_epfd,oper,sockfd,&ev);
            (void)n;
        }
        return true;
    }
public:
    Epoller()
    {
        _epfd = epoll_create(256);
        if(_epfd < 0)
        {
            LOG(LogLevel::FATAL)<<"epoll_create1 error";
            exit(EXIT_CODE::EPOLLER_ERROR);
        }
        LOG(LogLevel::INFO)<<"epoll_create1 success,_epfd="<<_epfd;
    }
    int WaitEvents(struct epoll_event revs[],int event_num,int timeout = 1000)
    {
        return epoll_wait(_epfd,revs,event_num,timeout);

    }
    bool AddEvent(int sockfd,uint32_t events)
    {
        return EpollCtl(sockfd,EPOLL_CTL_ADD,events);
    }
    bool ModEvents(int sockfd,uint32_t events)
    {
        return EpollCtl(sockfd,EPOLL_CTL_MOD,events);
    } 
    ~Epoller()
    {
        if(_epfd > 0)
        {
            close(_epfd);
        }
    }
private:
    int _epfd;
};

#endif


// class BasePoller
// {
// public:
//     virtual ~BasePoller()=default;
//     virtual void Wait()=0;
//     virtual void Ctl()=0;
// };

// class Epoller:public BasePoller
// {
// public:
//     Epoller()=default;
//     ~Epoller()=default;
//     void Wait()override;
//     void Ctl()override;
// };
// class Selecter:public BasePoller
// {
// public:
//     Selecter()=default;
//     ~Selecter()=default;
//     void Wait()override;
//     void Ctl()override;
// };
// class Poller:public BasePoller
// {
// public:
//     Poller()=default;
//     ~Poller()=default;
//     void Wait()override;
//     void Ctl()override;
// };