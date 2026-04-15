#include<iostream>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
void SetNonBlock(int fd)
{
    int fl = fcntl(fd,F_GETFL);
    if(fl < 0)
    {
        std::cout << "error " << std::endl;
        return;
    }
    fcntl(fd,F_SETFL,fl | O_NONBLOCK);
}

int main()
{
    char inbuffer[1024];

    SetNonBlock(0);
    while(true)
    {
        ssize_t n = read(0,inbuffer,sizeof(inbuffer)-1);
        if(n > 0)
        {
            inbuffer[n - 1] = 0; //处理回车
            std::cout << "echo: " << inbuffer << std::endl;
        }
        else
        {
            // read error  || 如果一个文件fd，读取的时候，如果没有就绪，也是以出错形式返回的！！但是它不是错误！！
            // 必须区分这些情况了！
            if (errno == EWOULDBLOCK || errno == EAGAIN)
            {
                // 进程就可以做自己的事情了！
                std::cout << "data is not ready!" << std::endl;
                sleep(1);
                continue;
            }
            else if(errno == EINTR)
            {
                sleep(1);
                continue;
            }
            else
            {
                std::cerr << "read error ... :  " << errno << std::endl;
                std::cerr << "read error ... :  " << strerror(errno) << std::endl;
            }
        }

        sleep(1);
    }
}