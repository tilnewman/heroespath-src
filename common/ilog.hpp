#ifndef APPBASE_ILOG_HPP
#define APPBASE_ILOG_HPP
//
// ilog.hpp
//
#include <string>


namespace appbase
{
    //simple wrapper for logger priority enum
    struct LogPri
    {
        enum Enum
        {
            None = 0,
            Debug,
            Default,
            Warn,
            Warning = Warn,
            Error,
            Fatal,
            Count,
            Disabled = Count,
        };

        static const std::string GetName(const Enum X)
        {
            switch(X)
            {
                case Debug:     { return "Debug";   }
                case Default:   { return "Default"; }
                case Warn:      { return "Warning"; }
                case Error:     { return "Error";   }
                case Fatal:     { return "Fatal";   }
                case None:      //intentional fall-through
                case Count:
                default:        { return "None";    }
            }
        }
    };


    //
    // ILog Class
    //  Usage interface
    class ILog
    {
    public:
        virtual ~ILog() {}

        virtual void Log(const std::string & MSG) = 0;

        virtual void Log(const std::string & FILE,
                         const int LINE, //why signed?
                         const std::string & MSG) = 0;

        virtual void Log(const LogPri::Enum Default,
                         const std::string & MSG) = 0;

        virtual void Log(const LogPri::Enum Default,
                         const std::string & FILE,
                         const int LINE, //why signed?
                         const std::string & MSG) = 0;
    };

}//end of namespace appbase


#endif //APPBASE_ILOG_HPP
