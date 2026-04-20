#pragma once

#include "Socket.hpp"
#include <cstddef>
#include <memory>
#include <stdint.h>
#include <sys/select.h>

using namespace NS_LOG_MODULE;
using namespace NS_SOCKET_MODULE;
static const int gfdnum = sizeof(fd_set) * 8;
static const int gdefaultfd = -1;
class selectServer {
public:
  selectServer(uint16_t port = 8080)
      : _port(port), _listensock(std::make_unique<TcpSocket>(port)),
        _quit(false) {
    _listensock->BuildTcpSocketMethod(port);
    LOG(LogLevel::DEBUG) << "create socket success, port: "
                         << _listensock->Sockfd();
    for (int i = 0; i < gfdnum; i++) {
      _rfdset[i] = gdefaultfd;
    }
  }
  ~selectServer() {}

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
            for (; pos < gfdnum; pos++) {
              if (_rfdset[pos] == gdefaultfd) {
                break;
              }
            }
            if (pos == gfdnum) {
              LOG(LogLevel::WARNING) << "server is full";
              close(sockfd);
            }
            else {
                _rfdset[pos] = sockfd;
            }
  }

void Recver(int i)
    {
        // 普通fd就绪了！
        char buffer[1024];
        ssize_t n = recv(_rfdset[i], buffer, sizeof(buffer), 0); // 这里读取会阻塞吗？？不会！！
        if (n > 0)
        {
            buffer[n] = 0;
            LOG(LogLevel::DEBUG) << "buffer : " << buffer;
            std::string echo_string = "echo #";
            echo_string += buffer;
            // 我们可以直接发送数据吗？
            send(_rfdset[i], echo_string.c_str(), echo_string.size(), 0);
        }
        else if (n == 0)
        {
            LOG(LogLevel::INFO) << "client quit: " << _rfdset[i];
            // 0. 关闭fd
            close(_rfdset[i]);
            // 1. 从select移除掉fd
            _rfdset[i] = gdefaultfd;
        }
        else
        {
            LOG(LogLevel::WARNING) << "recv error: " << _rfdset[i];
            // 0. 关闭fd
            close(_rfdset[i]);
            // 1. 从select移除掉fd
            _rfdset[i] = gdefaultfd;
        }
    }

  void Dispatcher(fd_set &rfds) {
    for (int i = 0; i < gfdnum; i++) {
      if (_rfdset[i] == gdefaultfd) {
        continue;
      } else {
        if (FD_ISSET(_rfdset[i], &rfds)) {
          if (_rfdset[i] == _listensock->Sockfd()) {
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
        for(int i= 0; i < gfdnum; i++)
        {
            if(_rfdset[i] == gdefaultfd)
                continue;
            std::cout << _rfdset[i] << " ";
        }
        std::cout << std::endl;
    } 
  void Loop() {
    while (!_quit) {
      fd_set rfds;
      FD_ZERO(&rfds);
      int maxfd = gdefaultfd;
      for (int i = 0; i < gfdnum; i++) {
        if (_rfdset[i] == gdefaultfd) {
          continue;
        }
        FD_SET(_rfdset[i], &rfds);
        // 找到最大的fd
        if (maxfd < _rfdset[i]) {
          maxfd = _rfdset[i];
        }
      }

      // FD_SET(_listensock->Sockfd(), &rfds);
      // struct timeval timeout = {3, 0};
      int n =
          select(maxfd + 1, &rfds, nullptr, nullptr, nullptr);

      switch (n) {
      case -1:
        LOG(LogLevel::WARNING) << "select error";
        break;
      case 0:
        LOG(LogLevel::INFO) << "select timeout";
        break;
      default:
        LOG(LogLevel::INFO) << "event ready";
        Dispatcher(rfds); // 不仅仅处理新链接，还会处理普通链接
        break;
      }
    }
  }

private:
  uint16_t _port;
  std::unique_ptr<Socket> _listensock;
  bool _quit;
  int _rfdset[gfdnum]; // 保存所有可读的fd集合

};