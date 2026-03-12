#include "TcpServer.hpp"
#include "Http.hpp"
#include <memory>


static void Usage(const std::string &proc)
{
    std::cout << "Usage:\n\t" << proc << " port" << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(1);
    }
    ENABLE_CONSOLE_LOG_STRATEGY();
    uint16_t port = std::stoi(argv[1]);

    // 2. 定义HTTP协议
    std::unique_ptr<HttpProtocol> protocol = std::make_unique<HttpProtocol>();

    // 3. 定义网络对象
    std::unique_ptr<TcpServer> tsvr = std::make_unique<TcpServer>(
        [&protocol](std::string &inbuffer)->std::string{
            return protocol->HandlerHttpRequest(inbuffer);
        }, port
    );
    
    // 4. 启动
    tsvr->Loop();

    return 0;
}