
#include "Calculator.hpp"
#include "Protocol.hpp"
#include "Reactor.hpp"
#include "Listener.hpp"

#include <memory>
#include <iostream>
#include "Logger.hpp"
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <port>" << std::endl;
        return -1;
    }

    ENABLE_CONSOLE_LOG_STRATEGY();

    for (int i = 0; i < 5; i++)
    {
        if (fork() == 0)
        {

            uint16_t port = std::stoi(argv[1]);
            // 创建网络计算器
            std::unique_ptr<Calculator> cal = std::make_unique<Calculator>();
            // 定义协议对象
            std::unique_ptr<Protocol> protocol = std::make_unique<Protocol>(
                [&cal](Request &req) -> Response
                {
                    return cal->Execute(req);
                });

            std::shared_ptr<Connection> listen = std::make_shared<Listener>([&protocol](std::string &inbuffer) -> std::string
                                                                            { return protocol->ParseRequest(inbuffer); }, port);
            std::unique_ptr<Reactor> R = std::make_unique<Reactor>();

            R->AddConnection(listen);

            R->Loop();
        }
    }
    // .... 
    return 0;
}