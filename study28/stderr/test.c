#include<stdio.h>
#include<unistd.h>
#include<string.h>
int main()
{
  printf("这是一个正常消息\n");
  fprintf(stdout,"正常消息\n");
  const char* s1 = "正常消息\n";
    write(1,s1,strlen(s1));
  fprintf(stderr,"错误消息\n");

  return 0;
}
