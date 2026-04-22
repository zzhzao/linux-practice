#pragma once

#include "Socket.hpp"
#include <cstddef>
#include <memory>
#include <stdint.h>
#include <poll.h>

using namespace NS_LOG_MODULE;
using namespace NS_SOCKET_MODULE;

static const int gdefaultfd = -1;
static const int gnum = 4096;
class PollServer {
public:
  PollServer(uint16_t port = 8080)
      : _port(port), _listensock(std::make_unique<TcpSocket>(port)),
        _quit(false) {
    _listensock->BuildTcpSocketMethod(port);
    LOG(LogLevel::DEBUG) << "create socket success, port: "
                         << _listensock->Sockfd();
    for (int i = 0; i < gnum; i++) {
      _fds[i].fd = gdefaultfd;
      _fds[i].events = 0;
      _fds[i].revents = 0;
    }
    _fds[0].fd = _listensock->Sockfd();
    _fds[0].events = POLLIN;
  }
  ~PollServer() {}

  void Accepter()
  {
// 监听套接字就绪
            LOG(LogLevel::WARNING) << "listensocketfd event ready";
            InetAddr clientaddr;
            int sockfd = _listensock->Accepter(clientaddr); // 在这里就不会卡住了
            LOG(LogLevel::DEBUG) << "get a new link, sockfd: " << sockfd;
            // 这时不能直接读取sockfd，因为sockfd是一个文件描述符，不能直接读取，需要使用select来等待读取事件
            // 保存sockfd到数组中
            int pos = 0;
            for (; pos < gnum; pos++) {
              if (_fds[pos].fd == gdefaultfd) {
                break;
              }
            }
            if (pos == gnum) {
              LOG(LogLevel::WARNING) << "server is full";
              close(sockfd);
            }
            else {
                _fds[pos].fd = sockfd;
                _fds[pos].events = POLLIN;
            }
  }

void Recver(int i)
    {
        // 普通fd就绪了！
        char buffer[1024];
        ssize_t n = recv(_fds[i].fd, buffer, sizeof(buffer), 0); // 这里读取会阻塞吗？？不会！！    
        if (n > 0)
        {
            buffer[n] = 0;
            LOG(LogLevel::DEBUG) << "buffer : " << buffer;
            std::string echo_string = "echo #";
            echo_string += buffer;
            // 我们可以直接发送数据吗？
            send(_fds[i].fd, echo_string.c_str(), echo_string.size(), 0);
        }
        else if (n == 0)
        {
            LOG(LogLevel::INFO) << "client quit: " << _fds[i].fd;
            // 0. 关闭fd
            close(_fds[i].fd);
            // 1. 从select移除掉fd
            _fds[i].fd = gdefaultfd;
        }
        else
        {
            LOG(LogLevel::WARNING) << "recv error: " << _fds[i].fd; 
            // 0. 关闭fd
            close(_fds[i].fd);
            // 1. 从select移除掉fd
            _fds[i].fd = gdefaultfd;
        }
    }

  void Dispatcher() {
    for (int i = 0; i < gnum; i++) {
      if (_fds[i].fd == gdefaultfd) {
        continue;
      } else {
        if (_fds[i].revents & POLLIN) {
          if (_fds[i].fd == _listensock->Sockfd()) {
            Accepter();
          }
          else
          {
            Recver(i);
          }
        }
       }
      }
      }
  
  void PrintFd()
    {
        std::cout << "select fd list: ";
        for(int i= 0; i < gnum; i++)
        {
            if(_fds[i].fd == gdefaultfd)
                continue;
            std::cout << _fds[i].fd << " ";
        }
        std::cout << std::endl;
    } 
  void Loop() {
    int timeout = 2000;
    while (!_quit) {


      // FD_SET(_listensock->Sockfd(), &rfds);
      // struct timeval timeout = {3, 0};
      int n =
          poll(_fds, sizeof(_fds)/sizeof(_fds[0]), timeout);

      switch (n) {
      case -1:
        LOG(LogLevel::WARNING) << "select error";
        break;
      case 0:
        LOG(LogLevel::INFO) << "select timeout";
        break;
      default:
        LOG(LogLevel::INFO) << "event ready";
        Dispatcher(); // 不仅仅处理新链接，还会处理普通链接
        break;
      }
    }
  }

private:
  uint16_t _port;
  std::unique_ptr<Socket> _listensock;
  bool _quit;

  struct pollfd _fds[gnum];

};