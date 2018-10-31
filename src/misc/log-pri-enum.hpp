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
#include "misc/enum-util.hpp"

#include <string>

namespace heroespath
{
namespace misc
{

    struct LogPriority : public misc::EnumBaseCounting<LogPriority, misc::EnumFirstValueValid>
    {
        enum Enum : misc::EnumUnderlying_t
        {
            Debug = 0,
            Default,
            Warn,
            Error,
            Fatal,
            Count,
        };

        static const std::string ToString(const Enum PRIORITY);
        static const std::string ToStringAcronym(const Enum PRIORITY);
        static const std::string ConsoleColorStringBegin(const Enum PRIORITY);
        static const std::string ConsoleColorStringEnd();
    };

} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_LOG_PRI_ENUM_HPP_INCLUDED
