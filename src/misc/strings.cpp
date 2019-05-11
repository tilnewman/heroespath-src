// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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

#include <algorithm>

namespace heroespath
{
namespace misc
{

    const std::string CamelTo(
        const std::string_view STR, const std::string_view SEPARATOR, const CaseChange CASE_CHANGES)
    {
        std::string result;

        if (STR.size() <= 1)
        {
            result = STR;
            return result;
        }

        result.reserve(STR.size() * 2);

        auto iter = std::begin(STR);
        const auto endIter = std::end(STR);

        while (iter != endIter)
        {
            const char PREV = *(iter++);
            result += PREV;

            if (iter == endIter)
            {
                break;
            }

            const char CURR = *iter;

            if (IsAlpha(PREV) && IsAlpha(CURR))
            {
                const auto IS_PREV_UPPER = IsUpper(PREV);
                const auto IS_CURR_UPPER = IsUpper(CURR);

                if (IS_PREV_UPPER != IS_CURR_UPPER)
                {
                    if ((CASE_CHANGES == CaseChange::Both)
                        || ((CASE_CHANGES == CaseChange::LowerToUpper) && !IS_PREV_UPPER)
                        || ((CASE_CHANGES == CaseChange::UpperToLower) && IS_PREV_UPPER))
                    {
                        result += SEPARATOR;
                    }
                }
            }
        }

        return result;
    }

    const std::string MakeLoggableString(
        const std::string_view ORIG_STR, const std::size_t MAX_SIZE, const bool WILL_ADD_ELIPSIS)
    {
        std::string newString { ORIG_STR };

        boost::algorithm::replace_all(newString, "\n", "\\n");

        if (newString.size() > MAX_SIZE)
        {
            newString.resize(MAX_SIZE);

            if (WILL_ADD_ELIPSIS && (newString.size() > 3))
            {
                newString[newString.size() - 1] = '.';
                newString[newString.size() - 2] = '.';
                newString[newString.size() - 3] = '.';
            }
        }

        return newString;
    }

    bool ContainsAnyOf(
        const std::string_view STR_TO_SEARCH,
        const std::vector<std::string> & STRS_TO_FIND,
        const bool IS_CASE_SENSITIVE)
    {
        if (STR_TO_SEARCH.empty() || STRS_TO_FIND.empty())
        {
            return false;
        }

        namespace ba = boost::algorithm;

        for (const auto & STR_TO_FIND : STRS_TO_FIND)
        {
            if (STR_TO_FIND.empty() || (STR_TO_FIND.size() > STR_TO_SEARCH.size()))
            {
                continue;
            }

            if (IS_CASE_SENSITIVE)
            {
                if (ba::contains(STR_TO_SEARCH, STR_TO_FIND))
                {
                    return true;
                }
            }
            else
            {
                if (ba::icontains(STR_TO_SEARCH, STR_TO_FIND))
                {
                    return true;
                }
            }
        }

        return false;
    }

    int FindNumber(const int NTH_NUMBER, const std::string_view STR)
    {
        const int ERROR_RESULT(-1);
        int result(ERROR_RESULT);

        if (STR.empty())
        {
            return result;
        }

        auto notDigit = [](const char CH) constexpr noexcept { return !IsDigit(CH); };

        std::vector<std::string> numberStrings;

        auto storeString = [&](const auto FIRST, const auto LAST) {
            numberStrings.emplace_back(FIRST, LAST);
            return false;
        };

        FindStringsBetweenSeparatorsAndCall(std::begin(STR), std::end(STR), notDigit, storeString);

        if (!numberStrings.empty())
        {
            if (NTH_NUMBER < 0)
            {
                result = ToNumberOr(numberStrings.back(), ERROR_RESULT);
            }
            else if (static_cast<std::size_t>(NTH_NUMBER) < numberStrings.size())
            {
                result
                    = ToNumberOr(numberStrings[static_cast<std::size_t>(NTH_NUMBER)], ERROR_RESULT);
            }
        }

        return result;
    }

    const std::vector<std::string> SplitByChars(const std::string_view TO_SPLIT, const SplitHow HOW)
    {
        std::vector<std::string> results;

        if (TO_SPLIT.empty())
        {
            return results;
        }

        if (HOW.separator_chars.empty())
        {
            results = { std::string(TO_SPLIT) };
            return results;
        }

        auto storeString = [&](const auto FIRST, const auto LAST) {
            std::string str(FIRST, LAST);

            if (!(HOW.options & SplitOpt::NoTrim))
            {
                TrimWhitespace(str);
            }

            if (!(HOW.options & SplitOpt::SkipEmpty) || !str.empty())
            {
                results.emplace_back(str);
            }

            return false;
        };

        auto isSeparator = [&HOW](const char CH) {
            for (const auto SEP_CHAR : HOW.separator_chars)
            {
                if (CH == SEP_CHAR)
                {
                    return true;
                }
            }

            return false;
        };

        FindStringsBetweenSeparatorsAndCall(
            std::begin(TO_SPLIT), std::end(TO_SPLIT), isSeparator, storeString);

        return results;
    }

} // namespace misc
} // namespace heroespath
