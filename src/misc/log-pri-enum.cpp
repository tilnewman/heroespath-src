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

    const std::string LogPriority::ToString(const Enum PRIORITY)
    {
        switch (PRIORITY)
        {
            case Debug: return "Debug";
            case Default: return "Default";
            case Warn: return "Warn";
            case Error: return "Error";
            case Fatal: return "Fatal";
            case Count: return "(Count)";
            default:
            {
                // can't use log or assert macros inside the logging code
                std::ostringstream ss;

                ss << __FILE__ << ":" << __func__ << "():" << __LINE__
                   << "enum_value=" << static_cast<EnumUnderlying_t>(PRIORITY)
                   << " is invalid. (count=" << static_cast<EnumUnderlying_t>(Count) << ")";

                std::cerr << ss.str() << std::endl;

                return "";
            }
        }
    }

    const std::string LogPriority::ToStringAcronym(const Enum PRIORITY)
    {
        switch (PRIORITY)
        {
            case Debug: return "DBG";
            case Default: return "DEF";
            case Warn: return "WRN";
            case Error: return "ERR";
            case Fatal: return "FAT";
            case Count: return "(Count)";
            default:
            {
                // can't use log or assert macros inside the logging code
                std::ostringstream ss;

                ss << __FILE__ << ":" << __func__ << "():" << __LINE__
                   << "enum_value=" << static_cast<EnumUnderlying_t>(PRIORITY)
                   << " is invalid. (count=" << static_cast<EnumUnderlying_t>(Count) << ")";

                std::cerr << ss.str() << std::endl;

                return "";
            }
        }
    }

    const std::string LogPriority::ConsoleColorStringBegin(const Enum PRIORITY)
    {
        if constexpr (misc::platform_is_any_windows)
        {
            // this just prevents a Visual Studio warning that PRIORITY is not used
            const auto IGNORED { PRIORITY };

            return "";
        }
        else
        {
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
                case Count: { return "(Count)";
                }
                default: { return "";
                }
            }
        }
    }

    const std::string LogPriority::ConsoleColorStringEnd()
    {
        if constexpr (misc::platform_is_any_windows)
        {
            return "";
        }
        else
        {
            return "\033[0;0m";
        }
    }

} // namespace misc
} // namespace heroespath
