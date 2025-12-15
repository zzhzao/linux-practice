#ifndef __BLOCK_QUEUE_H

#define __BLOCK_QUEUE_H
#include <iostream>
#include <queue>
#include <pthread.h>
const int defaultcap = 5;

template <class T>
class BlockQueue
{
public:
    BlockQueue(int cap = defaultcap) : _cap(cap)
    {
        pthread_mutex_init(&_mutex, nullptr);
        pthread_cond_init(&_consumer_cond, nullptr);
        pthread_cond_init(&_productor_cond, nullptr);
    }
    void Enqueue(T &in)
    {
        pthread_mutex_lock(&_mutex);

        while (_bq.size() == _cap)
        {
            pthread_cond_wait(&_productor_cond, &_mutex);
        }
        _bq.push(in);
        pthread_cond_signal(&_consumer_cond);
        pthread_mutex_unlock(&_mutex);
    }
    void Pop(T *out)
    {
        pthread_mutex_lock(&_mutex);

        while (_bq.empty())
        {
            pthread_cond_wait(&_consumer_cond, &_mutex);
        }
        *out = _bq.front();
        _bq.pop();
        pthread_cond_signal(&_productor_cond);
        pthread_mutex_unlock(&_mutex);
    }
    ~BlockQueue()
    {
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_consumer_cond);
        pthread_cond_destroy(&_productor_cond);
    }

private:
    std::queue<T> _bq;
    int _cap; // 阻塞队列为满的情况
    pthread_mutex_t _mutex;
    pthread_cond_t _consumer_cond;
    pthread_cond_t _productor_cond;

    // int _consumer_low_water;
    // int _productor_low_water;
    int _blockqueue_low_water;
    int _blockqueue_high_water;
};

#endif