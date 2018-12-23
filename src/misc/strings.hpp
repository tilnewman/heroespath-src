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

#include <sstream>
#include <string>
#include <vector>

namespace heroespath
{
namespace misc
{

    bool IsUpper(const char CH);
    bool IsLower(const char CH);

    void ToUpper(char & ch);
    char ToUpperCopy(const char CH);
    void ToUpper(std::string & str);
    const std::string ToUpperCopy(const std::string & STR);

    void ToLower(char & ch);
    char ToLowerCopy(const char CH);
    void ToLower(std::string & str);
    const std::string ToLowerCopy(const std::string & STR);

    bool IsAlpha(const char CH);
    bool IsDigit(const char CH);
    bool IsAlphaOrDigit(const char CH);
    bool IsWhitespace(const char CH);
    bool IsDisplayable(const char CH);
    bool IsWhitespaceOrNonDisplayable(const char CH);

    // trims any char(s) for which the lambda returns false
    template <typename Lambda_t>
    void TrimIf(std::string & str, Lambda_t lambda)
    {
        str.erase(std::begin(str), std::find_if(std::begin(str), std::end(str), lambda));
        str.erase(std::find_if(std::rbegin(str), std::rend(str), lambda).base(), std::end(str));
    }

    void TrimWhitespace(std::string & str);
    const std::string TrimWhitespaceCopy(const std::string & STR);

    void TrimNonDisplayable(std::string & str);
    const std::string TrimNonDisplayableCopy(const std::string & STR);

    void TrimWhitespaceAndNonDisplayable(std::string & str);
    const std::string TrimWhitespaceAndNonDisplayableCopy(const std::string & STR);

    enum class CaseChange
    {
        LowerToUpper,
        UpperToLower,
        Both
    };

    const std::string CamelTo(
        const std::string & STR,
        const std::string & SEPARATOR,
        const CaseChange CASE_CHANGES = CaseChange::Both);

    inline const std::string CamelTo(
        const std::string & STR,
        const char SEPARATOR,
        const CaseChange CASE_CHANGES = CaseChange::Both)
    {
        return CamelTo(STR, std::string(1, SEPARATOR), CASE_CHANGES);
    }

    template <typename T>
    const std::string ToString(const T & THING)
    {
        std::ostringstream ss;

        try
        {
            ss << std::boolalpha << THING;
            return ss.str();
        }
        catch (const std::exception & EXCEPTION)
        {
            M_HP_LOG_WRN(
                "misc::ToString<" << boost::typeindex::type_id<T>().pretty_name()
                                  << ">() threw std::exception=\"" << EXCEPTION.what()
                                  << "\".  Returning an empty string "
                                     "and continuing....");

            return "";
        }
        catch (...)
        {
            M_HP_LOG_WRN(
                "misc::ToString<" << boost::typeindex::type_id<T>().pretty_name()
                                  << ">() threw an unknown exception.  Returning an empty string "
                                     "and continuing....");

            return "";
        }
    }

    // empty strings always return RETURN_ON_ERROR
    template <typename T, typename = std::enable_if_t<is_number_v<T>>>
    T ToNumber(const std::string & STR, const T RETURN_ON_ERROR)
    {
        if (STR.empty())
        {
            return RETURN_ON_ERROR;
        }

        try
        {
            return boost::lexical_cast<T>(STR);
        }
        catch (...)
        {
            return RETURN_ON_ERROR;
        }
    }

    const std::string Quoted(const std::string & STR);

    const std::string MakeToStringPrefixWithTypename(
        const ToStringPrefix::Enum OPTIONS,
        const std::string & CONTAINER_NAME,
        const std::string & NAMESPACE_PREFIX_STR,
        const std::string & TYPE_NAME);

    template <typename T = void>
    const std::string MakeToStringPrefix(
        const ToStringPrefix::Enum OPTIONS,
        const std::string & CONTAINER_NAME,
        const std::string & NAMESPACE_PREFIX_STR = "")
    {
        std::string typeName;
        if constexpr (std::is_same<T, void>::value == false)
        {
            if (OPTIONS & ToStringPrefix::Typename)
            {
                typeName = std::string("<") + boost::typeindex::type_id<T>().pretty_name() + ">";
            }
        }

        return MakeToStringPrefixWithTypename(
            OPTIONS, CONTAINER_NAME, NAMESPACE_PREFIX_STR, typeName);
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

    const std::string MakeLoggableString(
        const std::string &, const std::size_t MAX_SIZE = 40, const bool WILL_ADD_ELIPSIS = true);

    // empty input always returns false, if the given vector is empty then returns false, any
    // STRS_TO_FIND that are empty will be ignored, any STRS_TO_FIND that are longer than
    // STR_TO_SEARCH will be ignored, if all are empty/ignored then returns false
    bool ContainsAnyOf(
        const std::string & STR_TO_SEARCH,
        const std::vector<std::string> & STRS_TO_FIND,
        const bool IS_CASE_SENSITIVE);

    const std::vector<std::string> MakeNumberStrings(const std::string & STR);

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
    T FindNumber(const std::size_t NTH_NUMBER, const std::string & STR, const T RETURN_ON_ERROR)
    {
        const auto NUMBER_STR_VEC { MakeNumberStrings(STR) };

        if (NUMBER_STR_VEC.empty())
        {
            return RETURN_ON_ERROR;
        }

        if (NTH_NUMBER >= NUMBER_STR_VEC.size())
        {
            return RETURN_ON_ERROR;
        }

        return ToNumber<T>(NUMBER_STR_VEC.at(NTH_NUMBER), RETURN_ON_ERROR);
    }

    // handy version of the FindNumber() function above, see comments for FindNumber()
    template <typename T = int, typename = std::enable_if_t<is_number_non_floating_point_v<T>>>
    T FindNumberLast(const std::string & STR, const T RETURN_ON_ERROR)
    {
        const auto NUMBER_STR_VEC { MakeNumberStrings(STR) };

        if (NUMBER_STR_VEC.empty())
        {
            return RETURN_ON_ERROR;
        }
        else
        {
            return ToNumber<T>(NUMBER_STR_VEC.back(), RETURN_ON_ERROR);
        }
    }

    int FindFirstNumber(const std::string & STR, const int RETURN_ON_ERROR = -1);
    int FindLastNumber(const std::string & STR, const int RETURN_ON_ERROR = -1);

    namespace string_helpers
    {
        const std::string NameEqualsValueStr(
            const std::string & NAME,
            const std::string & VALUE_STR,
            const bool WILL_WRAP,
            const std::string & PREFIX,
            const bool IS_STR_TYPE);
    }

    template <typename T>
    const std::string NameEqualsValueStr(
        const std::string & NAME,
        const T VALUE,
        const bool WILL_WRAP = false,
        const std::string & PREFIX = "")
    {
        const bool IS_STR_TYPE { (
            std::is_same<std::remove_const_t<T>, char *>::value
            || std::is_same<std::remove_const_t<T>, std::string>::value) };

        return string_helpers::NameEqualsValueStr(
            NAME, ToString(VALUE), WILL_WRAP, PREFIX, IS_STR_TYPE);
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
