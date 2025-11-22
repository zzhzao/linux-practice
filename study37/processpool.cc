#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <cstdio>
#include <functional>
#include <cstdlib>
#include <ctime>
////////////////////////子进程任务////////////////////////////
void SyncDisk()
{
    std::cout << getpid() << "刷新数据到磁盘" << std::endl;
    sleep(1);
}
void DownLoad()
{
    std::cout << getpid() <<"下载任务到系统" << std::endl;
    sleep(1);
}

void PrintLog()
{
    std::cout << getpid() << ": 打印日志到本地" << std::endl;
    sleep(1);
}

void UpdateStatus()
{
    std::cout << getpid() << ": 更新一次用户的状态" << std::endl;
    sleep(1);
}

typedef void (*task_t)(); // 函数指针
task_t tasks[4] = {SyncDisk, DownLoad, PrintLog, UpdateStatus};

///////////////////////进程池/////////////////////////////////

enum
{
    OK = 0,
    PIPE_ERROR,
    FORK_ERROR
};

void DoTask(int fd)
{
    while (true)
    {
        int task_code = 0;
        ssize_t n = read(fd, &task_code, sizeof(task_code));
        if(n == sizeof(task_code))
        {
            if(task_code >= 0 && task_code <4)
            {
                tasks[task_code]();
            }
        }
        else if(n == 0)
        {
            // 读到0时,则说明父进程要结束了
            std::cout << "task quit..." << std::endl;
            break;
        }
        else
        {
            perror("read");
            break;
        }
    }
}
const int gprocessnum = 5;
// 父进程管理 “通道”

// typedef std::function<void (int)> task_t;
using cb_t = std::function<void(int)>;
class ProcessPool
{
public:
    class Channel
    {
    public:
        Channel(int wfd, pid_t pid) : _wfd(wfd), _sub_pid(pid)
        {
            _sub_name = "sub-channel " + std::to_string(_sub_pid);
        }
        void PrintfInfo()
        {
            printf("wfd: %d, who : %d ,channel name :%s \n", _wfd, _sub_pid, _sub_name.c_str());
        }
        void Write(int index)
        {
            ssize_t n = write(_wfd, &index, sizeof(index)); // 约定的4字节
            (void)n;
        }
        std::string Name()
        {
            return _sub_name;
        }
        ~Channel()
        {
        }

    private:
        int _wfd;
        pid_t _sub_pid;
        std::string _sub_name;
    };

public:
    ProcessPool()
    {
        srand((unsigned int)time(nullptr) ^ getpid());
    }
    ~ProcessPool() {}
    void Init(cb_t cb)
    {
        CreateProcessChannel(cb);
    }
    void Debug()
    {
        for (auto &c : channels)
        {
            c.PrintfInfo();
        }
    }
    void Run()
    {
        while (true)
        {
            // 1. 选择一个channel
            int index = selectchannel();
            // std::cout << "index: " << index << std::endl;
            // sleep(1);

            // 2. 选择一个任务
            int task = selecttask();

            // 3. 发送一个任务给指定的channel
            SendTaskToSalver(task, index);
            sleep(1);
        }
    }

private:
    void SendTaskToSalver(int task, int index)
    {
        if (task >= 4 && task < 0)
        {
            return;
        }
        if (index < 0 || index >= channels.size())
        {
            return;
        }
        channels[index].Write(task);
    }
    int selectchannel()
    {
        static int index = 0;
        int selected = index;
        index++;
        index %= channels.size();
        return selected;
    }
    int selecttask()
    {
        int itask = rand() % 4;
        return itask;
    }
    void CreateProcessChannel(cb_t cb)
    {
        for (int i = 0; i < gprocessnum; i++)
        {
            int pipefd[2] = {0};
            int n = pipe(pipefd);
            if (n < 0)
            {
                std::cerr << "pipe create error" << std::endl;
                exit(PIPE_ERROR);
            }

            pid_t id = fork();
            if (id < 0)
            {
                std::cerr << "fork error" << std::endl;
                exit(FORK_ERROR);
            }
            else if (id == 0)
            {
                close(pipefd[1]);
                cb(pipefd[0]); // 回调函数
                exit(OK);
            }
            else
            {
            }
            close(pipefd[0]);
            channels.emplace_back(pipefd[1], id);
            // Channel ch(pipefd[1],id);
            // channels.push_back(ch);
            std::cout << "创建子进程：" << id << " success" << std::endl;
            sleep(1);
        }
    }

private:
    // 0.未来组织所有channel的容器
    std::vector<Channel> channels;
};

int main()
{
    ProcessPool pp;
    // 1. 创建多管道
    // pp.CreateProcessChannel();
    // 1. 初始化进程池
    pp.Init(DoTask);
    // 2. 父进程控制子进程
    pp.Debug();
    pp.Run();
    sleep(1000);

    // 3. 回收资源

    return 0;
}