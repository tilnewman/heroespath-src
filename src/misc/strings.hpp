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
#include "misc/log-macros.hpp"
#include "misc/to-string-prefix-enum.hpp"
#include "misc/type-helpers.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/type_index.hpp>

#include <algorithm>
#include <exception>
#include <functional>
#include <iomanip>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

namespace heroespath
{
namespace misc
{

    inline bool IsUpper(const char CHAR) { return ((CHAR >= 'A') && (CHAR <= 'Z')); }
    inline bool IsLower(const char CHAR) { return ((CHAR >= 'a') && (CHAR <= 'z')); }

    // if !IsLower() then ch is not changed
    inline void ToUpper(char & ch)
    {
        if (IsLower(ch))
        {
            ch -= 32;
        }
    }

    // if !IsLower() then returns the given CHAR unchanged
    inline char ToUpperCopy(const char CHAR)
    {
        auto copy { CHAR };
        ToUpper(copy);
        return copy;
    }

    inline void ToUpper(std::string & str)
    {
        for (char & ch : str)
        {
            ToUpper(ch);
        }
    }

    inline const std::string ToUpperCopy(const std::string & STRING)
    {
        auto copy { STRING };
        ToUpper(copy);
        return copy;
    }

    // if !IsUpper() then ch is not changed
    inline void ToLower(char & ch)
    {
        if (IsUpper(ch))
        {
            ch += 32;
        }
    }

    // if !IsUpper() then returns the given CHAR unchanged
    inline char ToLowerCopy(const char CHAR)
    {
        auto copy { CHAR };
        ToLower(copy);
        return copy;
    }

    inline void ToLower(std::string & str)
    {
        for (char & ch : str)
        {
            ToLower(ch);
        }
    }

    inline const std::string ToLowerCopy(const std::string & STRING)
    {
        auto copy { STRING };
        ToLower(copy);
        return copy;
    }

    inline bool IsAlpha(const char CHAR) { return (IsUpper(CHAR) || IsLower(CHAR)); }

    inline bool IsDigit(const char CHAR) { return ((CHAR >= '0') && (CHAR <= '9')); }

    inline bool IsAlphaOrDigit(const char CHAR) { return (IsAlpha(CHAR) || IsDigit(CHAR)); }

    enum class CaseChange
    {
        LowerToUpper,
        UpperToLower,
        Both
    };

    const std::string CamelTo(
        const std::string & STRING,
        const std::string & SEPARATOR,
        const CaseChange CASE_CHANGES = CaseChange::Both);

    inline const std::string CamelTo(
        const std::string & STRING,
        const char SEPARATOR,
        const CaseChange CASE_CHANGES = CaseChange::Both)
    {
        return CamelTo(STRING, std::string(1, SEPARATOR), CASE_CHANGES);
    }

    template <typename T>
    const std::string ToString(const T & THING)
    {
        std::ostringstream ss;

        try
        {
            ss << std::boolalpha << THING;
        }
        catch (const std::exception & EXCEPTION)
        {
            M_HP_LOG_WRN(
                "misc::ToString<" << boost::typeindex::type_id<T>().pretty_name()
                                  << ">() threw std::exception=\"" << EXCEPTION.what()
                                  << "\".  Returning an empty string "
                                     "and continuing....");
        }
        catch (...)
        {
            M_HP_LOG_WRN(
                "misc::ToString<" << boost::typeindex::type_id<T>().pretty_name()
                                  << ">() threw an unknown exception.  Returning an empty string "
                                     "and continuing....");
        }

        return ss.str();
    }

    // empty strings always return RETURN_ON_ERROR
    template <typename T, typename = std::enable_if_t<is_number_v<T>>>
    T ToNumber(const std::string & STRING, const T RETURN_ON_ERROR)
    {
        if (STRING.empty())
        {
            return RETURN_ON_ERROR;
        }

        try
        {
            return boost::lexical_cast<T>(STRING);
        }
        catch (...)
        {
            return RETURN_ON_ERROR;
        }
    }

    inline const std::string Quoted(const std::string & STRING)
    {
        return ToString(std::quoted(STRING));
    }

    template <typename T = void>
    const std::string MakeToStringPrefix(
        const ToStringPrefix::Enum OPTIONS,
        const std::string & CONTAINER_NAME,
        const std::string & NAMESPACE_PREFIX_STR = "")
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

        if constexpr (std::is_same<T, void>::value == false)
        {
            if (OPTIONS & ToStringPrefix::Typename)
            {
                ss << "<" << boost::typeindex::type_id<T>().pretty_name() << ">";
            }
        }

        return ss.str();
    }

    template <typename T = int, typename = std::enable_if_t<is_number_non_floating_point_v<T>>>
    const std::string NumberToStringWithOrdinalSuffix(const T NUMBER)
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

    const std::string MakeLoggableString(const std::string &);

    // empty input always returns false, if the given vector is empty then returns false, any
    // STRINGS_TO_FIND that are empty will be ignored, any STRINGS_TO_FIND that are longer than
    // STRING_TO_SEARCH will be ignored, if all are empty/ignored then returns false
    bool ContainsAnyOf(
        const std::string & STRING_TO_SEARCH,
        const std::vector<std::string> & STRINGS_TO_FIND,
        const bool IS_CASE_SENSITIVE);

    // returns the n'th (zero indexed) (non-floating-point) (positive) number found in the string or
    // RETURN_ON_ERROR, if the string is empty or contains no digits then RETURN_ON_ERROR is
    // returned, if NTH_NUMBER is >= to how many numbers are int the string then RETURN_ON_ERROR
    // is returned, if the number at NTH_NUMBER is too large for type T then RETURN_ON_ERROR is
    // returned, for example, if the string provided is:
    //  "The numbers are 0.1, and inside the filename log-234.txt"
    //      Find(0, "...", -1)   returns 0
    //      Find(1, "...", -1)   returns 1
    //      Find(2, "...", -1)   returns 234
    //      Find(3, "...", -1)   returns -1
    template <typename T = int, typename = std::enable_if_t<is_number_non_floating_point_v<T>>>
    T FindNumber(const std::size_t NTH_NUMBER, const std::string & STRING, const T RETURN_ON_ERROR)
    {
        if (STRING.empty())
        {
            return RETURN_ON_ERROR;
        }

        auto isNumber = [](const char CHAR) { return ((CHAR >= '0') && (CHAR <= '9')); };

        std::vector<std::string> numberStrings(1, "");
        numberStrings.reserve(10);

        for (const char CHAR : STRING)
        {
            const auto IS_DIGIT { isNumber(CHAR) };

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

        if (numberStrings.empty())
        {
            return RETURN_ON_ERROR;
        }

        if (NTH_NUMBER >= numberStrings.size())
        {
            return RETURN_ON_ERROR;
        }

        return ToNumber<T>(numberStrings.at(NTH_NUMBER), RETURN_ON_ERROR);
    }

    // handy version of the FindNumber() function above, see comments for FindNumber()
    template <typename T = int, typename = std::enable_if_t<is_number_non_floating_point_v<T>>>
    T FindNumberLast(const std::string & STRING, const T RETURN_ON_ERROR)
    {
        if (STRING.empty())
        {
            return RETURN_ON_ERROR;
        }

        auto isNumber = [](const char CHAR) { return ((CHAR >= '0') && (CHAR <= '9')); };

        std::vector<std::string> numberStrings(1, "");
        numberStrings.reserve(10);

        for (const char CHAR : STRING)
        {
            const auto IS_DIGIT { isNumber(CHAR) };

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

        if (numberStrings.empty())
        {
            return RETURN_ON_ERROR;
        }
        else
        {
            return ToNumber<T>(numberStrings.back(), RETURN_ON_ERROR);
        }
    }

    inline int FindFirstNumber(const std::string & STRING, const int RETURN_ON_ERROR = -1)
    {
        return FindNumber(0, STRING, RETURN_ON_ERROR);
    }

    inline int FindLastNumber(const std::string & STRING, const int RETURN_ON_ERROR = -1)
    {
        return FindNumberLast(STRING, RETURN_ON_ERROR);
    }

    template <typename T>
    const std::string NameEqualsValueStr(
        const std::string & NAME,
        const T VALUE,
        const bool WILL_WRAP = false,
        const std::string & PREFIX = "")
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

        const bool IS_STRING_TYPE { (
            std::is_same<std::remove_const_t<T>, char *>::value
            || std::is_same<std::remove_const_t<T>, std::string>::value) };

        const auto VALUE_STR { ToString(VALUE) };

        const bool ALREADY_STARTS_WITH_QUOTE { (
            (VALUE_STR.empty() == false) && (VALUE_STR.front() == '\"')) };

        const bool WILL_ADD_QUOTES { (IS_STRING_TYPE && (ALREADY_STARTS_WITH_QUOTE == false)) };

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

    struct SplitHow
    {
        explicit SplitHow(
            const std::string SEPARATOR_CHARS = ",",
            const bool WILL_SKIP_EMPTY = true,
            const bool WILL_TRIM_EACH = true)
            : separator_chars(SEPARATOR_CHARS)
            , will_skip_empty(WILL_SKIP_EMPTY)
            , will_trim_each(WILL_TRIM_EACH)
        {}

        explicit SplitHow(
            const char SEPARATOR_CHAR,
            const bool WILL_SKIP_EMPTY = true,
            const bool WILL_TRIM_EACH = true)
            : separator_chars(1, SEPARATOR_CHAR)
            , will_skip_empty(WILL_SKIP_EMPTY)
            , will_trim_each(WILL_TRIM_EACH)
        {}

        SplitHow(const SplitHow &) = default;
        SplitHow(SplitHow &&) = default;
        SplitHow & operator=(const SplitHow &) = default;
        SplitHow & operator=(SplitHow &&) = default;

        std::string separator_chars;
        bool will_skip_empty;
        bool will_trim_each;
    };

    // TO_SPLIT is split into pieces by any of the chars in HOW.separator_chars and NOT by that
    // string as a whole.  The resulting pieces will NOT contain any of the separator_chars. If
    // trimming makes a piece empty then HOW.will_skip_empty applies. If HOW.separator_chars is
    // empty then TO_SPLIT is the only string returned.
    const std::vector<std::string>
        SplitByChars(const std::string & TO_SPLIT, const SplitHow HOW = SplitHow());

} // namespace misc
} // namespace heroespath

#define M_HP_VAR_STR(var) heroespath::misc::NameEqualsValueStr(#var, var, true)

#endif // HEROESPATH_MISC_STRINGS_HPP_INCLUDED
