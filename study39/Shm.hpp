#pragma once
#include <iostream>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <cstdio>

const int gsize = 128;

#define PATHNAME "/tmp"
#define PROJ_ID 0x66

class Shm
{
public:
    Shm()
        : _shmid(-1), _size(gsize), _start_addr(nullptr)
    {
    }
    void Create()
    {
        GetHelper(IPC_CREAT|IPC_EXCL|0666);
    }
    void Get()
    {
        GetHelper(IPC_CREAT);
    }
    void Delete()
    {
        int n = shmctl(_shmid, IPC_RMID, nullptr);
    }
    void Attach()
    {
        _start_addr = shmat(_shmid, nullptr, 0);
        if ((long long int)_start_addr == -1)
            exit(3);
    }
    void Detach()
    {
        int n = shmdt(_start_addr);
        (void)n;
    }
    void* Addr()
    {
        return _start_addr;
    }
    int Size()
    {
        return _size;
    }
    void Attr()
    {
        struct shmid_ds ds;
        int n = shmctl(_shmid,IPC_STAT,&ds);
        if(n < 0)
        {
            perror("shmctl ");
            exit(4);
        }

    }
    ~Shm()
    {
    }

private:
    void GetHelper(int shmflg)
    {
        key_t k = GetKey();
        if (k < 0)
        {
            std::cerr << "GetKey error " << std::endl;
            exit(1);
        }
        printf("key : 0x%x \n", k);
        _shmid = shmget(k, _size, shmflg);
        if (_shmid < 0)
        {
            perror("shmget ");
            exit(2);
        }
        printf("key : 0x%x , shmid :%d \n", k, _shmid);
    }
    key_t GetKey()
    {
        return ftok(PATHNAME, PROJ_ID);
    }

private:
    int _shmid;
    int _size;
    void *_start_addr;
};