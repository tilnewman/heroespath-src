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

namespace heroespath
{

namespace enum_helpers
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

    EnumUnderlying_t ReturnValueOfStringOr(
        const misc::VectorMap<std::string, EnumUnderlying_t> & STRING_TO_VALUE_MAP,
        const std::string & STRING_TO_FIND,
        const EnumUnderlying_t TO_RETURN_IF_NOT_FOUND)
    {
        EnumUnderlying_t result { TO_RETURN_IF_NOT_FOUND };
        STRING_TO_VALUE_MAP.Find(STRING_TO_FIND, result);
        return result;
    }

    EnumUnderlying_t WordsToBitFlags(
        const std::vector<std::string> & WORDS_TO_FIND,
        const misc::VectorMap<std::string, EnumUnderlying_t> & WORDS_TO_BIT_FLAG_MAP)
    {
        EnumUnderlying_t result { 0 };

        for (const auto & WORD_TO_FIND : WORDS_TO_FIND)
        {
            EnumUnderlying_t bitFlag { 0 };
            WORDS_TO_BIT_FLAG_MAP.Find(WORD_TO_FIND, bitFlag);
            result |= bitFlag;
        }

        return result;
    }

} // namespace enum_helpers
} // namespace heroespath
