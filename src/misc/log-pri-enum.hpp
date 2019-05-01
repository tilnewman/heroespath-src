// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_MISC_LOG_PRI_ENUM_HPP_INCLUDED
#define HEROESPATH_MISC_LOG_PRI_ENUM_HPP_INCLUDED
//
// log-pri-enum.hpp
//

namespace heroespath
{
namespace misc
{

    struct LogPriority
    {
        enum Enum
        {
            Debug,
            Default,
            Warn,
            Error,
            Fatal
        };

        static const char * ToString(const Enum PRIORITY)
        {
            switch (PRIORITY)
            {
                case Debug: return "Debug";
                case Default: return "Default";
                case Warn: return "Warn";
                case Error: return "Error";
                case Fatal: return "Fatal";
                default: return "Enum_value_out_of_range";
            }
        }

        static const char * ToStringAcronym(const Enum PRIORITY)
        {
            switch (PRIORITY)
            {
                case Debug: return "DBG";
                case Default: return "DEF";
                case Warn: return "WRN";
                case Error: return "ERR";
                case Fatal: return "FAT";
                default: return "Enum_value_out_of_range";
            }
        }

        static const char * ConsoleColorStringBegin(const Enum PRIORITY)
        {
            switch (PRIORITY)
            {
                case Debug: return "\033[36;40m"; // cyan on black
                case Warn: return "\033[33;40m"; // yellow on black
                case Error:
                case Fatal: return "\033[31;40m"; // red on black
                case Default:
                default: return "Enum_value_out_of_range";
            }
        }

        static const char * ConsoleColorStringEnd() { return "\033[0;0m"; }
    };

} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_LOG_PRI_ENUM_HPP_INCLUDED
