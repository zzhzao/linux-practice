#include "TcpServer.hpp"
#include <memory>
#include"Command.hpp"
static void Usage(const std::string &process)
{
    std::cerr << "Usage:\n\t";
    std::cerr << process << " local_port" << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(USAGE_ERR);
    }
    ENABLE_CONSOLE_LOG_STRATEGY();
    uint16_t server_port = std::stoi(argv[1]);

    std::unique_ptr<Command> command = std::make_unique<Command>();

    std::unique_ptr<TcpServer> tsvr = std::make_unique<TcpServer>(server_port);
    tsvr->InitServer();
    tsvr->Register([&command](std::string _cmd) -> std::string {return command->Excute(_cmd);});
    tsvr->Start();

    return 0;
}