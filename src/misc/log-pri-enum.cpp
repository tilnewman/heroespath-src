// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// log-pri-enum.cpp
//
#include "log-pri-enum.hpp"

#include "misc/platform.hpp"

#include <iostream>
#include <sstream>

namespace heroespath
{
namespace misc
{

    bool LogPriority::IsValid(const unsigned int ENUM_VALUE) { return (ENUM_VALUE < Count); }

    const std::string LogPriority::ToString(const Enum PRIORITY)
    {
        switch (PRIORITY)
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
                return "Warn";
            }
            case Error:
            {
                return "Error";
            }
            case Fatal:
            {
                return "Fatal";
            }
            case Count:
            default:
            {
                std::cerr << MakeErrorString(PRIORITY, "ToString") << std::endl;
                return "(enum_out_of_bounds_error)";
            }
        }
    }

    const std::string LogPriority::ToStringAcronym(const Enum PRIORITY)
    {
        switch (PRIORITY)
        {
            case Debug:
            {
                return "DBG";
            }
            case Default:
            {
                return "DEF";
            }
            case Warn:
            {
                return "WRN";
            }
            case Error:
            {
                return "ERR";
            }
            case Fatal:
            {
                return "FAT";
            }
            case Count:
            default:
            {
                std::cerr << MakeErrorString(PRIORITY, "ToStringAcronym") << std::endl;
                return "(enum_out_of_bounds_error)";
            }
        }
    }

    const std::string LogPriority::ConsoleColorStringBegin(const Enum PRIORITY)
    {
#if defined(HEROESPATH_PLATFORM_DETECTED_IS_WINDOWS)
        const auto IGNORED { PRIORITY }; // this just prevents the Visual Studio warning
        return "";
#else
        switch (PRIORITY)
        {
            case Debug:
            {
                // cyan on black
                return "\033[36;40m";
            }
            case Warn:
            {
                // yellow on black
                return "\033[33;40m";
            }
            case Error:
            case Fatal:
            {
                // red on black
                return "\033[31;40m";
            }
            case Default:
            case Count:
            default:
            {
                return "";
            }
        }
#endif
    }

    const std::string LogPriority::ConsoleColorStringEnd()
    {
#if defined(HEROESPATH_PLATFORM_DETECTED_IS_WINDOWS)
        return "";
#else
        return "\033[0;0m";
#endif
    }

    const std::string LogPriority::MakeErrorString(
        const LogPriority::Enum PRIORITY, const std::string & FUNCTION_NAME)
    {
        std::ostringstream ss;

        ss << "misc::LogPriority::" << FUNCTION_NAME << "(" << unsigned(PRIORITY)
           << ") but that enum value is out of range (>=) with max=(Count-1)="
           << unsigned(Count - 1);

        return ss.str();
    }

} // namespace misc
} // namespace heroespath
