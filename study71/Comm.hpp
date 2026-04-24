#ifndef __COMMON_HPP
#define __COMMON_HPP

#include<iostream>
#include<fcntl.h>
#include"Logger.hpp"
using namespace NS_LOG_MODULE;
enum EXIT_CODE
{
    SUCCESS,
    EPOLLER_ERROR,
    EPOLLER_WAIT_FATAL,

};

void SetNonBlock(int sockfd)
{
    int fl = fcntl(sockfd,F_GETFL);
    if(fl < 0)
    {
        return;
    }
    fcntl(sockfd,F_SETFL,fl|O_NONBLOCK);
}
#endif