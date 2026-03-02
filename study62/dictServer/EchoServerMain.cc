#include"EchoServer.hpp"
#include"Dict.hpp"
#include<memory>


static void Usage(const std::string &process)
{
    std::cerr << "Usage: " << process << "  port" << std::endl;
}


int main(int argc,char *argv[])
{
    if(argc != 2)
    {
        Usage(argv[0]);
        exit(USAGE_ERR);
    }

    ENABLE_CONSOLE_LOG_STRATEGY();
    Dict dict;

    std::string server_ip = argv[1];
    uint16_t server_port = std::stoi(argv[2]);
    std::unique_ptr<UdpServer> usvr = std::make_unique<UdpServer>(server_port, [&dict](std::string word)->std::string{
        return dict.Translate(word);
    });
    usvr->Init();
    usvr->Start();
    return 0;
}