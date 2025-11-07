#pragma once 

#include<stdio.h>

#define SIZE 1024
#define NON_BUFFER   1
#define LINE_BUFFER   2
#define FULL_BUFFER   4



typedef struct _myFILE
{
  int fd;
  int flags;
  int flush_mode;
  char outbuffer[SIZE];
  int pos;
  int cap;
}myFILE;


myFILE* myfopen(const char *pathname,char* mode);
int myfputs(const char* str, myFILE* fp);
void myfflush(myFILE* fp);
void myfclose(myFILE* fp);
