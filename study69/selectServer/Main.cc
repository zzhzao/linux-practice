#include "PollServer.hpp"
#include "Logger.hpp"
#include<memory>
#include<iostream>


int main(int argc,char *argv[])
{
    if(argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <port>" << std::endl;
        return -1;
    }
    ENABLE_CONSOLE_LOG_STRATEGY();
    uint16_t port = std::stoi(argv[1]);

    std::unique_ptr<PollServer> svr = std::make_unique<PollServer>(port);
    svr->Loop();
    return 0;
}
