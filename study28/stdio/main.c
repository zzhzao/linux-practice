#include"mystdio.h"

int main()
{
  myFILE* fp = myfopen("log.txt","w");
  if(fp == NULL)
  {
    printf("myfopen error\n");
    return 0;
  }
  const char *msg = "hello bit\n";
  myfputs(msg,fp);
  myfclose(fp);
  return 0;
}
