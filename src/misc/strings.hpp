// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_MISC_STRINGS_HPP_INCLUDED
#define HEROESPATH_MISC_STRINGS_HPP_INCLUDED
//
// strings.hpp
//
#include <sstream>
#include <string>

namespace heroespath
{
namespace misc
{

    struct String
    {

        template <typename T>
        static const std::string DecorateNumber(const T NUMBER)
        {
            std::ostringstream ss;
            ss << NUMBER;

            if (NUMBER == 1)
            {
                ss << "st";
            }
            else if (NUMBER == 2)
            {
                ss << "nd";
            }
            else if (NUMBER == 3)
            {
                ss << "rd";
            }
            else
            {
                ss << "th";
            }

            return ss.str();
        }
    };
} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_STRINGS_HPP_INCLUDED
