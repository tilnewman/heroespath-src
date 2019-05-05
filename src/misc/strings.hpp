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
#include "misc/enum-common.hpp"
#include "misc/string-stream-holder.hpp"
#include "misc/type-helpers.hpp"

#include <algorithm>
#include <iomanip>
#include <string>
#include <vector>

namespace heroespath
{
namespace misc
{

    [[nodiscard]] constexpr bool IsUpper(const char CH) noexcept
    {
        return !((CH < 'A') || (CH > 'Z'));
    }

    [[nodiscard]] constexpr bool IsLower(const char CH) noexcept
    {
        return !((CH < 'a') || (CH > 'z'));
    }

    inline void ToUpper(char & ch) noexcept
    {
        if (IsLower(ch))
        {
            ch = static_cast<char>(ch - 32);
        }
    }

    [[nodiscard]] inline char ToUpperCopy(const char CH) noexcept
    {
        auto copy { CH };
        ToUpper(copy);
        return copy;
    }

    inline void ToUpper(std::string & str) noexcept
    {
        for (char & ch : str)
        {
            ToUpper(ch);
        }
    }

    [[nodiscard]] inline const std::string ToUpperCopy(const std::string & STR) noexcept
    {
        auto copy { STR };
        ToUpper(copy);
        return copy;
    }

    inline void ToLower(char & ch) noexcept
    {
        if (IsUpper(ch))
        {
            ch += char(32);
        }
    }

    [[nodiscard]] inline char ToLowerCopy(const char CH) noexcept
    {
        auto copy { CH };
        ToLower(copy);
        return copy;
    }

    inline void ToLower(std::string & str) noexcept
    {
        for (char & ch : str)
        {
            ToLower(ch);
        }
    }

    [[nodiscard]] inline const std::string ToLowerCopy(const std::string & STR) noexcept
    {
        auto copy { STR };
        ToLower(copy);
        return copy;
    }

    [[nodiscard]] constexpr bool IsEitherNewline(const char CH) noexcept
    {
        return ((CH == '\r') || (CH == '\n'));
    }

    [[nodiscard]] constexpr bool IsAlpha(const char CH) noexcept
    {
        return (IsUpper(CH) || IsLower(CH));
    }

    [[nodiscard]] constexpr bool IsDigit(const char CH) noexcept
    {
        return !((CH < '0') || (CH > '9'));
    }

    [[nodiscard]] constexpr bool IsAlphaOrDigit(const char CH) noexcept
    {
        return (IsAlpha(CH) || IsDigit(CH));
    }

    [[nodiscard]] constexpr bool IsWhitespace(const char CH) noexcept
    {
        return ((CH == ' ') || (CH == '\t') || (CH == '\r') || (CH == '\n'));
    }

    [[nodiscard]] constexpr bool IsDisplayable(const char CH) noexcept
    {
        return (((CH > 31) && (CH != 127)) || IsWhitespace(CH));
    }

    [[nodiscard]] constexpr bool IsNonDisplayable(const char CH) noexcept
    {
        return !IsDisplayable(CH);
    }

    [[nodiscard]] constexpr bool IsWhitespaceOrNonDisplayable(const char CH) noexcept
    {
        return ((CH < 33) || (CH == 127));
    }

    [[nodiscard]] bool AreEqualCaseInsensitive(const std::string & A, const std::string & B);

    template <typename InputIter_t, typename SeparatorDetector_t>
    const std::string::const_iterator MoveToEndAndReturnBeginOfNextSeparated(
        InputIter_t & iter, const InputIter_t LAST, SeparatorDetector_t & separatorDetector)
    {
        iter = std::find_if_not(iter, LAST, separatorDetector);
        const auto LINE_FIRST = iter;
        iter = std::find_if(iter, LAST, separatorDetector);
        return LINE_FIRST;
    }

    template <
        typename InputIter_t,
        typename SeparatorDetector_t,
        typename CallOnAllFoundAndReturnTrueToStop_t>
    bool FindStringsBetweenSeparatorsAndCall(
        const InputIter_t FIRST,
        const InputIter_t LAST,
        SeparatorDetector_t & separatorDetector,
        CallOnAllFoundAndReturnTrueToStop_t & callOnAllFound)
    {
        auto iter = FIRST;
        while (iter != LAST)
        {
            const auto STR_BETWEEN_BEGIN
                = MoveToEndAndReturnBeginOfNextSeparated(iter, LAST, separatorDetector);

            // iter is now the STR_BETWEEN_END

            if (STR_BETWEEN_BEGIN != iter)
            {
                if (callOnAllFound(STR_BETWEEN_BEGIN, iter))
                {
                    return true;
                }
            }
        }

        return false;
    }

    namespace helpers
    {

        [[nodiscard]] constexpr bool IsWhitespaceOrPlus(const char C) noexcept
        {
            return (IsWhitespace(C) || (C == '+'));
        }

        using Str2NumInteger_t = unsigned long long;
        using Str2NumReal_t = double;

        const std::tuple<bool, Str2NumInteger_t> ToNumber_ParseInteger(
            std::string::const_iterator iter, const std::string::const_iterator END);

        const std::tuple<bool, Str2NumReal_t> ToNumber_ParseFraction(
            std::string::const_iterator iter, const std::string::const_iterator END);

        struct PreParseResult
        {
            PreParseResult(const std::string::const_iterator LAST)
                : success(false)
                , is_negative(false)
                , integer_begin_iter(LAST)
                , integer_end_iter(LAST)
                , fraction_begin_iter(LAST)
                , fraction_end_iter(LAST)
            {}

            bool success;
            bool is_negative;
            std::string::const_iterator integer_begin_iter;
            std::string::const_iterator integer_end_iter;
            std::string::const_iterator fraction_begin_iter;
            std::string::const_iterator fraction_end_iter;
        };

        // returns iters such that there are only isDigit() between begin and end
        const PreParseResult ToNumber_PreParse(
            const std::string::const_iterator FIRST, const std::string::const_iterator LAST);

        template <typename T>
        [[nodiscard]] std::enable_if_t<are_arithmetic_nobool_v<T>, bool> ToNumber_Impl(
            const std::string::const_iterator FIRST,
            const std::string::const_iterator LAST,
            T & result)
        {
            const auto PARSE = ToNumber_PreParse(FIRST, LAST);

            if (!PARSE.success)
            {
                return false;
            }

            if (PARSE.is_negative && !are_signed_v<T>)
            {
                return false;
            }

            const auto [INTEGER_SUCCESS, INTEGER]
                = ToNumber_ParseInteger(PARSE.integer_begin_iter, PARSE.integer_end_iter);

            if (!INTEGER_SUCCESS)
            {
                return false;
            }

            if constexpr (are_floating_point_v<T>)
            {
                const auto [FRACTION_SUCCESS, FRACTION]
                    = ToNumber_ParseFraction(PARSE.fraction_begin_iter, PARSE.fraction_end_iter);

                if (!FRACTION_SUCCESS)
                {
                    return false;
                }

                const Str2NumReal_t TOTAL((static_cast<Str2NumReal_t>(INTEGER) + FRACTION));

                // if (TOTAL > static_cast<Str2NumReal_t>(std::numeric_limits<T>::max()))
                //{
                //    return false;
                //}
                //
                // if (TOTAL <
                // static_cast<Str2NumReal_t>(std::numeric_limits<T>::lowest()))
                //{
                //    return false;
                //}

                result = static_cast<T>(TOTAL);
            }
            else
            {
                // if (INTEGER >
                // static_cast<Str2NumInteger_t>(std::numeric_limits<T>::max()))
                //{
                //    return false;
                //}
                //
                // if (INTEGER <
                // static_cast<Str2NumInteger_t>(std::numeric_limits<T>::lowest()))
                //{
                //    return false;
                //}

                result = static_cast<T>(INTEGER);
            }

            if (PARSE.is_negative)
            {
                result = -result;
            }

            return true;
        }

    } // namespace helpers

    // ignores surrounding whitespace and leading plus signs, fails if there are no digits,
    // fails if the number is negative and the type is unsigned, if false is returned then the
    // result is not changed
    //
    // My hand-rolled ToNumber() function beats std::sto, boost::lexical_cast, and
    // stringstream:
    //
    // sto: msvc: int=.5x float= 4x   mac/clang: int= 2x float= 2x   linux/gcc: int=-.3x float=
    // 3x lex: msvc: int=.5x float=25x   mac/clang: int= 2x float=25x   linux/gcc: int=  1x
    // float=14x ss:  msvc: int=18x float=20x   mac/clang: int=15x float=20x   linux/gcc: int=
    // 4x float= 7x
    //
    // Those times include both BOOST_LEXICAL_CAST_ASSUME_C_LOCALE and
    // ss::imbue(locale::classic().
    //
    // So with ints   I'm:  2x vs sto/lexical_cast and:   5x-20x vs stringstream.
    // So with floats I'm:  3x vs sto              and:   20x    vs lexical_cast/stringstream.
    //
    // So my closest rival is std::sto, and I'm maybe not fast enough to warrant using
    // hand-rolled code, except that std::sto is REALLY annoying.  For example:
    //
    // It basically accepts any crap after an initial valid number without throwing, but...
    //  std::stoi("1.")                     == 1
    //  std::stoi("1a")                     == 1
    //  std::stoi("1 1")                    == 1
    //  std::stoi("1:cat_eats_dog 123!@#")  == 1
    //
    // ...it will throw if, and god forbid, there is a leading decimal point on an integer
    // type?!
    //  std::stoi(".1")                     == throws exception! (why not just return zero?)
    //  std::stoi(".1a")                    == throws exception! (why not just return zero?)
    //  std::stoi(".1 1")                   == throws exception! (why not just return zero?)
    //  std::stoi(".1:cat_eats_dog 123!@#") == throws exception! (why not just return zero?)
    //
    // So I decided to use my hand-rolled version because I can customize the behavior to
    // suit my needs and it's 2x to 25x faster.  So there.  Ahem.
    template <typename T>
    [[nodiscard]] std::enable_if_t<are_arithmetic_nobool_v<T>, bool> ToNumber(
        const std::string::const_iterator FIRST, const std::string::const_iterator LAST, T & result)
    {
        return helpers::ToNumber_Impl(FIRST, LAST, result);
    }

    template <typename T>
    [[nodiscard]] std::enable_if_t<are_arithmetic_nobool_v<T>, bool>
        ToNumber(const std::string & STR, T & result)
    {
        // shortcut for single characters
        if (STR.size() == 1)
        {
            if (IsDigit(STR[0]))
            {
                result = static_cast<T>(STR[0] - '0');
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return helpers::ToNumber_Impl(std::begin(STR), std::end(STR), result);
        }
    }

    template <typename T>
    [[nodiscard]] std::enable_if_t<are_arithmetic_nobool_v<T>, T>
        ToNumberOr(const std::string & STR, const T RETURN_IF_ERROR)
    {
        T result = RETURN_IF_ERROR;
        if (ToNumber(STR, result))
        {
            return result;
        }
        else
        {
            return RETURN_IF_ERROR;
        }
    }

    template <typename T>
    [[nodiscard]] std::enable_if_t<are_arithmetic_nobool_v<T>, T> ToNumberOr(
        const std::string::const_iterator FIRST,
        const std::string::const_iterator LAST,
        const T RETURN_IF_ERROR)
    {
        T result = RETURN_IF_ERROR;
        if (helpers::ToNumber_Impl(FIRST, LAST, result))
        {
            return result;
        }
        else
        {
            return RETURN_IF_ERROR;
        }
    }

    template <typename T>
    [[nodiscard]] std::enable_if_t<are_arithmetic_nobool_v<T>, T>
        ToNumberOrZero(const std::string & STR)
    {
        return ToNumberOr(STR, T(0));
    }

    template <typename T>
    [[nodiscard]] std::enable_if_t<are_arithmetic_nobool_v<T>, T> ToNumberOrZero(
        const std::string::const_iterator FIRST, const std::string::const_iterator LAST)
    {
        return ToNumberOr(FIRST, LAST, T(0));
    }

    template <typename WillTrimDetector_t>
    void TrimIf(std::string & str, WillTrimDetector_t willTrimDetector)
    {
        auto erase_begin
            = std::find_if_not(std::rbegin(str), std::rend(str), willTrimDetector).base();

        if (erase_begin != std::begin(str))
        {
            const auto KEEP_BEGIN
                = std::find_if_not(std::begin(str), std::end(str), willTrimDetector);

            if (KEEP_BEGIN != std::begin(str))
            {
                erase_begin = std::copy(KEEP_BEGIN, erase_begin, std::begin(str));
            }
        }

        str.erase(erase_begin, std::end(str));
    }

    inline void TrimWhitespace(std::string & str) { TrimIf(str, IsWhitespace); }

    [[nodiscard]] inline const std::string TrimWhitespaceCopy(const std::string & STR)
    {
        std::string copy { STR };
        TrimWhitespace(copy);
        return copy;
    }

    inline void TrimNonDisplayable(std::string & str) { TrimIf(str, IsNonDisplayable); }

    [[nodiscard]] inline const std::string TrimNonDisplayableCopy(const std::string & STR)
    {
        std::string copy { STR };
        TrimNonDisplayable(copy);
        return copy;
    }

    inline void TrimWhitespaceAndNonDisplayable(std::string & str)
    {
        TrimIf(str, IsWhitespaceOrNonDisplayable);
    }

    [[nodiscard]] inline const std::string
        TrimWhitespaceAndNonDisplayableCopy(const std::string & STR)
    {
        std::string copy { STR };
        TrimWhitespaceAndNonDisplayable(copy);
        return copy;
    }

    enum class CaseChange
    {
        LowerToUpper,
        UpperToLower,
        Both
    };

    [[nodiscard]] const std::string CamelTo(
        const std::string & STR,
        const std::string & SEPARATOR,
        const CaseChange CASE_CHANGES = CaseChange::Both);

    [[nodiscard]] inline const std::string CamelTo(
        const std::string & STR,
        const char SEPARATOR,
        const CaseChange CASE_CHANGES = CaseChange::Both)
    {
        return CamelTo(STR, std::string(1, SEPARATOR), CASE_CHANGES);
    }

    template <typename T>
    [[nodiscard]] const std::string ToString(const T & THING)
    {
        try
        {
            stringStreamHolder::ostreamer() << THING;
            return stringStreamHolder::toString();
        }
        catch (...)
        {
            return "";
        }
    }

    [[nodiscard]] inline const std::string Quoted(const std::string & STR)
    {
        return ToString(std::quoted(STR));
    }

    template <typename T = int, typename = std::enable_if_t<are_integral_nobool_v<T>>>
    [[nodiscard]] const std::string NumberToStringWithOrdinalSuffix(const T NUMBER)
    {
        std::string str(std::to_string(NUMBER));

        if (NUMBER == 1)
        {
            str += "st";
        }
        else if (NUMBER == 2)
        {
            str += "nd";
        }
        else if (NUMBER == 3)
        {
            str += "rd";
        }
        else
        {
            str += "th";
        }

        return str;
    }

    [[nodiscard]] const std::string MakeLoggableString(
        const std::string &, const std::size_t MAX_SIZE = 40, const bool WILL_ADD_ELIPSIS = true);

    // empty STR_TO_SEARCH or STRS_TO_FIND returns false, any STRS_TO_FIND that are empty will
    // be ignored, any STRS_TO_FIND that are longer than STR_TO_SEARCH will be ignored, if all
    // are empty/ignored then returns false
    [[nodiscard]] bool ContainsAnyOf(
        const std::string & STR_TO_SEARCH,
        const std::vector<std::string> & STRS_TO_FIND,
        const bool IS_CASE_SENSITIVE);

    // returns the NTH_NUMBER (zero indexed) positive int found in STR, if STR is empty or
    // contains no digits or there is no NTH_NUMBER then returns negative. If NTH_NUMBER is negative
    // then returns the last number found.
    //
    //  "This string has these numbers 0.1f and some inside this filename: 'log-234-5-67.txt'"
    //      Find(negative)  = 67
    //      Find(0)         = 0
    //      Find(1)         = 1
    //      Find(2)         = 234
    //      Find(3)         = 5
    //      Find(4)         = 67
    //      Find(5 or more) = negaive
    [[nodiscard]] int FindNumber(const int NTH_NUMBER, const std::string & STR);

    [[nodiscard]] inline int FindNumberFirst(const std::string & STR) { return FindNumber(0, STR); }
    [[nodiscard]] inline int FindNumberLast(const std::string & STR) { return FindNumber(-1, STR); }

    enum SplitOpt : EnumUnderlying_t
    {
        None = 0,
        NoTrim = 1 << 0, // always trims whitespace unless this is set
        SkipEmpty = 1 << 1
    };

    // always trims whitespace unless SplitOpt::NoTrim is set
    struct SplitHow
    {
        SplitHow(const SplitOpt OPTIONS = SplitOpt::None, const std::string & SEPARATOR_CHARS = ",")
            : options(OPTIONS)
            , separator_chars(SEPARATOR_CHARS)
        {}

        SplitHow(const std::string & SEPARATOR_CHARS, const SplitOpt OPTIONS = SplitOpt::None)
            : options(OPTIONS)
            , separator_chars(SEPARATOR_CHARS)
        {}

        SplitHow(const SplitHow &) = default;
        SplitHow(SplitHow &&) = default;
        SplitHow & operator=(const SplitHow &) = default;
        SplitHow & operator=(SplitHow &&) = default;

        SplitOpt options;
        std::string separator_chars;
    };

    // TO_SPLIT is split into pieces by any of the chars in HOW.separator_chars and NOT by that
    // string as a whole.  The resulting pieces will NOT contain any of the separator_chars. If
    // trimming makes a piece empty then HOW.will_skip_empty applies. If HOW.separator_chars is
    // empty then TO_SPLIT is the only string returned.
    [[nodiscard]] const std::vector<std::string>
        SplitByChars(const std::string & TO_SPLIT, const SplitHow HOW = SplitHow());

    enum JoinOpt : EnumUnderlying_t
    {
        Nothing = 0,
        Wrap = 1 << 0, // wraps all after being joined together
        And = 1 << 1, // appends 'and' before the last item joined if more than two total
        Ellipsis = 1 << 2 // appends '...' to the end but only if max_count<CONTAINER.size()
    };

    struct JoinHow
    {
        JoinHow(
            const JoinOpt OPTIONS = JoinOpt::Nothing,
            const std::size_t MAX_COUNT = 0,
            const std::string & SEPARATOR = ", ")
            : options(OPTIONS)
            , separator(SEPARATOR)
            , max_count(MAX_COUNT)
        {}

        JoinHow(const JoinOpt OPTIONS, const std::string & SEPARATOR)
            : options(OPTIONS)
            , separator(SEPARATOR)
            , max_count(0)
        {}

        JoinHow(const std::string & SEPARATOR)
            : options(JoinOpt::Nothing)
            , separator(SEPARATOR)
            , max_count(0)
        {}

        JoinHow(const JoinHow &) = default;
        JoinHow(JoinHow &&) = default;
        JoinHow & operator=(const JoinHow &) = default;
        JoinHow & operator=(JoinHow &&) = default;

        JoinOpt options;
        std::string separator;
        std::size_t max_count;
    };

    template <
        typename Container_t,
        typename ToStringFunction_t,
        typename Value_t = typename Container_t::value_type,
        typename = std::enable_if_t<!std::is_arithmetic_v<ToStringFunction_t>>>
    const std::string Join(
        const Container_t & CONTAINER,
        const JoinHow & HOW,
        const ToStringFunction_t & toStringFunction)
    {
        std::string str;

        if (CONTAINER.empty())
        {
            return str;
        }

        const auto CONTAINER_SIZE = CONTAINER.size();

        std::string temp;
        temp.reserve(32);

        std::vector<std::string> strings;
        strings.reserve(CONTAINER_SIZE);
        std::size_t stringsCharCount(0);

        for (const auto & X : CONTAINER)
        {
            if ((HOW.max_count > 0) && (strings.size() >= HOW.max_count))
            {
                break;
            }

            temp = (toStringFunction(X));

            if (temp.empty())
            {
                continue;
            }

            stringsCharCount += temp.size();
            strings.emplace_back(temp);
        }

        if (strings.empty())
        {
            return str;
        }

        // found by experiment to be a good guess for the game
        str.reserve(2 + stringsCharCount + (strings.size() * HOW.separator.size()));

        if (HOW.options & JoinOpt::Wrap)
        {
            str += "(";
        }

        for (std::size_t i(0); i < (strings.size() - 1); ++i)
        {
            str += strings[i];
            str += HOW.separator;
        }

        if ((HOW.options & JoinOpt::And) && (strings.size() > 2))
        {
            str += "and ";
        }

        str += strings.back();

        if ((HOW.options & JoinOpt::Ellipsis) && (HOW.max_count > 0)
            && (HOW.max_count == strings.size()) && (HOW.max_count < CONTAINER_SIZE))
        {
            str += "...";
        }

        if (HOW.options & JoinOpt::Wrap)
        {
            str += ")";
        }

        return str;
    }

    template <typename Container_t>
    const std::string Join(const Container_t & CONTAINER, const JoinHow & HOW = JoinHow())
    {
        return Join(CONTAINER, HOW, ToString<typename Container_t::value_type>);
    }

} // namespace misc

#define M_HP_VAR_STR(var) ("(" #var "=" + heroespath::misc::ToString(var) + ")")

} // namespace heroespath

#endif // HEROESPATH_MISC_STRINGS_HPP_INCLUDED
