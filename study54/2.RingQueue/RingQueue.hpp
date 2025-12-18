#pragma once
#include <iostream>
#include <pthread.h>
#include <vector>
#include <string>
#include "Sem.hpp"
#include "Mutex.hpp"
const int defaultcap = 5;

template <class T>
class RingQueue
{
public:
    RingQueue(int cap = defaultcap)
        : _cap(cap),
          _rq(cap),
          _consumer_step(0),
          _productor_step(0),
          _blank_sem(cap),
          _data_sem(0)
    {
    }
    void Enqueue(T &in)
    {

        _blank_sem.P();

        {
            LockGuard lockguard(_pmutex);
            // 找位置生产
            _rq[_productor_step++] = in;
            _productor_step %= _cap;
        }
        _data_sem.V();
    }
    void Pop(T *out)
    {

        _data_sem.P();
        {
            LockGuard lockguard(_cmutex);
            *out = _rq[_consumer_step++];
            _consumer_step %= _cap;
        }

        _blank_sem.V();
    }
    ~RingQueue()
    {
    }

private:
    int _cap; // 环形队列容量
    std::vector<T> _rq;

    int _consumer_step;  // 消费位置
    int _productor_step; // 生产位置

    Sem _blank_sem; // 空位置
    Sem _data_sem;  // 数据信号量

    Mutex _cmutex;
    Mutex _pmutex;
};