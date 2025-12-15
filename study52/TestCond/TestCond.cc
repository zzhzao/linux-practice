#include <iostream>
#include <string>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t gmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t gcond = PTHREAD_COND_INITIALIZER;

void *Print(void *args)
{
    std::string name = static_cast<const char *>(args);
    while (true)
    {
        pthread_mutex_lock(&gmutex);
        std::cout << "我是新线程：" << name << std::endl;
        //sleep(1);
        pthread_cond_wait(&gcond,&gmutex);
        pthread_mutex_unlock(&gmutex);
    }
    return nullptr;
}
int main()
{
    pthread_t tids[4];
    for (int i = 0; i < 4; i++)
    {
        char *name = new char[64];
        snprintf(name, 64, "thread-%d", i + 1);
        pthread_create(tids + i, nullptr, Print, (void *)name);
    }
    while(true)
    {
        // pthread_cond_signal(&gcond);
        pthread_cond_broadcast(&gcond);
        sleep(1);
    }
    for (int i = 0; i < 4; i++)
    {
        pthread_join(tids[i], nullptr);
    }

    return 0;
}