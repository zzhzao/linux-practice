
#include"mystdio.h"
#include<string.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<fcntl.h>
#include<stdlib.h>
#include<unistd.h>
#define MODE 0666

myFILE* myfopen(const char *pathname,char* mode)
{
  int fd = -1;
  int flags = 0;
  if(strcmp(mode,"r") == 0)
  {
    flags = O_RDONLY;
    fd = open(pathname,O_RDONLY,MODE);
  }
  else if(strcmp(mode,"w") == 0)
  {
    flags = O_RDONLY|O_CREAT|O_TRUNC;
    fd = open(pathname,O_RDONLY|O_CREAT|O_TRUNC,MODE);
  }
  else if(strcmp(mode,"a") == 0)
  {
    flags = O_RDONLY|O_CREAT|O_APPEND;
    fd = open(pathname,O_RDONLY|O_CREAT|O_APPEND,MODE);
  }
  else 
  {
    
  }
  if(fd < 0)
    return NULL;

  myFILE* fp = (myFILE*)malloc(sizeof(myFILE));
  if(fp == NULL)
    return NULL;
  fp->fd = fd;
  fp->flags = flags;
  fp->flush_mode = LINE_BUFFER;
  fp->cap = SIZE;
  fp->pos = 0;

  return fp;
}
int myfputs(const char* str, myFILE* fp)
{
  if(strlen(str) == 0)
  {
    return 0;
  }
  memcpy(fp->outbuffer + fp->pos, str ,strlen(str));
  fp->pos += strlen(str);
  myfflush(fp);
  return strlen(str);
}

void myfflush(myFILE* fp)
{
  if(fp->pos == 0)
    return;
  if(fp->flush_mode & LINE_BUFFER)
  {
    if(fp->outbuffer[fp->pos - 1] == '\n')
    {
      write(fp->fd,fp->outbuffer,fp->pos);
      fp->pos = 0;
    }
  }
  else if(fp->flush_mode & FULL_BUFFER)
  {

  }
  else if(fp->flush_mode & NON_BUFFER)
  {

  }
}
void myfclose(myFILE* fp)
{
  myfflush(fp);
  close(fp->fd);
  free(fp);
}
