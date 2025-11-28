#include <iostream>
#include <unistd.h>
#include <signal.h>

void handler(int signo)
{
    std::cout << "我是一个信号 ..." << std::endl;
    // exit(10);
}

int main()
{
    //signal(SIGINT, handler);
    for (int i = 1; i < 32; i++)
    {
        signal(i, handler);
    }
   // while (true)
   // {
        std::cout << "test ... pid : " << getpid() << std::endl;
        sleep(1);
	int i = 10;
	i /= 0;
	sleep(1);
	exit(0);
   // }
   return 0;
}
