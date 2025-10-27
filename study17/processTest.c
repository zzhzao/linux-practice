#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
int main()
{
  pid_t id = fork();
  if(id == 0)
  {
  while(1)
  {
    printf("子进程：%d,ppid : %d \n",getpid(),getppid());
    sleep(1);
  }
  }
  else 
  {
    int cnt = 5;
    while(cnt)
    {
      cnt--;
      printf("父进程：%d, cnt : %d\n",getpid(),cnt);
      sleep(1);
    }
  }
  return 0;
}
