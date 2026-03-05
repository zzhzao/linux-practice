#pragma once

#include <iostream>
#include <vector>
#include <queue>
#include "Logger.hpp"
#include "Thread.hpp"
#include "Mutex.hpp"
#include "Cond.hpp"

namespace NS_THREAD_POOL_MODULE
{
    using namespace NS_LOG_MODULE;
    using namespace NS_THREAD_MODULE;

    const int defaultnum = 15;

    // void Test()
    // {
    //     char name[128];
    //     pthread_getname_np(pthread_self(), name, sizeof(name));
    //     while (true)
    //     {
    //         LOG(LogLevel::DEBUG) << "我是一个线程，我要进行运行:" << name;
    //         sleep(1);
    //     }
    // }

    // 线程池要不要对多个线程进行管理呢？？
    // 先描述，在组织！
    template <typename T>
    class ThreadPool
    {
    private:
        void HandlerTask()
        {
            char name[128];
            pthread_getname_np(pthread_self(), name, sizeof(name));
            while (true)
            {
                T task;
                {
                    // 保护临界区
                    LockGuard lockguard(_mutex);
                    // 检测任务。不休眠：1. 队列不为空 2. 线程池退出 -> 队列为空 && 线程池不退出
                    while (_tasks.empty() && _isrunning)
                    {
                        // 没有任务, 休眠
                        _slaver_sleep_count++;
                        _cond.Wait(_mutex);
                        _slaver_sleep_count--;
                    }
                    // 线程池退出了-> while 就要break -> 不能
                    // 1. 线程池退出 && _tasks empty
                    if (!_isrunning && _tasks.empty())
                    {
                        _mutex.Unlock();
                        break;
                    }

                    // 有任务, 取任务，本质：把任务由公共变成私有
                    // T -> task*
                    task = _tasks.front();
                    _tasks.pop();
                }

                // 处理任务, 约定
                // 处理任务需要再临界区内部处理吗？不需要
                LOG(LogLevel::INFO) << name << "处理任务:";
                task();
            }

            // 线程退出
            LOG(LogLevel::INFO) << name << " quit...";
        }

        ThreadPool(int slaver_num = defaultnum) : _isrunning(false), _slaver_sleep_count(0), _slaver_num(slaver_num)
        {
            // ThreadPool对象已经存在了
            for (int idx = 0; idx < _slaver_num; idx++)
            {
                // auto f = std::bind(&ThreadPool::HandlerTask, this);
                // // auto f = [this](){
                // //      this->HandlerTask();
                // // };
                // _slavers.emplace_back(f);
                _slavers.emplace_back([this]()
                                      { this->HandlerTask(); });
            }
        }
        // 赋值 拷贝构造禁止
        ThreadPool<T> &operator=(const ThreadPool<T> &) = delete;
        ThreadPool(const ThreadPool<T> &) = delete;

    public:
        // 如果多线程获取这个单例呢.加锁
        // 多线程安全了，但是效率比较低，双if判断
        static ThreadPool<T> *Instance()
        {
            if(nullptr == _instance) // 双if判断
            {
                // 多线程
                LockGuard lockguard(_lock);
                if (nullptr == _instance)
                {
                    // 第一次调用
                    _instance = new ThreadPool<T>();
                    _instance->Start();
                    LOG(LogLevel::INFO) << "第一次使用线程池，创建线程池对象";
                }
            }
            return _instance;
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
            // version1 -- 后续调整
            // if (!_isrunning)
            // {
            //     LOG(LogLevel::WARNING) << "Thread Pool Is Not Running";
            //     return;
            // }
            // for (auto &slave : _slavers)
            // {
            //     slave.Die(); // 太简单粗暴了
            // }
            // _isrunning = false;
            // version 2
            // 1. _isrunning = false
            // 2. 处理完成tasks所有的任务
            // 线程状态: 休眠，正在处理任务 -> 让所有线程全部唤醒
            // HandlerTask自动break
            _mutex.Lock();
            _isrunning = false;
            if (_slaver_sleep_count > 0)
                _cond.Broadcast();
            _mutex.Unlock();
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
                _cond.Signal();
            _mutex.Unlock();
        }
        ~ThreadPool()
        {
        }

    private:
        bool _isrunning;
        int _slaver_num;
        std::vector<Thread> _slavers;
        std::queue<T> _tasks; // 任务队列，临界资源
        Mutex _mutex;
        Cond _cond;
        int _slaver_sleep_count;

        // 添加单例模式
        static ThreadPool<T> *_instance;
        static Mutex _lock; // 保证单例的安全
    };

    template <typename T>
    ThreadPool<T> *ThreadPool<T>::_instance = nullptr;

    template <typename T>
    Mutex ThreadPool<T>::_lock;

}