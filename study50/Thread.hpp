#pragma once

#include <iostream>
#include <pthread.h>
#include <string>
#include <functional>

namespace ThreadModule
{
    static int gnumber = 1;
    using callback_t = std::function<void()>;
    enum class TSTATUS
    {
        THREAD_NEW,
        THREAD_RUNNING,
        THREAD_STOP
    };

    std::string Status2String(TSTATUS s)
    {
        switch (s)
        {
        case TSTATUS::THREAD_NEW:
            return "THREAD_NEW";
        case TSTATUS::THREAD_RUNNING:
            return "THREAD_RUNNING";
        case TSTATUS::THREAD_STOP:
            return "THREAD_STOP";
        default:
            return "UNKNOWN";
        }
    }
    class Thread
    {
    private:
        void ToRunning()
        {
            _status = TSTATUS::THREAD_RUNNING;
        }
        void ToStop()
        {
            _status = TSTATUS::THREAD_STOP;
        }
        static void *ThreadRoutine(void *args)
        {
            Thread *self = static_cast<Thread *>(args);
       
            pthread_setname_np(self->_tid, self->_name.c_str());
            self->_cb();
            self->ToStop();
            return nullptr;
        }
        std::string IsJoined(bool joinable)
        {
            return joinable ? "true" : "false";
        }

    public:
        Thread(callback_t cb)
            : _tid(-1), _status(TSTATUS::THREAD_NEW), _cb(cb), _result(nullptr)
        {
            _name = "Thread-" + std::to_string(gnumber++);
        }
        bool Start()
        {
            int n = pthread_create(&_tid, nullptr, ThreadRoutine, this);
            if (n != 0)
            {
                return false;
            }
            ToRunning();
            return true;
        }
        void Join()
        {
            if (_joinable)
            {
                int n = pthread_join(_tid, &_result);
                if (n != 0)
                {
                    std::cerr << "join error:" << n << std::endl;
                }
                (void)_result;
                _status = TSTATUS::THREAD_STOP;
            }
            else
            {
                std::cerr << "error thread join status: " << _joinable << std::endl;
            }
        }
        void Stop()
        {
            if (_status == TSTATUS::THREAD_RUNNING)
            {
                pthread_cancel(_tid);
                _status = TSTATUS::THREAD_STOP;
            }
        }
        void Detach()
        {
            if (_status == TSTATUS::THREAD_RUNNING && _joinable)
            {
                pthread_detach(_tid);
                _joinable = false;
            }
            else
            {
                std::cerr << "detach" << _name << "failed" << std::endl;
            }
        }

        void PrintInfo()
        {
            std::cout << "thread name : " << _name << std::endl;
            std::cout << "thread _tid : " << _tid << std::endl;
            std::cout << "thread _status : " << Status2String(_status) << std::endl;
            std::cout << "thread _joinable : " << IsJoined(_joinable) << std::endl;
        }
        ~Thread()
        {
        }

    private:
        std::string _name;
        pthread_t _tid;
        TSTATUS _status;
        bool _joinable = true;
        callback_t _cb;

        void *_result;
    };
}