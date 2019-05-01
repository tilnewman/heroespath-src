// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// enum-util.cpp
//
#include "enum-util.hpp"

#include "misc/strings.hpp"

#include <algorithm>

namespace heroespath
{

namespace helpers
{

    const std::string MakeLowerCaseCopy(const std::string & STR) { return misc::ToLowerCopy(STR); }

    const std::vector<std::string> BitwiseEnumFromStringSplit(const std::string & STR_ORIG)
    {
        const auto STR_LOWERCASE { MakeLowerCaseCopy(STR_ORIG) };

        if (STR_LOWERCASE.empty())
        {
            return {};
        }

        std::string seperatorChars;

        for (const auto CHAR : STR_LOWERCASE)
        {
            // spaces/dashes/colons are not valid separators because some names will have them
            if ((misc::IsAlphaOrDigit(CHAR) == false) && (CHAR != ' ') && (CHAR != '-')
                && (CHAR != ':'))
            {
                seperatorChars.push_back(CHAR);
            }
        }

        return misc::SplitByChars(STR_LOWERCASE, misc::SplitHow(seperatorChars));
    }

    void sortStrNumPairVec(StrNumPairVec_t & v) { std::sort(std::begin(v), std::end(v)); }

} // namespace helpers
} // namespace heroespath
