#include "Socket.hpp"
#include "InetAddr.hpp"
#include "Logger.hpp"
#include <memory>
#include <unistd.h>
#include <signal.h>
#include <functional>
static const uint16_t gport = 8080;

using namespace NS_SOCKET_MODULE;
using namespace NS_LOG_MODULE;

using Handler_t = std::function<std::string(std::string &)>;
class TcpServer
{
public:
    TcpServer(Handler_t handler, uint16_t port = gport)
        : _port(port),
          _listensock(std::make_unique<TcpSocket>()),
          _handler(handler)
    {
        _listensock->BulidTcpSocketMethod(_port);
    }
    void Loop()
    {
        signal(SIGCHLD, SIG_IGN);
        while (true)
        {
            InetAddr clientaddr;
            auto sockfd = _listensock->Accepter(clientaddr);
            if (!sockfd)
            {
                continue;
            }

            LOG(LogLevel::DEBUG) << "get a new link" << clientaddr.ToString();
            if (fork() == 0)
            {

                service(sockfd, clientaddr);
                sockfd->Close();
                exit(0);
            }
        }
    }

    ~TcpServer()
    {
    }

private:
    void service(std::shared_ptr<Socket> sockfd, InetAddr &clientaddr)
    {
        std::string inbuffer, outbuffer;

        outbuffer.clear();
        int n = sockfd->Recv(&inbuffer);
        if (n <= 0)
        {
            LOG(LogLevel::WARNING) << "recv: client quit," << clientaddr.ToString();
        }
        if (_handler)
            outbuffer = _handler(inbuffer);
        if (outbuffer.empty())
            return;
        n = sockfd->Send(outbuffer);
        if (n <= 0)
        {
            LOG(LogLevel::WARNING) << "send: client quit," << clientaddr.ToString();

        }
    }

private:
    uint16_t _port;
    std::unique_ptr<Socket> _listensock;
    Handler_t _handler;
};
