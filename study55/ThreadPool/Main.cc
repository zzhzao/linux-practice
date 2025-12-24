#include"Logger.hpp"
#include "ThreadPool.hpp"
using namespace NS_THREAD_POOL;
using namespace NS_LOG_MODULE;
int main()
{
    ENABLE_CONSOLE_LOG_STRATEGY();
    ThreadPool tp;
    tp->start();
    while (true)
    {
    }
    tp->Stop();
    return 0;
}