// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// strings.cpp
//
#include "strings.hpp"

#include "misc/boost-string-includes.hpp"

namespace heroespath
{
namespace misc
{

    const std::string MakeLoggableString(const std::string & ORIG_STR)
    {
        std::string newString(ORIG_STR);

        boost::algorithm::replace_all(newString, "\n", "\\n");

        const std::size_t MAX_SIZE { 40 };
        if (newString.size() > MAX_SIZE)
        {
            newString.resize(MAX_SIZE - 3);
            newString += "...";
        }

        return newString;
    }

} // namespace misc
} // namespace heroespath
