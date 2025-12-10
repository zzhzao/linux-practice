#include "Thread.hpp"
#include<unistd.h>
#include<vector>
void Loop()
{
    char name[64];
    pthread_getname_np(pthread_self(), name, sizeof(name));
    while (true)
    {
        std::cout << "new thread running" << name << std::endl;
    }
}
int main()
{
    ThreadModule::Thread t(Loop);
    t.Start();

    t.Stop();

    t.Join();

    return 0;
}
