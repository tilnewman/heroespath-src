//
// logger.cpp
//
#include "logger.hpp"


namespace game
{

    LoggerSPtr_t Logger::instanceSPtr_;


    Logger::Logger()
    :
        LogBase(appbase::logbase::LogBase::FILE_NAME_DEFAULT, appbase::logbase::LogBase::FILE_NAME_EXT_DEFAULT, "")
    {}


    Logger::~Logger()
    {}


    LoggerSPtr_t Logger::Instance()
    {
        if (nullptr == instanceSPtr_.get())
            instanceSPtr_.reset( new Logger() );

        return instanceSPtr_;
    }

}