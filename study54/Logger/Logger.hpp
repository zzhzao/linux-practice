#ifndef __LOGGER_HPP
#define __LOGGER_HPP

#include <iostream>
#include <string>
#include <sys/time.h>
#include <ctime>
#include <cstdio>
#include <unistd.h>
#include "Mutex.hpp"
#include <filesystem> // C++17
#include <fstream>
#include <memory>
#include <sstream>

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
    std::string GetCurrentTime()
    {
        struct timeval current_time;
        int n = gettimeofday(&current_time, nullptr);
        (void)n;
        struct tm struct_time;
        localtime_r(&(current_time.tv_sec), &struct_time);

        char timestr[128];
        snprintf(timestr, sizeof(timestr), "%04d-%02d-%02d %02d:%02d:%02d.%ld",
                 struct_time.tm_year + 1900,
                 struct_time.tm_mon,
                 struct_time.tm_mday,
                 struct_time.tm_hour,
                 struct_time.tm_min,
                 struct_time.tm_sec,
                 current_time.tv_usec);
        return timestr;
    }

    class LogStrategy
    {
    public:
        virtual ~LogStrategy() = default;
        virtual void SyncLog(const std::string &message) = 0;
    };
    // 显示器策略，日志将来要向显示器打印
    class ConsoleStrategy : public LogStrategy
    {
    public:
        void SyncLog(const std::string &message) override
        {
            LockGuard lockguard(_mutex);
            std::cerr << message << std::endl; // ??
        }
        ~ConsoleStrategy() {}

    private:
        Mutex _mutex;
    };
    // 文件策略
    const std::string defaultpath = "./log";
    const std::string defaultfilename = "log.txt";
    class FileStrategy : public LogStrategy
    {
    public:
        FileStrategy(const std::string &path = defaultpath, const std::string &name = defaultfilename)
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
        virtual void SyncLog(const std::string &message)
        {
            LockGuard lockguard(_mutex);
            if (!_logpath.empty() && _logpath.back() != '/')
            {
                _logpath += "/";
            }
            std::string targetlog = _logpath + _logfilename; // c++文件操作
            std::ofstream out(targetlog, std::ios::app);
            if (!out.is_open())
            {
                return;
            }
            out << message << "\n";
            out.close();
        }
        ~FileStrategy()
        {
        }

    private:
        std::string _logpath;
        std::string _logfilename;
        Mutex _mutex;
    };
    class Logger
    {
    public:
        Logger()
        {
        }
        void UseConsoleStrategy()
        {
            _strategy = std::make_unique<ConsoleStrategy>();
        }
        void UseFileStrategy()
        {
            _strategy = std::make_unique<FileStrategy>();
        }
        class LogMessage
        {
        public:
            LogMessage(LogLevel level, std::string &filename, int line,Logger &logger)
                : _level(level),
                  _curr_time(GetCurrentTime()),
                  _pid(getpid()),
                  _filename(filename),
                  _line(line),
                  _logger(logger)
            {
                std::stringstream ss;
                ss << "[" << _curr_time << "] "
                   << "[" << LogLevel2Message(_level) << "] "
                   << "[" << _pid << "] "
                   << "[" << _filename << "] "
                   << "[" << _line << "] "
                   << " - ";

                _loginfo = ss.str();
            }
            template<typename T>
            LogMessage &operator<<(const T &info)
            {
                std::stringstream ss;
                ss << info;
                _loginfo += ss.str();
                return *this; // 返回当前LogMessage对象，方便下次继续进行<<
            }
            ~LogMessage()
            {
                if(_logger._strategy)
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
            std::string _loginfo;

            Logger &_logger;//引用外部logger，方便进行策略式刷新
        };

        //故意采用拷贝Logmessage
        LogMessage operator()(LogLevel level,std::string filename,int line)
        {
            return LogMessage(level,filename,line,logger);
        }
        ~Logger()
        {
        }

    private:
        std::unique_ptr<LogStrategy> _strategy;
    };

    Logger logger;

#define ENABLE_CONSOLE_LOG_STRATEGY() logger.UseConsoleStrategy();
#define ENABLE_FILE_LOG_STRATEGY() logger.UseFileStrategy();

#define LOG(level) logger(level,__FILE__,__LINE__)

};

#endif
