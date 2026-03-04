#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include "Logger.hpp"
#include <memory>
#include <cstring>
#include "InetAddr.hpp"
#include <string>
#include <signal.h>
#include <sys/wait.h>
#include <pthread.h>
#include"ThreadPool.hpp"
#include<functional>
using namespace NS_LOG_MODULE;
using namespace NS_THREAD_POOL_MODULE; 

using task_t = std::function<void()>;

enum
{
    SUCCESS = 0,
    USAGE_ERR,
    SOCKET_ERR,
    BIND_ERR,
    LISTEN_ERR,
    FORK_ERR
};

static const int gbacklog = 16;
static const uint16_t gport = 8888;
class TcpServer
{
public:
    TcpServer(uint16_t port = gport) : _port(port)
    {
    }
    ~TcpServer() { close(_listensockfd); }
    void InitServer()
    {
        _listensockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (_listensockfd < 0)
        {
            LOG(LogLevel::FATAL) << "socket error";
            exit(SOCKET_ERR);
        }
        struct sockaddr_in local;
        memset(&local, 0, sizeof(local));
        local.sin_family = AF_INET;
        local.sin_port = htons(_port);
        local.sin_addr.s_addr = INADDR_ANY;

        int n = bind(_listensockfd, (struct sockaddr *)&local, sizeof(local));
        if (n < 0)
        {
            LOG(LogLevel::FATAL) << "bind error";
            exit(BIND_ERR);
        }
        LOG(LogLevel::DEBUG) << "bind socket success";
        n = listen(_listensockfd, gbacklog);
        if (n < 0)
        {
            LOG(LogLevel::FATAL) << "listen error";
            exit(LISTEN_ERR);
        }
        LOG(LogLevel::DEBUG) << "listen socket success";
    }
    void serviceIO(int sockfd, InetAddr clientaddress)
    {
        LOG(LogLevel::INFO) << "service client: " << clientaddress.ToString();
        while (true)
        {
            // char inbuffer[1024];
            // ssize_t n = read(sockfd, inbuffer, sizeof(inbuffer) - 1);
            // if (n > 0)
            // {
            //     inbuffer[n] = '\0';
            //     LOG(LogLevel::INFO) << "recv client: " << clientaddress.ToString() << " say: " << inbuffer;
            //     std::string echo_string = "service echo: " + std::string(inbuffer);
            //     write(sockfd, echo_string.c_str(), echo_string.size());
            // }
            // else if (n == 0)
            // {
            //     LOG(LogLevel::INFO) << "client: " << clientaddress.ToString() << " closed";
            //     break;
            // }
            // else
            // {
            //     LOG(LogLevel::ERROR) << "read error";
            //     break;
            // }
            char inbuffer[1024];
            ssize_t n = read(sockfd, inbuffer, sizeof(inbuffer) - 1);
            if (n > 0)
            {
                inbuffer[n] = '\0';
                LOG(LogLevel::INFO) << "recv client: " << clientaddress.ToString() << " say: " << inbuffer;
                std::string echo_string = "service echo: " + std::string(inbuffer);
                write(sockfd, echo_string.c_str(), echo_string.size());
            }
            else if (n == 0)
            {
                LOG(LogLevel::INFO) << "client: " << clientaddress.ToString() << " closed";
                break;
            }
            else
            {
                LOG(LogLevel::ERROR) << "read error";
                break;
            }
            close(sockfd);
        }
    }
    class ThreadData
    {
    public:
        ThreadData(TcpServer *tsvr, int sockfd, const InetAddr &clientaddress) : _tsvr(tsvr), _sockfd(sockfd), _clientaddress(clientaddress)
        {
        }

    public:
        TcpServer *_tsvr;
        int _sockfd;
        InetAddr _clientaddress;
    };
    static void *thread_routine(void *arg)
    {
        ThreadData *td = (ThreadData *)arg;
        td->_tsvr->serviceIO(td->_sockfd, td->_clientaddress);
        delete td;
        return nullptr;
    }
    void Start()
    {
        signal(SIGCHLD, SIG_IGN);
        while (true)
        {
            struct sockaddr_in clientaddr;
            socklen_t len = sizeof(clientaddr);
            int sockfd = accept(_listensockfd, (struct sockaddr *)&clientaddr, &len);
            if (sockfd < 0)
            {
                LOG(LogLevel::ERROR) << "accept error";
                continue;
            }
            LOG(LogLevel::INFO) << "accept a new client: " << inet_ntoa;
            // InetAddr clientaddress(clientaddr);
            // serviceIO(sockfd,clientaddress);

            //   verson1
            // pid_t id = fork();
            // if (id < 0)
            // {
            //     LOG(LogLevel::ERROR) << "fork error";
            //     exit(FORK_ERR);
            // }
            // else if (id == 0)
            // {
            //     // if(fork() > 0)
            //     // {
            //     //     exit(0);
            //     // }
            //     close(_listensockfd);
            //     InetAddr clientaddress(clientaddr);
            //     serviceIO(sockfd, clientaddress);
            //     close(sockfd);
            //     exit(0);
            // }
            // else
            // {
            //     // pid_t rid = waitpid(id,nullptr,0);
            //     // (void)rid;
            //     close(sockfd);
            // }

            // versno2
            // pthread_t tid;
            // InetAddr clientaddress(clientaddr);
            // ThreadData *td = new ThreadData(this, sockfd, clientaddress);

            // pthread_create(&tid, nullptr, thread_routine, (void *)td);

            // verson 3 接入线程池
            InetAddr clientaddress(clientaddr);
            ThreadPool<task_t>::Instance()->Enqueue([this,sockfd,clientaddress]()->void{this->serviceIO(sockfd,clientaddress);});
        }
    }

private:
    uint16_t _port;
    int _listensockfd;
};