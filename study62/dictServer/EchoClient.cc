#include<iostream>
#include<string>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<cstdlib>
#include<cstring>

static void Usage(const std::string &process)
{
    std::cerr << "Usage: " << process << " ip port" << std::endl;
}


int main(int argc,char *argv[])
{
    if(argc != 3)
    {
        Usage(argv[0]);
        exit(1);
    }
    uint16_t server_port = std::stoi(argv[2]);
    std::string server_ip = argv[1];
    int sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if(sockfd < 0)
    {
        std::cerr << "create socket error" << std::endl;
        exit(2);
    } 

    sockaddr_in server_addr;    
    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    server_addr.sin_addr.s_addr = inet_addr(server_ip.c_str());


    while(true)
    {
        std::cout <<"please Enter# ";
        std::string msg;
        std::getline(std::cin,msg);
        ssize_t n = sendto(sockfd,msg.c_str(),msg.size(),0,(struct sockaddr*)&server_addr,sizeof(server_addr));
        if(n > 0)
        {
            char inbuf[1024];
            struct sockaddr_in temp;
            socklen_t len = sizeof(temp);
            ssize_t m = recvfrom(sockfd,inbuf,sizeof(inbuf)-1,0,(sockaddr*)&temp,&len);
            if(m > 0)
            {
                inbuf[m] = '\0';
                std::cout << inbuf << std::endl;
            }
        }
    }
    return 0;
}