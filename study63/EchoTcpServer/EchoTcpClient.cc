#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <string>
#include <unistd.h>
#include "InetAddr.hpp"

static void Usage(const std::string &process)
{
    std::cerr << "Usage:\n\t";
    std::cerr << process << " server_ip server_port" << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        Usage(argv[0]);
        exit(1);
    }
    std::string server_ip = argv[1];
    uint16_t server_port = std::stoi(argv[2]);

    int sockfd = -1;

        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
        {
            std::cerr << "socket error" << std::endl;
            exit(2);
        }
        InetAddr serveraddress(server_port, server_ip);

        int n = connect(sockfd, (struct sockaddr *)serveraddress.GetNetAddress(), serveraddress.Len());
        if (n < 0)
        {
            std::cerr << "connect error" << std::endl;
            exit(3);
        }
        std::cerr << "connect server success" << std::endl;
    
    while (true)
    {
        std::string line;
        std::cout << "Please Enter# ";
        std::getline(std::cin, line);

        write(sockfd, line.c_str(), line.size());

        char buffer[1024];
        ssize_t n = read(sockfd, buffer, sizeof(buffer) - 1);
        if (n > 0)
        {
            buffer[n] = 0;
            std::cout << "service echo# " << buffer << std::endl;
        }
        else if (n == 0)
        {
            std::cerr << "server close" << std::endl;
            break;
        }
        else
        {
            std::cerr << "read error" << std::endl;
            break;
        }
    }
}