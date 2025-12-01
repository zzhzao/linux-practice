#include <iostream>
#include <signal.h>
#include <unistd.h>
#include<vector>

int current = 0;
class task_struct
{
private:
    int pid;
    int status;
    int counter;
public:
    task_struct(int p):pid(p),counter(5)
    {}
    void desc()
    {
        counter--;
    }
    void run()
    {
        std::cout<<"process " << pid << "running" << std::endl;
    }
    void resetcounter()
    {
        counter = 5;
    }
    bool Expired()
    {
        return counter <=0;
    }
    ~task_struct()
    {}

};
std::vector<task_struct> task;

// void Reset(int signo)
// {
//     alarm(1);
// }

void do_timer(int signo)
{
    task[current].desc();
    if(task[current].Expired())
    {
        //调度进程
        current = rand()%task.size();
        task[current].resetcounter();
    }
    else
    {
        task[current].run();
    }
    alarm(1);
}
int main()
{
    alarm(1);
    signal(SIGALRM, do_timer);
    srand(time(nullptr));

    task.emplace_back(1);
    task.emplace_back(2);
    task.emplace_back(3);
    task.emplace_back(4);
    task.emplace_back(5);

    for (;;)
    {
        printf("中断被唤醒\n");
        pause();
    }
}
