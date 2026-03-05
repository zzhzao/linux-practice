#ifndef __COND_HPP
#define __COND_HPP

#include <pthread.h>
#include "Mutex.hpp"

class Cond
{
public:
    Cond()
    {
        pthread_cond_init(&_cond, nullptr);
    }
    void Wait(Mutex &mutex)
    {
        int n = pthread_cond_wait(&_cond, mutex.Ptr());
        (void)n;
    }
    void Signal()
    {
        int n = pthread_cond_signal(&_cond);
        (void)n;
    }
    void Broadcast()
    {
        int n = pthread_cond_broadcast(&_cond);
        (void)n;
    }
    ~Cond()
    {
        pthread_cond_destroy(&_cond);
    }
private:
    pthread_cond_t _cond;
};

#endif