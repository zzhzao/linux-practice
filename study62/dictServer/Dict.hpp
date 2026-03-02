#pragma once

#include<iostream>
#include<unordered_map>
#include<fstream>
#include<string>
#include "Logger.hpp"
using namespace NS_LOG_MODULE;
  
static const std::string default_path = "./dict.txt";
static const std::string sep = ": ";
class Dict
{
public:
    Dict(const std::string &dict_path = default_path)
    {
        _dict_path = dict_path;
    }
    ~Dict() {}
    void LoadDict()
    {
        std::ifstream in(_dict_path);
        if(!in.is_open())
        {
            LOG(LogLevel::FATAL) << "open error";
            exit(1);
        }
        std::string line;
        while(std::getline(in,line))
        {
            LOG(LogLevel::DEBUG) << "load " << line << "success";
            auto pos = line.find(sep);
            if(pos == std::string::npos)
            {   
                LOG(LogLevel::WARNING) << "invalid line: " << line;
                continue;
            }
            std::string word = line.substr(0,pos);
            std::string meaning = line.substr(pos + sep.size());
            _dict[word] = meaning;

        }
        in.close();
        LOG(LogLevel::INFO) << "load dict success";

    }
    std::string Translate(const std::string &word)
    {
        auto it = _dict.find(word);
        if(it == _dict.end())
        {
            return "not found";
        }
        return it->second;
    }
private:
    std::string _dict_path;
    std::unordered_map<std::string,std::string> _dict;
};