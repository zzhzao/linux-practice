#pragma once
#include<iostream>
#include<unistd.h>
#include<signal.h>
#include<stdlib.h>
#include<fcntl.h>

// 1 是修改 0 是不修改
void Daemon(int isclose, int isredir)
{
    signal(SIGPIPE,SIG_IGN);
    signal(SIGCHLD,SIG_IGN);

    // 不能是组长
    if(fork() > 0)
    {
        exit(0);
    }
    //  设置新会话
    pid_t id = setsid();
    (void)id;

    //更改守护进程的工作目录
    if(isredir)
    chdir("/");
    //重定向 0 1 2
    if(isclose)
   {
        close(0);
        close(1); 
        close(2); 

   }
    else{
    int fd = open("/dev/null",O_RDWR);
    if(fd >= 0)
    {
        dup2(fd,0);
        dup2(fd,1);
        dup2(fd,2);
        close(fd);
    }
}
}