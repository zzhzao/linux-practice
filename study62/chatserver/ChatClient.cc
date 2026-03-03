#include <iostream>
#include <string>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib>
#include <cstring>
#include "Thread.hpp"
#include "InetAddr.hpp"

using namespace NS_THREAD_MODULE;

static void Usage(const std::string &process)
{
    std::cerr << "Usage: " << process << " ip port" << std::endl;
}
int sockfd = 0;
uint16_t server_port = 0;
std::string server_ip;

void RecvMessage()
{
    while (true)
    {
        // recvfrom
        char inbuffer[1024] = {0};
        struct sockaddr_in temp;
        socklen_t len = sizeof(temp);
        ssize_t m = recvfrom(sockfd, inbuffer, sizeof(inbuffer) - 1, 0, (struct sockaddr *)&temp, &len);
        if (m > 0)
        {
            inbuffer[m] = 0;
            std::cerr << inbuffer << std::endl; // 2
        }
    }
}
void SendMessage()
{
    InetAddr serveraddr(server_port, server_ip);
    while (true)
    {
        std::cout << "please Enter# ";
        std::string msg;
        std::getline(std::cin, msg);
        ssize_t n = sendto(sockfd, msg.c_str(), msg.size(), 0, (struct sockaddr *)serveraddr.GetNetAddress(), serveraddr.Len());
        (void)n;
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        Usage(argv[0]);
        exit(1);
    }
    server_port = std::stoi(argv[2]);
    server_ip = argv[1];
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "create socket error" << std::endl;
        exit(2);
    }

    Thread recver(RecvMessage);
    Thread sender(SendMessage);

    recver.Start();
    sender.Start();

    recver.Join();
    sender.Join();

    return 0;
}