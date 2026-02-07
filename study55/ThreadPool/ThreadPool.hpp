#pragma once
#include <iostream>
#include "Logger.hpp"
#include "Thread.hpp"
#include <vector>
#include <queue>
#include "Mutex.hpp"
#include "Cond.hpp"

namespace NS_THREAD_POOL_MODULE
{
    using namespace NS_LOG_MODULE;
    using namespace NS_THREAD_MODULE;

    void Test()
    {
        char name[128];
        pthread_getname_np(pthread_self(), name, sizeof(name));
        while (true)
        {
            LOG(LogLevel::DEBUG) << "我是一个线程，我要进行运行:" << name;
            sleep(1);
        }
    }

    const int defaultnum = 5;
    template <class T>
    class ThreadPool
    {
    private:
        void HandlerTask()
        {
            char name[128];
            pthread_getname_np(pthread_self(), name, sizeof(name));
            while (true)
            {
                _mutex.Lock();

                while (_tasks.empty())
                {
                    _slaver_sleep_count++;
                    _cond.Wait(&_mutex);
                    _slaver_sleep_count--;
                }

                T task = _tasks.front();
                _tasks.pop();

                task();

                _mutex.Unlock();
            }
        }

    public:
        ThreadPool()
            : isrunning(false),
              _slaver_sleep_count(0)
        {
            for (int idx = 0; idx < _slaver_num; idx++)
            {
                _slavers.emplace_back(Test);
            }
        }
        void Start()
        {
            if (_isrunning)
            {
                LOG(LogLevel::WARNING) << "Thread Pool Is Already Running";
                return;
            }
            _isrunning = true;
            for (auto &slave : _slavers)
            {
                slave.Start();
            }
        }
        void Stop()
        {
            if (!_isrunning)
            {
                LOG(LogLevel::WARNING) << "Thread Pool Is Not Running";
                return;
            }
            for (auto &slave : _slavers)
            {
                slave.Die(); // 不建议
            }
            _isrunning = false;
        }
        void Wait()
        {
            for (auto &slave : _slavers)
            {
                slave.Join();
            }
        }
        void Enqueue(T in)
        {
            _mutex.Lock();

            _tasks.push(in);
            if (_slaver_sleep_count > 0)
            {
                _cond.Signal();
            }
            _mutex.Unlock();
        }
        ~ThreadPool()
        {
        }

    private:
        bool _isrunning;
        int _slaver_num;
        std::vector<Thread> _slavers;
        std::queue<T> _tasks;
        Mutex _mutex;
        Cond _cond;
        int _slaver_sleep_count;
    };
}