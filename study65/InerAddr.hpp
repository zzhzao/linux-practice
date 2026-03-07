#pragma once

#include <iostream>
#include <string>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define CONV(address) ((struct sockaddr *)address)

// 对客户端进行先描述
class InetAddr
{
public:
    InetAddr()
    {
    }
    InetAddr(const struct sockaddr_in &address) : _address(address), _len(sizeof(address))
    {
        // _ip = inet_ntoa(_address.sin_addr);
        char ipstr[32];
        inet_ntop(AF_INET, &(_address.sin_addr), ipstr, sizeof(ipstr));
        _ip = ipstr;
        _port = ntohs(_address.sin_port);
    }
    InetAddr(uint16_t port, const std::string &ip = "0.0.0.0") : _ip(ip), _port(port)
    {
        bzero(&_address, sizeof(_address));
        _address.sin_family = AF_INET;
        _address.sin_port = htons(_port); // h->n
        //_address.sin_addr.s_addr = inet_addr(_ip.c_str()); // 1. 字符串ip->4字节IP 2. hton
        inet_pton(AF_INET, ip.c_str(), &(_address.sin_addr));
        _len = sizeof(_address);
    }
    bool operator==(const InetAddr &addr)
    {
        return (this->_ip == addr._ip) && (this->_port == addr._port);
    }
    void operator=(struct sockaddr_in &addr)
    {
        _address = addr;
        char ipstr[32];
        inet_ntop(AF_INET, &(_address.sin_addr), ipstr, sizeof(ipstr));
        _ip = ipstr;
        _port = ntohs(_address.sin_port);
    }
    std::string ToString()
    {
        return "[" + _ip + ":" + std::to_string(_port) + "]";
    }

    struct sockaddr *NetAddress()
    {
        return CONV(&_address);
    }
    socklen_t Len()
    {
        return _len;
    }
    ~InetAddr()
    {
    }

private:
    // net address
    struct sockaddr_in _address;
    socklen_t _len;
    // host address
    std::string _ip;
    uint16_t _port;
};