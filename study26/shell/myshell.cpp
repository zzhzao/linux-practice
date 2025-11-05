#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<string>
#include<iostream>
#include<ctype.h>
#include<sys/stat.h>
#include<fcntl.h>
# define MAX_SIZE 128
#define MAXARGS 32

char* gargv[MAXARGS];
int gargc = 0;
const char* gsep = " ";

char cwd[MAX_SIZE];
int lastcode;

char* genv[MAXARGS];
int genvc = 0;


#define NoneRedir 0
#define InputRedir 1
#define AppRedir 2
#define OutputRedir 3

//重定向的方式
int redir_type = NoneRedir;
char* filename = NULL;//保存重定向的目标文件


#define TrimSpace(start) do{
  \while(isspace(*start)) 
  \start++;
  \}while(0)


static std::string rFindDir(const std::string &p)
{

  if(p == "/")
  {
    return p;
  }
  const std::string psep = "/";
  auto pos = p.rfind("psep");
  if(pos == std::string::npos)
    return std::string();
  return p.substr(pos+1);
}


char* GetUserName()
{
  char* name = getenv("USER");
  if(name == NULL)
  {
    return (char*)"None";
  }
  return name;
}
char* GetHostName()
{
  char* hostname = getenv("HOSTNAME");
  if(hostname == NULL)
  {
    return (char*)"None";
  }
  return hostname;
}

const char* GetPwd()
{
  //char* pwd = getenv("PWD");
  char* pwd = getcwd(cwd,sizeof(cwd));
  if(pwd == NULL)


  {
    return "None";
  }
  return cwd;
}


void PrintCommandLine()
{
  printf("[%s@%s %s]# ",GetUserName(),GetHostName(),rFindDir(GetPwd()).c_str());//用户名@主机名 当前路径
  fflush(stdout);
}


int GetCommand(char command_line[],int size)
{

    if(NULL == fgets(command_line,size,stdin))
      return 0;
    command_line[strlen(command_line) -1] = '\0';
    return strlen(command_line);
}

int  ParseCommand(char commandline[])
{
  gargc = 0;
  memset(gargv,0,sizeof(gargv));
  gargv[0] = strtok(commandline,gsep);
  while((gargv[++gargc] = strtok(NULL,gsep)));
  return gargc;
}


int ExecuteCommand()
{
  pid_t id = fork();
  if(id < 0)
  {
    return -1;
  }
  else if(id == 0)
  {
    int fd = -1;
    if(redir_type == NoneRedir)
    {

    }
    else if(redir_type == OutputRedir)
    {
      fd = open(filename,O_WRONLY|O_CREAT|O_TRUNC,0666);
      dup2(fd,1);
    }
    else if(redir_type == AppRedir)
    {
      fd = open(filename,O_WRONLY|O_CREAT|O_APPEND,0666);
      dup2(fd,1);
    }
    else if(redir_type == InputRedir)
    {
      fd = open(filename,O_RDONLY);
      dup2(fd,0);
    }
    else 
    {

    }
    execvp(gargv[0],gargv);
    exit(0);
  }
  else
  {
   int status = 0;
   pid_t rid =  waitpid(id,&status,0);
   if(rid > 0)
   {
     lastcode = WEXITSTATUS(status);
   //  printf("wait child process success!\n");
   }
  }
  return 0;
}

int CheckBuiltinExecute()
{
  if(strcmp(gargv[0],"cd") == 0)
  {
    if(gargc == 2)
    {
      chdir(gargv[1]);
      char pwd[1024];
      getcwd(pwd,sizeof(pwd));
      snprintf(cwd,sizeof(cwd),"PWD=%s",pwd);
      putenv(cwd);
      lastcode = 0;
    }
    return 1;
  }
  else if(strcmp(gargv[0],"echo") == 0)
  {
    if(gargc == 2)
    {
      if(gargv[1][0] == '$')
      {
        if(strcmp(gargv[1]+1,"?")==0)
        {
          printf("lastcode: %d\n",lastcode);
        }
      }
      else if(strcmp(gargv[1]+1,"PATH") == 0)
      {
         printf("%s\n",getenv("PATH"));
      }
    }
    lastcode = 0;
    return 1;
  }
  return 0;
}

void LoadEnv()
{
  extern char** environ;
  for(;environ[genvc];genvc++)
  {
    genv[genvc] = new char[4096];
    strcpy(genv[genvc],environ[genvc]);
  }
  genv[genvc] = NULL;
}

void ParseRedir(char commandline[])
{
  redir_type = NoneRedir;
  filename = NULL;

  char* start = commandline;
  char* end = commandline + strlen(commandline) -1;
  while(start < end)
  {
    if(*start == '>')
    {
      if(*(start + 1) == '>')
      {
        *start == '\0';
        start++;
        *start = '\0';
        start++;
        //去除文件名前面的空格
        TrimSpace(start);
        redir_type = AppRedir;
        filename = start;

        break;
      }
        *start = '\0';
        start++;
        //去除文件名前面的空格
        TrimSpace(start);
        redir_type = OutputRedir;
        filename = start;

        break;
    }
    else if(*start == '<')
    {
      *start = '\0';
      start++;
      TrimSpace(start);
      redir_type = InputRedir;
      filename = start;
      break;
    }
    else{
      start++;
    }
  }
}



int main()
{


  char command_line[MAX_SIZE] = {0}; 

  while(1)
  {
    PrintCommandLine();

    if(0 == GetCommand(command_line,sizeof(command_line)))
    {
      continue;
    }
    ParseRedir(command_line);
    ParseCommand(command_line);

  //  printf("%s\n",command_line);
   // sleep(1);
   //
   //

   //执行内建命令
   //
   
   if(CheckBuiltinExecute())
   {
     continue;
   }


   
   ExecuteCommand();
  }


  return 0;
}
