#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
int main()
{
  pid_t id = fork();
  if(id == 0)
  {
    int cnt = 5;
    while(cnt--)
    {
      printf("我是子进程pid = %d\n",getpid());
      sleep(1);
    }
    exit(1);
  }
  else if(id > 0)
  {
//    while(1)
//    {
//      printf("我是一个父进程，pid = %d \n",getpid());
//      sleep(1);
//    
//    }
   // sleep(10);
    int status = 0;
    pid_t rid = waitpid(id,&status,0);
    if(rid == id)
    {
      printf("wait success!\n");
      printf("exit_sig : %d\n",status&0x7F);
    }
   // sleep(5);
    exit(0);
  }
}















//int main()
//{
//  exit(0);
////  FILE* fp = fopen("./logtxt","r");
////  if(fp == NULL)
////  {
////    printf("%d->%s\n",errno,strerror(errno));
////  }
//
//while(1)
//{
//  printf("I am a process!");
//  sleep(1);
//}
//
//
//
////   for(int i = 0;i<200 ;i++)
////   {
////     printf("%d->%s\n",i,strerror(i));
////   }
//  return 0;
//}
