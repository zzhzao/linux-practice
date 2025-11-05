#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>


int main()
{
//  printf("stdin->fd : %d\n",stdin->_fileno);
 
//  close(1);
//  int fd = open("log.txt",O_WRONLY|O_CREAT|O_APPEND);
//  printf("fd = %d\n",fd);

 // int fd = open("log.txt",O_WRONLY|O_CREAT|O_TRUNC);
  int fd = open("log.txt",O_WRONLY|O_CREAT|O_APPEND);
  printf("fd : %d\n",fd);
  
  dup2(fd,1);

  printf("aaaaaaaaaaaaa\n");
  printf("aaaaaaaaaaaaa\n");
  printf("aaaaaaaaaaaaa\n");
  printf("aaaaaaaaaaaaa\n");



  dup2(fd,0);
  char buffer[128];
  fgets(buffer,sizeof(buffer),stdin);

  printf("buffer : %s\n",buffer);




  return 0;
}
