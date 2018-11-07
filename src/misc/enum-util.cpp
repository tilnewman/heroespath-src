// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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

#include "misc/boost-string-includes.hpp"
#include "misc/log.hpp"
#include "misc/strings.hpp"

#include <algorithm>
#include <exception>
#include <sstream>

namespace heroespath
{
namespace misc
{

    namespace enum_helpers
    {

        const std::string TrimAndMakeLowerCase(const std::string & STR)
        {
            return boost::algorithm::trim_copy(boost::algorithm::to_lower_copy(STR));
        }

        const std::vector<std::string> BitwiseEnumFromStringSplit(const std::string & STR_ORIG)
        {
            const auto STR_TRIMMED_AND_LOWERCASE { TrimAndMakeLowerCase(STR_ORIG) };

            if (STR_TRIMMED_AND_LOWERCASE.empty())
            {
                return {};
            }

            std::string seperatorChars;

            for (const auto CHAR : STR_TRIMMED_AND_LOWERCASE)
            {
                // spaces/dashes/colons are not valid separators because some names will have them
                if ((IsAlphaOrDigit(CHAR) == false) && (CHAR != ' ') && (CHAR != '-')
                    && (CHAR != ':'))
                {
                    seperatorChars.push_back(CHAR);
                }
            }

            return SplitByChars(STR_TRIMMED_AND_LOWERCASE, SplitHow(seperatorChars));
        }

        const std::string ValueOutOfRangeErrorString(
            const EnumUnderlying_t ENUM_VALUE,
            const EnumUnderlying_t LARGEST_VALID_VALUE,
            const EnumUnderlying_t COUNT_VALUE)
        {
            std::ostringstream ss;

            ss << "(enum_value_out_of_range=" << ENUM_VALUE
               << "_largest_valid=" << LARGEST_VALID_VALUE << ", count=" << COUNT_VALUE << ")";

            return ss.str();
        }

        EnumUnderlying_t ReturnValueOfStringOr(
            const std::map<std::string, EnumUnderlying_t> & STRING_TO_VALUE_MAP,
            const std::string & STRING_TO_FIND,
            const EnumUnderlying_t TO_RETURN_IF_NOT_FOUND)
        {
            const auto FOUND_ITER { std::find_if(
                std::begin(STRING_TO_VALUE_MAP),
                std::end(STRING_TO_VALUE_MAP),
                [&](const auto & NAME_VALUE_PAIR) {
                    return (STRING_TO_FIND == NAME_VALUE_PAIR.first);
                }) };

            if (FOUND_ITER == std::end(STRING_TO_VALUE_MAP))
            {
                return TO_RETURN_IF_NOT_FOUND;
            }
            else
            {
                return FOUND_ITER->second;
            }
        }

        EnumUnderlying_t WordsToBitFlags(
            const std::vector<std::string> & WORDS_TO_FIND,
            const std::map<std::string, EnumUnderlying_t> & WORDS_TO_BIT_FLAG_MAP)
        {
            EnumUnderlying_t result { 0 };

            for (const auto & WORD_TO_FIND : WORDS_TO_FIND)
            {
                const auto FOUND_ITER { std::find_if(
                    std::begin(WORDS_TO_BIT_FLAG_MAP),
                    std::end(WORDS_TO_BIT_FLAG_MAP),
                    [&](const auto & NAME_VALUE_PAIR) {
                        return (WORD_TO_FIND == NAME_VALUE_PAIR.first);
                    }) };

                if (FOUND_ITER != std::end(WORDS_TO_BIT_FLAG_MAP))
                {
                    result |= FOUND_ITER->second;
                }
            }

            return result;
        }

    } // namespace enum_helpers
} // namespace misc
} // namespace heroespath
