#include<iostream>
#include<pthread.h>
#include<unistd.h>
void *threadRun(void *args)
{
    while(1)
    {
        printf("pid : %d \n",getpid());
        sleep(1);
    }
}

int main()
{
    pthread_t tid;
    pthread_create(&tid,nullptr,threadRun,nullptr);


    while(true)
    {
        std::cout << "主线程正在运行" <<std::endl;
        printf("pid : %d \n",getpid());
        sleep(1);
    }
    return 0;
}