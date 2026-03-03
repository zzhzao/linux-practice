#include "UdpServer.hpp"
#include "ThreadPool.hpp"
#include "Route.hpp"
#include <memory>

static void Usage(const std::string &process)
{
    std::cerr << "Usage: " << process << "  port" << std::endl;
}

using namespace NS_THREAD_POOL_MODULE;

using task_t = std::function<void()>;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(USAGE_ERR);
    }

    ENABLE_CONSOLE_LOG_STRATEGY();

    std::string server_ip = argv[1];
    uint16_t server_port = std::stoi(argv[2]);

    auto thread_pool = ThreadPool<task_t>::Instance();

    Route route;
    UdpServer server(server_port);
    server.Init();
    server.RegisterService([&route](const InetAddr &addr)
                           { route.CheckUser(addr); }, [&route, thread_pool](int sockfd, std::string msg)
                           {
        task_t t = std::bind(&Route::Broadcast,&route,sockfd,std::ref(msg));
        thread_pool->Enqueue(t); });

    server.Start();
    return 0;
}