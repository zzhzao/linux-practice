#ifndef __IOMANAGER_HPP
#define __IOMANAGER_HPP
#include "Connection.hpp"
#include "Comm.hpp"

#include <functional>
using on_message_t = std::function<std::string(std::string &)>;

static const int buffersize = 1024;

class IOManager : public Connection
{
public:
    IOManager(int sockfd, uint32_t events, on_message_t on_message) : Connection(sockfd, events), _on_message(on_message)
    {
        Active();
    }
    ~IOManager() {}
    int Sender() override
    {
        Active();
        while (true)
        {
            ssize_t n = send(_sockfd, _outbuffer.c_str(), _outbuffer.size(), 0);
            if (n > 0)
            {
                _outbuffer.erase(0, n);
                if (_outbuffer.empty())
                    break;
            }
            else
            {
                if (errno == EWOULDBLOCK || errno == EAGAIN)
                {
                    break;
                }
                else if (errno = EINTR)
                {
                    continue;
                }
                else
                {
                    LOG(LogLevel::INFO) << "send client: " << _clientaddr.ToString() << " error, sockfd: " << _sockfd;
                    Excepter();
                    return -1;
                }
            }
        }
        // 缓冲区让写满了    /  _outbuffer发完了
        if (!_outbuffer.empty())
        {
            // 写条件不满足  -> 开启sockfd对些时间的关心
            R->EnableReadWrite(_sockfd, true, true);
        }
        else
        {
            R->EnableReadWrite(_sockfd, true, false);
        }

        return 0;
    }
    int Recver() override
    {
        Active();
        LOG(LogLevel::DEBUG) << "事件派发到了IOManager";
        // 实现非阻塞读取
        char buffer[buffersize];
        while (true)
        {
            ssize_t n = recv(_sockfd, buffer, buffersize - 1, 0);
            if (n > 0)
            {
                buffer[n] = 0;
                _inbuffer += buffer;
            }
            else if (n == 0)
            {
                LOG(LogLevel::INFO) << "client:" << _clientaddr.ToString() << " quit";
                Excepter();
                return -1;
            }
            else
            {
                if (errno == EWOULDBLOCK || errno == EAGAIN)
                {
                    break;
                }
                if (errno == EINTR)
                {
                    continue;
                }
                else
                {
                    Excepter();
                    LOG(LogLevel::INFO) << "client:" << _clientaddr.ToString() << " error,sockfd: " << _sockfd;
                    return -2;
                }
            }
        }
        // 读完了
        LOG(LogLevel::DEBUG) << "本轮数据读完 inbuffer:" << _inbuffer;
        // inbuffer交给应用层协议处理
        // 1.怎么交给上层协议
        // 2.上层协议是谁
        if (_on_message)
        {
            _outbuffer += _on_message(_inbuffer);
        }

        //最佳实践   1
        // if (!_outbuffer.empty())
        // {
        //     Sender();
        // }

        if(!_outbuffer.empty())
        {
            R->EnableReadWrite(_sockfd,true,true);
        }



        return _inbuffer.size();
    }
    int Excepter() override
    {
        //归一化异常处理
        LOG(LogLevel::WARNING) << "errno : "<< errno << strerror(errno);
        R->DelConnection(_sockfd);
        return 0;
    }

private:
    on_message_t _on_message;
};

#endif