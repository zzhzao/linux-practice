#include<iostream>
#include"Protocol.hpp"
#include"Socket.hpp"
#include<memory>
using namespace NS_SOCKET_MODULE;

void Usage(const std::string& proc)
{
    std::cout << "Usage: " << proc << " ip port" << std::endl;
}

static void HanlderResponse(Response &resp)
{
    std::cout << "result: " << resp._result << "[" << resp._code << "]" << std::endl;
}
int main(int argc, char *argv[])
{
    if(argc != 3)
    {
        Usage(argv[0]);
        exit(1);
    }
    std::string server_ip = argv[1];
    uint16_t server_port = atoi(argv[2]);
    std::unique_ptr<Socket> socket = std::make_unique<TcpSocket>();
    socket->BuildTcpClientSockMethod();

    InetAddr serveraddress(server_port, server_ip);
    bool n = socket->Connect(serveraddress);
    Protocol procotol;
    if (!n)
    {
        std::cerr << "connect error: " << serveraddress.ToString() << std::endl;
        exit(2);
    }
    std::string inbuffer(HanlderResponse);
    while(true)
    {
        int x,y;
        char oper;
        std::cout << "请输入x: " ;
        std::cin >> x;
        std::cout << "请输入y: " ;
        std::cin >> y;
        std::cout << "请输入操作符: ";
        std::cin >> oper;

        Request req(x,y,oper);
        std::string req_json;
        req.Serialize(&req_json);

        std::string send_req_string = procotol.Packet(req_json);
        socket->Send(send_req_string);

        socket->Recv(&inbuffer);
        procotol.ParseResponse(inbuffer);
    }
    socket->Close();
    return 0;
}