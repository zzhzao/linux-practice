#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<string.h>
int main()
{
    //创建管道
    int pipefd[2] = {0};
    int n = pipe(pipefd);
    if(n < 0)
    {
        perror("pipe");
        return 1;
    }
    printf("pipr[0]:%d, pipe[1] : %d \n",pipefd[0],pipefd[1]);
    //创建子进程
    pid_t id = fork();
    if(id == 0)
    {
        close(pipefd[0]);
        char *msg = "hello";
        // write(pipefd[1],msg,strlen(msg));
        // exit(0);
        int cnt = 10;
        char outbuffer[1024];
        while(cnt)
        {
            snprintf(outbuffer,sizeof(outbuffer),"f->c# %s %d,pid: %d\n",msg,cnt--,getpid());
            write(pipefd[1],outbuffer,strlen(outbuffer));
        }
        exit(0);
    }


    close(pipefd[1]);
    //char buffer[1024];
    char inbuffer[1024];
    while(1)
    {
        ssize_t n = read(pipefd[0],inbuffer,sizeof(inbuffer) - 1);
        if(n > 0)
        {
            inbuffer[n] = 0;
            printf("%s",inbuffer);
        }
        else{
            perror("read");
            break;
        }
    }


    pid_t rid = waitpid(id,NULL,0);
    (void)rid;//防止编译器报警。
    return 0;
}
