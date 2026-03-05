#pragma once
#include <string>
#include <cstdio>
#include <iostream>
#include <vector>
class Command
{
private:
    void LoadCommand()
    {
        _whitelist.push_back("ls -a -l");
        _whitelist.push_back("ls -l");
        _whitelist.push_back("pwd");
        _whitelist.push_back("whoami");
        _whitelist.push_back("who");
        _whitelist.push_back("ps -al");
        _whitelist.push_back("netstat -nltp");
        _whitelist.push_back("netstat -nutp");
        _whitelist.push_back("netstat -natp");
        _whitelist.push_back("netstat -naup");
    }
    bool IsSafe(const std::string &cmd)
    {
        for (auto &elem : _whitelist)
        {
            if (elem == cmd)
                return true;
        }
        return false;
    }

public:
    std::string Excute(const std::string &cmd)
    {
        if (!IsSafe(cmd))
        {
            return "bad man!";
        }

        FILE *fp = popen(cmd.c_str(), "r");
        if (fp == nullptr)
            return "execute error!";
        std::string result;
        char buffer[1024];
        while (fgets(buffer, sizeof(buffer), fp))
        {
            result += buffer;
        }

        pclose(fp);

        return result;
    }

private:
    std::vector<std::string> _whitelist;
};
