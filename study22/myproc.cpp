#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<stdlib.h>
#include <iostream>
#include <vector>
//启动多进程
//
enum 
{
  OK,
  USAGE_ERR
};
void Task()
{

}
int main(int argc,char* argv[])
{
  if(argc != 2)
  {
    std::cout << "Usage" << argv[0] << "process_num" << std::endl;
    exit(USAGE_ERR);
  }
  int num = std::stoi(argv[1]);
  pid_t id = fork();
  if(id == 0)
  {
    Task();
    exit(0);
  }
  int status = 0;
  pid_t rid = waitpid(id,&status,0);
  if(rid > 0)
  {
    printf("子进程：%d Exit,exit code : %d\n",rid,WEXITSTATUS(status));
  }
}





//
//int main()
//{
//  printf("我是一个进程：pid : %d,ppid : %d\n",getpid(),getppid());
//
//  pid_t id= fork();
//  if(id < 0)
//  {
//    perror("fork");
//    exit(1);
//  }
//  else if(id == 0)
//  {
//    int cnt = 5;
//    while(cnt--)
//    {
//
//       printf("我是一个子进程：pid : %d,ppid : %d\n",getpid(),getppid());
//       sleep(1);
//    }
//  }
//  else{
//
//    while(1)
//    {
//    int status = 0; 
//    pid_t rid = waitpid(-1,&status,WNOHANG);
//    if(rid > 0)
//    {
//      printf("wait success,退出的子进程是：%d,exit_code: %d,exit_sig : %d\n",rid,(status >> 8)&0xFF,status&0x7F);
//      break;
//    }
//    else if(rid == 0)
//    {
//      printf("子进程正在运行，父进程还需要等待\n");
//      sleep(1);
//    }
//    else{
//      perror("waitpid");
//      break;
//    }
//
//  }
//}
