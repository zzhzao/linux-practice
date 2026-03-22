#include "TcpServer.hpp"
#include "Http.hpp"
#include <memory>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include"Daemon.hpp"
static void Usage(const std::string &proc)
{
    std::cout << "Usage:\n\t" << proc << " port" << std::endl;
}

void Login(HttpRequest &req, HttpResponse &resp)
{
    std::cout << "Login start" << std::endl;
    req.DebugPrint();
    std::string data = req.Text();
    std::cout << "data is: " << data << std::endl;

    // 拿到正文 name passwd
    // 访问数据库

    resp.SetCode(200);
    resp.AddHeader("Content-Type", MiniType::Suffix2MimeType(".txt"));
    resp.SetBody("Login success!");
    resp.AddHeader("Content-Length", std::to_string(resp.BodySize()));
}
void Register(HttpRequest &req, HttpResponse &resp)
{
    std::cout << "Register start" << std::endl;
}
void Search(HttpRequest &req, HttpResponse &resp)
{
    std::cout << "Search start" << std::endl;
}

void Exec(HttpRequest &req, HttpResponse &resp)
{

    std::cout << "Exec start" << std::endl;

    req.DebugPrint();
    std::cout << "###################################################################" << std::endl;

    std::string text = req.Text();
    std::cout << "text is: " << text << std::endl;
    text = text.substr(8);
    std::cout << "text is: " << text << std::endl;
    std::cout << "###################################################################" << std::endl;

    // 修复点1：处理无空格的情况
    auto iter = text.find(" ");
    std::string s1, s2;
    if (iter == std::string::npos)
    {
        s1 = text;
        s2 = ""; // 无参数
    }
    else
    {
        s1 = text.substr(0, iter);
        s2 = text.substr(iter + 1); // 修复点：去掉空格本身
    }

    // 使用 stringstream 分割多个参数
    std::vector<char *> args;
    args.push_back(const_cast<char *>(s1.c_str()));
    std::stringstream ss(s2);
    std::string arg;
    while (ss >> arg)
    {
        args.push_back(const_cast<char *>(arg.c_str()));
    }
    args.push_back(NULL);

    char buffer[10240] = {0};
    int fd[2];

    // 修复点3：检查 pipe 创建是否成功
    if (pipe(fd) == -1)
    {
        std::cerr << "Pipe creation failed" << std::endl;
        resp.SetCode(500);
        resp.SetBody("Internal error");
    }

    pid_t id = fork();
    int status;

    if (id == -1)
    { // 修复点4：检查 fork 是否成功
        std::cerr << "Fork failed" << std::endl;
        close(fd[0]);
        close(fd[1]);
        resp.SetCode(500);
        resp.SetBody("Internal error");
    }

    if (id == 0)
    {                 // 子进程
        close(fd[0]); // 关闭读端

        // 修复点5：检查 dup2 是否成功
        if (dup2(fd[1], 1) == -1)
        {
            std::cerr << "Dup2 failed" << std::endl;
            exit(1);
        }
        close(fd[1]); // 复制后关闭原描述符

        // 执行外部命令
        execvp(s1.c_str(), args.data());

        // 修复点6：execlp 执行失败后必须退出子进程
        std::cerr << "Exec failed: " << s1 << std::endl;
        exit(EXIT_FAILURE); // 确保子进程退出
    }

    // 父进程
    close(fd[1]); // 关闭写端

    // 修复点7：正确处理 read 返回值
    ssize_t n = read(fd[0], buffer, sizeof(buffer) - 1);
    if (n == -1)
    {
        std::cerr << "Read failed" << std::endl;
        buffer[0] = '\0';
    }
    else if (n > 0)
    {
        buffer[n] = '\0';
    }
    else
    { // n == 0，表示管道写端已关闭（子进程退出）
        buffer[0] = '\0';
    }
    close(fd[0]); // 修复点8：关闭读端

    std::cout << "###################################################################" << std::endl;
    std::cout << buffer << std::endl;
    std::cout << "###################################################################" << std::endl;
    // 修复点9：等待子进程退出并检查执行状态
    waitpid(id, &status, 0);
    if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
    {
        std::cerr << "Child process exited with error: " << WEXITSTATUS(status) << std::endl;
        resp.SetCode(400);
        resp.SetBody("Command execution failed");
    }

    // 构建响应
    resp.SetCode(200);
    resp.AddHeader("Content-Type", MiniType::Suffix2MimeType(".txt"));
    resp.SetBody(buffer);
    resp.AddHeader("Content-Length", std::to_string(resp.BodySize()));
}
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(1);
    }
    ENABLE_CONSOLE_LOG_STRATEGY();
    uint16_t port = std::stoi(argv[1]);
    //Daemon(1,1);
    daemon(0,0);
    // 2. 定义HTTP协议
    std::unique_ptr<HttpProtocol> protocol = std::make_unique<HttpProtocol>();
    protocol->RegisterService("/Login", Login);
    protocol->RegisterService("/Register", Register);
    protocol->RegisterService("/Search", Search);
    protocol->RegisterService("/exec", Exec);

    // 3. 定义网络对象
    std::unique_ptr<TcpServer> tsvr = std::make_unique<TcpServer>(
        [&protocol](std::string &inbuffer) -> std::string
        {
            return protocol->HandlerHttpRequest(inbuffer);
        },
        port);

    // 4. 启动
    tsvr->Loop();

    return 0;
}