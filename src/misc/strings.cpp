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

    bool AreEqualCaseInsensitive(const std::string & A, const std::string & B)
    {
        if (A.size() != B.size())
        {
            return false;
        }

        auto iterA = std::begin(A);
        auto iterB = std::begin(B);
        const auto END_A = std::end(A);

        while (iterA != END_A)
        {
            const char CHAR_A = *(iterA++);
            const char CHAR_B = *(iterB++);

            if (CHAR_A != CHAR_B)
            {
                if (IsUpper(CHAR_A))
                {
                    if (!IsLower(CHAR_B))
                    {
                        return false;
                    }
                }
                else if (IsLower(CHAR_A))
                {
                    if (!IsUpper(CHAR_B))
                    {
                        return false;
                    }
                }
            }
        }

        return true;
    }

    namespace helpers
    {
        const std::tuple<bool, Str2NumInteger_t> ToNumber_ParseInteger(
            std::string::const_iterator iter, const std::string::const_iterator END)
        {
            Str2NumInteger_t integer = 0;

            while (iter != END)
            {
                const auto PREV = integer;

                integer *= 10;
                integer += static_cast<Str2NumInteger_t>(*(iter++) - '0');

                if (integer < PREV)
                {
                    return { false, 0 };
                }
            }

            return { true, integer };
        }

        const std::tuple<bool, Str2NumReal_t> ToNumber_ParseFraction(
            std::string::const_iterator iter, const std::string::const_iterator END)
        {
            Str2NumInteger_t numerator = 0;
            Str2NumInteger_t denominator = 1;

            while (iter != END)
            {
                const auto PREV = numerator;

                numerator *= 10;
                numerator += static_cast<Str2NumInteger_t>(*(iter++) - '0');

                if (numerator < PREV)
                {
                    return { false, Str2NumReal_t(0) };
                }

                denominator *= 10;
            }

            return { true,
                     static_cast<Str2NumReal_t>(
                         static_cast<long double>(numerator)
                         / static_cast<long double>(denominator)) };
        }

        // returns iters such that there are only isDigit() between begin and end
        const PreParseResult ToNumber_PreParse(
            const std::string::const_iterator FIRST, const std::string::const_iterator LAST)
        {
            PreParseResult parse(LAST);

            if (FIRST == LAST)
            {
                return parse;
            }

            // skip past leading whitespaces or '+'
            parse.integer_begin_iter = std::find_if_not(FIRST, LAST, IsWhitespaceOrPlus);

            // is all whitespace or plus signs
            if (parse.integer_begin_iter == LAST)
            {
                return parse;
            }

            if (*parse.integer_begin_iter == '-')
            {
                parse.is_negative = true;
                ++parse.integer_begin_iter;
            }

            parse.integer_end_iter = std::find_if_not(parse.integer_begin_iter, LAST, IsDigit);

            // integer digits continue until the end of STR
            if (parse.integer_end_iter == LAST)
            {
                // this is fine as long as there were some integer digits
                parse.success = (parse.integer_begin_iter != parse.integer_end_iter);
                return parse;
            }

            const auto HAS_INTEGER_DIGITS { (parse.integer_begin_iter != parse.integer_end_iter) };

            // integer digits end with whitespace
            if (IsWhitespace(*parse.integer_end_iter))
            {
                // this is fine if: there were some integer digits AND only whitespace remains
                parse.success
                    = (HAS_INTEGER_DIGITS
                       && (std::find_if_not((parse.integer_end_iter + 1), LAST, IsWhitespace)
                           == LAST));

                return parse;
            }

            // integer digits do not extend to the end of the STR, do not end in whitespace, or
            // period
            if (*parse.integer_end_iter != '.')
            {
                return parse;
            }

            parse.fraction_begin_iter = (parse.integer_end_iter + 1);

            parse.fraction_end_iter = std::find_if_not(parse.fraction_begin_iter, LAST, IsDigit);

            const auto HAS_FRACTION_DIGITS { (
                parse.fraction_begin_iter != parse.fraction_end_iter) };

            // always fail if there were no fraction or integer digits
            if (!HAS_FRACTION_DIGITS && !HAS_INTEGER_DIGITS)
            {
                return parse;
            }

            // always fail if there is anything other than whitespace after the fraction digits
            if (parse.fraction_end_iter != LAST)
            {
                if (std::find_if_not(parse.fraction_end_iter, LAST, IsWhitespace) != LAST)
                {
                    return parse;
                }
            }

            // Move the fraction_end_iter backward until all trailing zeros are eliminated.
            // Testing revealed it is faster to eliminate trailing zeros here than it is to
            // handle them in ToNumber_Custom_ParseFraction().
            {
                auto revIter = std::string::const_reverse_iterator(parse.fraction_end_iter);

                if (*revIter == '0')
                {
                    parse.fraction_end_iter
                        = std::find_if(
                              revIter,
                              std::string::const_reverse_iterator(parse.fraction_begin_iter),
                              [](const char C) { return (C != '0'); })
                              .base();
                }
            }

            parse.success = true;
            return parse;
        }

    } // namespace helpers

    const std::string CamelTo(
        const std::string & STR_ORIG, const std::string & SEPARATOR, const CaseChange CASE_CHANGES)
    {
        char prevChar { 0 };
        std::string result;
        result.reserve(STR_ORIG.size() * 2);

        for (const char CURR_CHAR : STR_ORIG)
        {
            if (IsAlpha(prevChar) && IsAlpha(CURR_CHAR))
            {
                const auto IS_PREV_UPPER = IsUpper(prevChar);
                const auto IS_CURR_UPPER = IsUpper(CURR_CHAR);

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

            result += CURR_CHAR;
            prevChar = CURR_CHAR;
        }

        return result;
    }

    const std::string MakeLoggableString(
        const std::string & ORIG_STR, const std::size_t MAX_SIZE, const bool WILL_ADD_ELIPSIS)
    {
        auto newString { ORIG_STR };

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
        const std::string & STR_TO_SEARCH,
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

    int FindNumber(const int NTH_NUMBER, const std::string & STR)
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

    const std::vector<std::string> SplitByChars(const std::string & TO_SPLIT, const SplitHow HOW)
    {
        std::vector<std::string> results;

        if (TO_SPLIT.empty())
        {
            return results;
        }

        if (HOW.separator_chars.empty())
        {
            results = { TO_SPLIT };
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
