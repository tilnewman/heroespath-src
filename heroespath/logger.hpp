#ifndef APPBASE_HEROESPATH_LOGGER_INCLUDED
#define APPBASE_HEROESPATH_LOGGER_INCLUDED
//
// logger.hpp
//  A singleton logging class.
//
#include "logbase/logbase.hpp"
#include <memory>
#include <string>


namespace heroespath
{

    class Logger;
    using LoggerSPtr_t = std::shared_ptr<Logger>;


    //Singleton class that logs to a file and std out
    class Logger : public appbase::logbase::LogBase
    {
        //prevent copy assignment, copy construction, and non-singleton construction
        Logger(const Logger &);
        Logger & operator=(const Logger &);
        Logger();

    public:
        virtual ~Logger();

        static LoggerSPtr_t Instance();

    private:
        static LoggerSPtr_t instanceSPtr_;
    };

}
#endif //APPBASE_HEROESPATH_LOGGER_INCLUDED
