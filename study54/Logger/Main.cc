#include "Logger.hpp"
using namespace NS_LOG_MODULE;

int main()
{
    ENABLE_CONSOLE_LOG_STRATEGY();
    //logger.Debug("CONSOLE STRATEGY!\n");
    //ENABLE_FILE_LOG_STRATEGY();
    //logger.Debug("filestrategy!\n");


    logger(LogLevel::DEBUG,__FILE__,__LINE__);
    return 0;
}