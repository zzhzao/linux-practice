#include"EchoServer.hpp"
#include<memory>


static void Usage(const std::string &process)
{
    std::cerr << "Usage: " << process << " ip port" << std::endl;
}


int main(int argc,char *argv[])
{
    if(argc != 3)
    {
        Usage(argv[0]);
        exit(USAGE_ERR);
    }

    ENABLE_CONSOLE_LOG_STRATEGY();

    std::string server_ip = argv[1];
    uint16_t server_port = std::stoi(argv[2]);
    std::unique_ptr<UdpServer> usvr = std::make_unique<UdpServer>();
    usvr->Init();
    usvr->Start();
    return 0;
}