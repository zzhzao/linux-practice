#pragma once
#include<iostream>
#include"Logger.hpp"
#include"InetAddr.hpp"
#include<vector>
using namespace NS_LOG_MODULE;
// class User
// {
// public:
// private:
// };

class UserManager
{
public:
    UserManager(){}
    ~UserManager(){}
    void AddUser(const InetAddr &addr)
    {
        if(SearchUser(addr))
        {
            return;
        }
        _users.push_back(addr);
    }
    bool SearchUser(const InetAddr &addr)
    {
        for(auto &user : _users)
        {
            if(user == addr)
            {
                return true;
            }
        }
        return false;
    }
    void DelUser(const InetAddr &addr)
    {
        for(auto it = _users.begin(); it != _users.end(); ++it)
        {
            if(*it == addr)
            {
                _users.erase(it);
                break;
            }
        }
    }
    // bool ModUser(const InetAddr& addr)
    // {}
    std::vector<InetAddr> GetUsers()
    {
        return _users;
    }
private:
    std::vector<InetAddr> _users;
};
