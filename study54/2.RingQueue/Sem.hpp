#ifndef __SEM_HPP
#define __SEM_HPP

#include <iostream>
#include <semaphore.h>

class Sem
{
public:
    Sem(int init_val)
    {
        if (init_val >= 0)
            sem_init(&_sem, 0, init_val);
    }
    void P()
    {
        int n = sem_wait(&_sem);
        (void)n;
    }
    void V()
    {
        int n = sem_post(&_sem);
        (void)n;
    }

    ~Sem()
    {
        int n = sem_destroy(&_sem);
        (void)n;
    }

private:
    sem_t _sem;
};

#endif