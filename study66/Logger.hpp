#ifndef __LOGGER_HPP
#define __LOGGER_HPP

#include <iostream>
#include <cstdio>
#include <string>
#include <memory>
#include <sstream>
#include <ctime>
#include <sys/time.h>
#include <unistd.h>
#include <filesystem> // C++17
#include <fstream>
#include "Mutex.hpp"

namespace NS_LOG_MODULE
{
    enum class LogLevel
    {
        INFO,
        WARNING,
        ERROR,
        FATAL,
        DEBUG
    };
    std::string LogLevel2Message(LogLevel level)
    {
        switch (level)
        {
        case LogLevel::INFO:
            return "INFO";
        case LogLevel::WARNING:
            return "WARNING";
        case LogLevel::ERROR:
            return "ERROR";
        case LogLevel::FATAL:
            return "FATAL";
        case LogLevel::DEBUG:
            return "DEBUG";
        default:
            return "UNKNOWN";
        }
    }

    // 1. 时间戳 2. 日期+时间
    std::string GetCurrentTime()
    {
        struct timeval current_time;
        int n = gettimeofday(&current_time, nullptr);
        (void)n;

        // current_time.tv_sec; current_time.tv_usec;
        struct tm struct_time;
        localtime_r(&(current_time.tv_sec), &struct_time); // r: 可重入函数
        char timestr[128];
        snprintf(timestr, sizeof(timestr), "%04d-%02d-%02d %02d:%02d:%02d.%ld",
                 struct_time.tm_year + 1900,
                 struct_time.tm_mon + 1,
                 struct_time.tm_mday,
                 struct_time.tm_hour,
                 struct_time.tm_min,
                 struct_time.tm_sec,
                 current_time.tv_usec);
        return timestr;
    }

    // 输出角度 -- 刷新策略
    // 1. 显示器打印
    // 2. 文件写入
    // 策略模式，策略接口
    class LogStrategy
    {
    public:
        virtual ~LogStrategy() = default;
        virtual void SyncLog(const std::string &message) = 0;
    };
    // 控制台日志刷新策略, 日志将来要向显示器打印
    class ConsoleStrategy : public LogStrategy
    {
    public:
        void SyncLog(const std::string &message) override
        {
            LockGuard lockguard(_mutex);
            std::cerr << message << std::endl; // ??
        }
        ~ConsoleStrategy()
        {
        }

    private:
        Mutex _mutex;
    };

    const std::string defaultpath = "./log";
    const std::string defaultfilename = "log.txt";


    // 文件策略
    class FileLogStrategy : public LogStrategy
    {
    public:
        FileLogStrategy(const std::string &path = defaultpath, const std::string &name = defaultfilename)
            : _logpath(path),
              _logfilename(name)
        {
            LockGuard lockguard(_mutex);
            if (std::filesystem::exists(_logpath))
                return;
            try
            {
                std::filesystem::create_directories(_logpath);
            }
            catch (const std::filesystem::filesystem_error &e)
            {
                std::cerr << e.what() << '\n';
            }
        }

        void SyncLog(const std::string &message) override
        {
            {
                LockGuard lockguard(_mutex);
                if (!_logpath.empty() && _logpath.back() != '/')
                {
                    _logpath += "/";
                }
                std::string targetlog = _logpath + _logfilename; // "./log/log.txt"
                std::ofstream out(targetlog, std::ios::app);     // 追加方式写入
                if (!out.is_open())
                {
                    std::cerr << "open " << targetlog << "failed" << std::endl;
                    return;
                }
                out << message << "\n";
                out.close();
            }
        }

        ~FileLogStrategy()
        {
        }

    private:
        std::string _logpath;
        std::string _logfilename;
        Mutex _mutex;
    };

    // 交给大家
    // const std::string defaultfilename = "log.info";
    // const std::string defaultfilename = "log.warning";
    // const std::string defaultfilename = "log.fatal";
    // const std::string defaultfilename = "log.error";
    // const std::string defaultfilename = "log.debug";
     // 文件策略&&分日志等级来进行保存
    // class FileLogLevelStrategy : public LogStrategy
    // {
    // public:
    // private:
    // };


    // 日志类:
    // 1. 日志的生成
    // 2. 根据不同的策略，进行刷新
    class Logger
    {
        // 日志的生成:
        // 构建日志字符串
    public:
        Logger()
        {
            UseConsoleStrategy();
        }
        void UseConsoleStrategy()
        {
            _strategy = std::make_unique<ConsoleStrategy>();
        }
        void UseFileStrategy()
        {
            _strategy = std::make_unique<FileLogStrategy>();
        }
        // 内部类, 标识一条完整的日志信息
        //  一条完整的日志信息 = 做半部分固定部分 + 右半部分不固定部分
        //  LogMessage RAII风格的方式，进行刷新
        class LogMessage
        {
        public:
            LogMessage(LogLevel level, std::string &filename, int line, Logger &logger)
                : _level(level),
                  _curr_time(GetCurrentTime()),
                  _pid(getpid()),
                  _filename(filename),
                  _line(line),
                  _logger(logger)
            {
                // 先构建出来左半部分
                std::stringstream ss;
                ss << "[" << _curr_time << "] "
                   << "[" << LogLevel2Message(_level) << "] "
                   << "[" << _pid << "] "
                   << "[" << _filename << "] "
                   << "[" << _line << "] "
                   << " - ";

                _loginfo = ss.str();
            }
            template <typename T>
            LogMessage &operator<<(const T &info)
            {
                std::stringstream ss;
                ss << info;
                _loginfo += ss.str();
                return *this; // 返回当前LogMessage对象，方便下次继续进行<<
            }

            ~LogMessage()
            {
                if (_logger._strategy)
                {
                    _logger._strategy->SyncLog(_loginfo);
                }
            }

        private:
            LogLevel _level;
            std::string _curr_time;
            pid_t _pid;
            std::string _filename;
            int _line;
            std::string _loginfo; // 一条完整的日志信息

            // 一个引用，引用外部的Logger类对象
            Logger &_logger; // 方便我们后续进行策略式刷新
        };

        // 这里已经不是内部类了
        // 故意采用拷贝LogMessage
        LogMessage operator()(LogLevel level, std::string filename, int line)
        {
            return LogMessage(level, filename, line, *this);
        }

        ~Logger()
        {
        }

    private:
        std::unique_ptr<LogStrategy> _strategy; // 刷新策略
    };

    // 日志对象，全局使用
    Logger logger;

#define ENABLE_CONSOLE_LOG_STRATEGY() logger.UseConsoleStrategy();
#define ENABLE_FILE_LOG_STRATEGY() logger.UseFileStrategy();

#define LOG(level) logger(level, __FILE__, __LINE__)

}

#endif