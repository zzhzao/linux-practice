#pragma once
#include <iostream>
#include "UserManage.hpp"
#include <string>
#include <memory>
#include "Mutex.hpp"
#include <sys/socket.h>
class Route
{
public:
    Route() : _uma(std::make_unique<UserManager>())
    {
    }
    void Broadcast(int sockfd, std::string &message)
    {
        LockGuard LockGuard(_lock);
        for (auto &user : _uma->GetUsers())
        {
            sendto(sockfd, message.c_str(), message.size(), 0, (struct sockaddr *)user.GetNetAddress(), user.Len());
        }
    }
    void CheckUser(const InetAddr &addr)
    {
        LockGuard LockGuard(_lock);
        _uma->AddUser(addr);
    }
    void OfflineUser(const InetAddr &addr)
    {
        LockGuard LockGuard(_lock);
        _uma->DelUser(addr);
    }
    ~Route() {}

private:
    std::unique_ptr<UserManager> _uma;
    Mutex _lock;
};