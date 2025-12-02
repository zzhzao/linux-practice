#pragma once

#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstdlib>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#define ForRead 1
#define ForWrite 2

const std::string gcommfile = "./fifo";
class Fifo
{
public:
    Fifo(const std::string &commfile = gcommfile)
        : _commfile(commfile), _mode(0666), _fd(-1)
    {
    }
    void Build()
    {
        if (IsExists())
            return;
        umask(0);
        int n = mkfifo(_commfile.c_str(), _mode);
        if (n < 0)
        {
            std::cerr << "mkfifo error: " << strerror(errno) << " errno: " << errno << std::endl;
            exit(1);
        }
        std::cerr << "mkfifo success: " << strerror(errno) << " errno: " << errno << std::endl;
    }
    void Open(int mode)
    {
        if (mode == ForRead)
        {
            _fd = open(_commfile.c_str(), O_RDONLY);
        }
        else if (mode == ForWrite)
        {
            _fd = open(_commfile.c_str(), O_WRONLY);
        }
        else
        {
        }
        if (_fd < 0)
        {
            std::cerr << "open error: " << strerror(errno) << " errno: " << errno << std::endl;
        }
        else
        {
            std::cout<< " open sucess " << std::endl;
        }
    }
    void Delete()
    {
        if (!IsExists())
            return;
        int n = unlink(_commfile.c_str());
        (void)n;
    }
    void Send(const std::string &msgin)
    {
        ssize_t n = write(_fd,msgin.c_str(),msgin.size());
        (void)n;
    }
    int Recv(std::string *msgout) 
    {
        char buffer[128];
        ssize_t n = read(_fd,buffer,sizeof(buffer) - 1);
        if(n > 0)
        {
            buffer[n] = 0;
            *msgout = buffer;
            return n;
        }
        else if(n == 0)
        {
            return 0;
        }
        else 
        {
            return -1;
        }
    }
    ~Fifo()
    {}
private:
    bool IsExists()
    {
        struct stat st;
        int n = stat(_commfile.c_str(), &st);
        if (n == 0)
        {
            // std::cout << "文件存在" << std::endl;
            return true;
        }
        else
        {
            errno = 0;
            // std::cout << "文件不存在: " << errno << std::endl;
            return false;
        }
    }

private:
    std::string _commfile;
    mode_t _mode;
    int _fd;
};