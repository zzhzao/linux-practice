#include "TcpServer.hpp"
#include "Http.hpp"
#include <memory>

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
    //访问数据库
    
    resp.SetCode(200);
    resp.AddHeader("Content-Type",MiniType::Suffix2MimeType(".txt"));
    resp.SetBody("Login success!");
    resp.AddHeader("Content-Length",std::to_string(resp.BodeSize()));
}
void Register(HttpRequest &req, HttpResponse &resp)
{
    std::cout << "Register start" << std::endl;
}
void Search(HttpRequest &req, HttpResponse &resp)
{
    std::cout << "Search start" << std::endl;
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

    // 2. 定义HTTP协议
    std::unique_ptr<HttpProtocol> protocol = std::make_unique<HttpProtocol>();
    protocol->RegisterService("/Login", Login);
    protocol->RegisterService("/Register", Register);
    protocol->RegisterService("/Search", Search);

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