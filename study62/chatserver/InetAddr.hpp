#pragma once

#include<netinet/in.h>
#include<arpa/inet.h>       
#include<iostream>
#include<string>
#include<sys/socket.h>
#include<cstring>
class InetAddr
{
public:
    InetAddr(const struct sockaddr_in &address):_address(address),_len(sizeof(address))
    {
        _ip = inet_ntoa(_address.sin_addr);
        _port = ntohs(_address.sin_port);
    }
    InetAddr(uint16_t port, const std::string &ip = "0.0.0.0")
        :_ip(ip),
        _port(port)
    {
        bzero(&_address, sizeof(_address));
        _address.sin_family = AF_INET;
        _address.sin_port = htons(port);
        _address.sin_addr.s_addr = inet_addr(ip.c_str());
        _len = sizeof(_address);
    }
    InetAddr(){}
    struct sockaddr_in *GetNetAddress()
    {
        return &_address;
    }
    bool operator==(const InetAddr &other) const
    {
        return _ip == other._ip && _port == other._port;
    }
    socklen_t Len()
    {
        return _len;
    }
    ~InetAddr()
    {}
private:

    struct sockaddr_in _address;
    socklen_t _len;
    std::string _ip;
    uint16_t _port;
};
