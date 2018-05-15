// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_LOG_ILOG_HPP_INCLUDED
#define HEROESPATH_LOG_ILOG_HPP_INCLUDED
//
// ilog.hpp
//
#include <string>

namespace heroespath
{
namespace log
{

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
            switch (X)
            {
                case Debug:
                {
                    return "Debug";
                }
                case Default:
                {
                    return "Default";
                }
                case Warn:
                {
                    return "Warning";
                }
                case Error:
                {
                    return "Error";
                }
                case Fatal:
                {
                    return "Fatal";
                }
                case None:
                case Count:
                default:
                {
                    return "None";
                }
            }
        }
    };

    class ILog
    {
    public:
        virtual ~ILog() = default;

        virtual void Log(const std::string & MSG) = 0;

        virtual void
            Log(const std::string & FILE,
                const int LINE, // why signed?
                const std::string & MSG)
            = 0;

        virtual void Log(const LogPri::Enum Default, const std::string & MSG) = 0;

        virtual void
            Log(const LogPri::Enum Default,
                const std::string & FILE,
                const int LINE, // why signed?
                const std::string & MSG)
            = 0;
    };
} // namespace log
} // namespace heroespath

#endif // HEROESPATH_LOG_ILOG_HPP_INCLUDED
