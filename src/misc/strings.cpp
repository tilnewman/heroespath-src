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
#include <iomanip>
#include <type_traits>

namespace heroespath
{
namespace misc
{

    bool IsUpper(const char CH) { return !((CH < 'A') || (CH > 'Z')); }
    bool IsLower(const char CH) { return !((CH < 'a') || (CH > 'z')); }

    void ToUpper(char & ch)
    {
        if (IsLower(ch))
        {
            ch -= 32;
        }
    }

    char ToUpperCopy(const char CH)
    {
        auto copy { CH };
        ToUpper(copy);
        return copy;
    }

    void ToUpper(std::string & str)
    {
        for (char & ch : str)
        {
            ToUpper(ch);
        }
    }

    const std::string ToUpperCopy(const std::string & STR)
    {
        auto copy { STR };
        ToUpper(copy);
        return copy;
    }

    void ToLower(char & ch)
    {
        if (IsUpper(ch))
        {
            ch += 32;
        }
    }

    char ToLowerCopy(const char CH)
    {
        auto copy { CH };
        ToLower(copy);
        return copy;
    }

    void ToLower(std::string & str)
    {
        for (char & ch : str)
        {
            ToLower(ch);
        }
    }

    const std::string ToLowerCopy(const std::string & STR)
    {
        auto copy { STR };
        ToLower(copy);
        return copy;
    }

    bool IsAlpha(const char CH) { return (IsUpper(CH) || IsLower(CH)); }

    bool IsDigit(const char CH) { return !((CH < '0') || (CH > '9')); }

    bool IsAlphaOrDigit(const char CH) { return (IsAlpha(CH) || IsDigit(CH)); }

    bool IsWhitespace(const char CH)
    {
        return ((CH == ' ') || (CH == '\t') || (CH == '\r') || (CH == '\n'));
    }

    bool IsDisplayable(const char CH) { return (((CH > 31) && (CH != 127)) || IsWhitespace(CH)); }

    bool IsWhitespaceOrNonDisplayable(const char CH) { return ((CH < 33) || (CH == 127)); }

    void TrimWhitespace(std::string & str)
    {
        TrimIf(str, [](const char CH) { return !IsWhitespace(CH); });
    }

    const std::string TrimWhitespaceCopy(const std::string & STR_ORIG)
    {
        std::string newStr { STR_ORIG };
        TrimWhitespace(newStr);
        return newStr;
    }

    void TrimNonDisplayable(std::string & str)
    {
        TrimIf(str, [](const char CH) { return IsDisplayable(CH); });
    }

    const std::string TrimNonDisplayableCopy(const std::string & STR_ORIG)
    {
        std::string newStr { STR_ORIG };
        TrimNonDisplayable(newStr);
        return newStr;
    }

    void TrimWhitespaceAndNonDisplayable(std::string & str)
    {
        TrimIf(str, [](const char CH) { return !IsWhitespaceOrNonDisplayable(CH); });
    }

    const std::string TrimWhitespaceAndNonDisplayableCopy(const std::string & STR_ORIG)
    {
        std::string newStr { STR_ORIG };
        TrimWhitespaceAndNonDisplayable(newStr);
        return newStr;
    }

    const std::string CamelTo(
        const std::string & STR_ORIG, const std::string & SEPARATOR, const CaseChange CASE_CHANGES)
    {
        char prevChar { 0 };
        std::string result;

        for (const char CURR_CHAR : STR_ORIG)
        {
            if (IsAlpha(prevChar) && IsAlpha(CURR_CHAR)
                && (IsUpper(prevChar) != IsUpper(CURR_CHAR)))
            {
                if ((CASE_CHANGES == CaseChange::Both)
                    || ((CASE_CHANGES == CaseChange::LowerToUpper) && IsLower(prevChar))
                    || ((CASE_CHANGES == CaseChange::UpperToLower) && IsUpper(prevChar)))
                {
                    result += SEPARATOR;
                }
            }

            result += CURR_CHAR;
            prevChar = CURR_CHAR;
        }

        return result;
    }

    const std::string Quoted(const std::string & STR) { return ToString(std::quoted(STR)); }

    const std::string MakeToStringPrefixWithTypename(
        const ToStringPrefix::Enum OPTIONS,
        const std::string & CONTAINER_NAME,
        const std::string & NAMESPACE_PREFIX_STR,
        const std::string & TYPE_NAME)
    {
        std::ostringstream ss;

        if (OPTIONS & ToStringPrefix::Namespace)
        {
            std::string namespacePrefixToUse { NAMESPACE_PREFIX_STR };

            if ((NAMESPACE_PREFIX_STR.size() > 2)
                && (NAMESPACE_PREFIX_STR[NAMESPACE_PREFIX_STR.size() - 1] != ':')
                && (NAMESPACE_PREFIX_STR[NAMESPACE_PREFIX_STR.size() - 2] != ':'))
            {
                namespacePrefixToUse += "::";
            }

            ss << namespacePrefixToUse;
        }

        if (OPTIONS & ToStringPrefix::SimpleName)
        {
            ss << CONTAINER_NAME;
        }

        ss << TYPE_NAME;

        return ss.str();
    }

    const std::vector<std::string> MakeNumberStrings(const std::string & STR)
    {
        if (STR.empty())
        {
            return {};
        }

        std::vector<std::string> numberStrings(1, "");
        numberStrings.reserve(10);

        for (const char CHAR : STR)
        {
            const auto IS_DIGIT { IsDigit(CHAR) };

            if ((numberStrings.back().empty() == false) && (IS_DIGIT == false))
            {
                numberStrings.emplace_back(std::string());
            }
            else if (IS_DIGIT)
            {
                numberStrings.back().push_back(CHAR);
            }
        }

        while ((numberStrings.empty() == false) && numberStrings.back().empty())
        {
            numberStrings.pop_back();
        }

        return numberStrings;
    }

    int FindFirstNumber(const std::string & STR, const int RETURN_ON_ERROR)
    {
        return FindNumber(0, STR, RETURN_ON_ERROR);
    }

    int FindLastNumber(const std::string & STR, const int RETURN_ON_ERROR)
    {
        return FindNumberLast(STR, RETURN_ON_ERROR);
    }

    const std::string string_helpers::NameEqualsValueStr(
        const std::string & NAME,
        const std::string & VALUE_STR,
        const bool WILL_WRAP,
        const std::string & PREFIX,
        const bool IS_STR_TYPE)
    {
        if (NAME.empty())
        {
            return "";
        }

        std::string finalStr { PREFIX };

        if (WILL_WRAP)
        {
            finalStr += "(";
        }

        finalStr += (NAME + '=');

        const bool ALREADY_STARTS_WITH_QUOTE { (
            (VALUE_STR.empty() == false) && (VALUE_STR.front() == '\"')) };

        const bool WILL_ADD_QUOTES { (IS_STR_TYPE && (ALREADY_STARTS_WITH_QUOTE == false)) };

        if (WILL_ADD_QUOTES)
        {
            finalStr += '\"';
        }

        finalStr += VALUE_STR;

        if (WILL_ADD_QUOTES)
        {
            finalStr += '\"';
        }

        if (WILL_WRAP)
        {
            finalStr += ")";
        }

        return finalStr;
    }

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

    const std::vector<std::string> SplitByChars(const std::string & TO_SPLIT, const SplitHow HOW)
    {
        if (TO_SPLIT.empty())
        {
            return {};
        }

        if (HOW.separator_chars.empty())
        {
            return { TO_SPLIT };
        }

        const bool IS_ONLY_ONE_SPLIT_CHAR { (HOW.separator_chars.size() == 1) };

        auto isSplitChar = [&](const char CHAR_TO_CHECK) {
            for (const char SPLIT_CHAR : HOW.separator_chars)
            {
                if (CHAR_TO_CHECK == SPLIT_CHAR)
                {
                    return true;
                }
            }

            return false;
        };

        std::vector<std::string> splitStrings;
        splitStrings.reserve(16); // found by experiment to be a good guess for the game

        auto appendCurrentString = [&](std::string & currentStrToAppend) {
            if (HOW.will_trim_each && !currentStrToAppend.empty())
            {
                TrimWhitespace(currentStrToAppend);
            }

            if (HOW.will_skip_empty && currentStrToAppend.empty())
            {
                return;
            }

            splitStrings.emplace_back(currentStrToAppend);
        };

        std::string currentStr;

        for (const char CH : TO_SPLIT)
        {
            if ((IS_ONLY_ONE_SPLIT_CHAR && (CH == HOW.separator_chars[0])) || isSplitChar(CH))
            {
                appendCurrentString(currentStr);
                currentStr.clear();
            }
            else
            {
                currentStr += CH;
            }
        }

        if (!currentStr.empty())
        {
            appendCurrentString(currentStr);
        }

        return splitStrings;
    }

} // namespace misc
} // namespace heroespath
