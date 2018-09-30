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
#include <string>

namespace heroespath
{
namespace misc
{

    // Responsible for enumerating all possible log priorities
    //
    // NOTE:  This does not use the helper stuff in enum-util.hpp because it can't, because the code
    // in that file uses the logger!
    //
    struct LogPriority
    {
        enum Enum : unsigned int
        {
            Debug = 0,
            Default,
            Warn,
            Error,
            Fatal,
            Count,
        };

        static bool IsValid(const unsigned int ENUM_VALUE);

        static const std::string ToString(const Enum PRIORITY);

        static const std::string ToStringAcronym(const Enum PRIORITY);

        static const std::string ConsoleColorStringBegin(const Enum PRIORITY);

        static const std::string ConsoleColorStringEnd();

    private:
        static const std::string
            MakeErrorString(const Enum PRIORITY, const std::string & FUNCTION_NAME);
    };

} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_LOG_PRI_ENUM_HPP_INCLUDED
