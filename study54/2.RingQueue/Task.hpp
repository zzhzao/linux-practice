#ifndef __TASK_HPP
#define __TASK_HPP

#include <iostream>
#include <string>
#include <functional>

using task_t = std::function<void()>;


class Task
{
public:
    Task()
    {
    }
    Task(int x, int y)
        : _x(x), _y(y)
    {
    }
    void Execute()
    {
        _result = _x + _y;
    }
    void operator()()
    {
        Execute();
    }
    std::string getResult()
    {
        return std::to_string(_x) + "+" + std::to_string(_y) + "=" + std::to_string(_result);
    }
    std::string Question()
    {
        return std::to_string(_x) + "+" + std::to_string(_y) + "=?";
    }
    ~Task() {}

private:
    int _x;
    int _y;
    int _result;
};

class CalTask : public Task{};
class StorageTask : public Task{};
class NetTask : public Task{};

#endif