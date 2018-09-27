// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_MISC_ENUM_UTIL_HPP_INCLUDED
#define HEROESPATH_MISC_ENUM_UTIL_HPP_INCLUDED
//
// enum-util.hpp
//
#include "log/log-macros.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/boost-string-includes.hpp"
#include "misc/vector-map.hpp"
#include "misc/wrap-enum.hpp"
#include "stringutil/stringhelp.hpp"

#include <boost/type_index.hpp>

#include <cctype>
#include <exception>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

namespace heroespath
{
namespace misc
{

    using EnumUnderlying_t = unsigned int;

    namespace enum_util
    {
        void TestLog(const std::string &);

        template <typename EnumWrapper_t>
        void Test(
            const EnumUnderlying_t LAST_VALID_VALUE,
            const bool MUST_FIRST_STRING_TO_BE_EMPTY,
            const bool WILL_DISPLAY_MSG_ON_SCREEN = true)
        {
            std::ostringstream msgSS;
            msgSS << "misc::enum_util::TestToString<" << EnumWrapper_t::TypeName() << ", "
                  << EnumWrapper_t::UnderlyingTypeName()
                  << ">(last_valid_value=" << LAST_VALID_VALUE
                  << ", must_first_be_empty=" << std::boolalpha << MUST_FIRST_STRING_TO_BE_EMPTY
                  << ") ";

            if (WILL_DISPLAY_MSG_ON_SCREEN)
            {
                TestLog(msgSS.str() + "Starting...");
            }

            M_ASSERT_OR_LOGANDTHROW_SS(
                (LAST_VALID_VALUE > 0),
                msgSS.str() << "was given a last_valid_value that was not > zero.");

            static EnumUnderlying_t flagValue { 0 };

            // this is used to verify ToString() returns a unique string for each unique enum
            static std::vector<std::string> alreadyGeneratedStrings;
            if (alreadyGeneratedStrings.capacity() < static_cast<std::size_t>(LAST_VALID_VALUE))
            {
                alreadyGeneratedStrings.reserve(static_cast<std::size_t>(LAST_VALID_VALUE));
            }

            try
            {
                while (flagValue <= LAST_VALID_VALUE)
                {
                    auto const FLAG_VALUE_TO_TEST_AND_REPORT { flagValue++ };

                    if (EnumWrapper_t::IsValid(static_cast<typename EnumWrapper_t::Enum>(
                            FLAG_VALUE_TO_TEST_AND_REPORT))
                        == false)
                    {
                        M_HP_LOG_DBG(
                            msgSS.str()
                            << "skipping invalid value=" << FLAG_VALUE_TO_TEST_AND_REPORT);

                        continue;
                    }

                    auto const STRING { EnumWrapper_t::ToString(
                        static_cast<typename EnumWrapper_t::Enum>(FLAG_VALUE_TO_TEST_AND_REPORT)) };

                    if ((FLAG_VALUE_TO_TEST_AND_REPORT == 0) && MUST_FIRST_STRING_TO_BE_EMPTY)
                    {
                        M_ASSERT_OR_LOGANDTHROW_SS(
                            (STRING.empty()),
                            msgSS.str() << "ToString() returned non-empty string (\"" << STRING
                                        << "\") on first value.");
                    }
                    else
                    {
                        M_ASSERT_OR_LOGANDTHROW_SS(
                            (STRING.empty() == false),
                            msgSS.str() << "ToString() returned an empty string on value "
                                        << FLAG_VALUE_TO_TEST_AND_REPORT << ".");
                    }

                    auto const IS_DUPLICATE { std::find(
                                                  std::begin(alreadyGeneratedStrings),
                                                  std::end(alreadyGeneratedStrings),
                                                  STRING)
                                              != std::end(alreadyGeneratedStrings) };

                    M_ASSERT_OR_LOGANDTHROW_SS(
                        (IS_DUPLICATE == false),
                        msgSS.str() << "value=" << FLAG_VALUE_TO_TEST_AND_REPORT << "=\"" << STRING
                                    << "\" is a duplicate of a previous generated string.");

                    auto const FROM_STRING_RESULT { EnumWrapper_t::FromString(STRING) };

                    M_ASSERT_OR_LOGANDTHROW_SS(
                        (FROM_STRING_RESULT == FLAG_VALUE_TO_TEST_AND_REPORT),
                        msgSS.str() << "FromString(\"" << STRING << "\")=" << FROM_STRING_RESULT
                                    << "!=(expected)=" << FLAG_VALUE_TO_TEST_AND_REPORT << ".");

                    M_ASSERT_OR_LOGANDTHROW_SS(
                        (EnumWrapper_t::IsValid(
                            static_cast<typename EnumWrapper_t::Enum>(FROM_STRING_RESULT))),
                        msgSS.str()
                            << "FromString(\"" << STRING << "\")=" << FROM_STRING_RESULT
                            << "=(expected)=" << FLAG_VALUE_TO_TEST_AND_REPORT
                            << " -but that value was not valid.  (IsValid() returned false)");
                }
            }
            catch (const std::exception & EXCEPTION)
            {
                --flagValue;

                M_HP_LOG_ERR(
                    msgSS.str() << "threw exception \"" << EXCEPTION.what()
                                << "\" on value=" << flagValue << ".");

                throw;
            }

            if (WILL_DISPLAY_MSG_ON_SCREEN)
            {
                msgSS << "Done testing all " << flagValue << " values.";
                TestLog(msgSS.str());
            }
        }
    } // namespace enum_util

    enum class NoneEmpty : bool
    {
        No = false,
        Yes = true
    };

    struct EnumFirstValueValid
    {};

    struct EnumFirstValueNot
    {};

    struct EnumFirstValueNothing
    {};

    struct EnumFirstValueNever
    {};

    struct EnumFirstValueNone
    {};

    // Responsible for common operations of enums that are used as counted values.
    template <typename EnumWrapper_t, typename EnumFirstValue_t>
    class EnumBaseCounting
    {
    public:
        EnumBaseCounting() = delete;

        static bool IsValid(const EnumUnderlying_t ENUM_VALUE)
        {
            return (ENUM_VALUE <= LargestValidValue());
        }

        static EnumUnderlying_t ToUnderlyingType(const EnumUnderlying_t ENUM_VALUE)
        {
            return ENUM_VALUE;
        }

        static bool IsValidAndNonZero(const EnumUnderlying_t ENUM_VALUE)
        {
            return ((ENUM_VALUE != 0) && IsValid(ENUM_VALUE));
        }

        static constexpr EnumUnderlying_t LargestValidValue() { return EnumWrapper_t::Count - 1; }

        static const std::string ToStringNoThrow(const EnumUnderlying_t ENUM_VALUE)
        {
            if (IsValid(ENUM_VALUE))
            {
                return EnumWrapper_t::ToString(
                    static_cast<typename EnumWrapper_t::Enum>(ENUM_VALUE));
            }
            else
            {
                if (ENUM_VALUE == EnumWrapper_t::Count)
                {
                    return "Count";
                }
                else
                {
                    std::ostringstream ss;
                    ss << "(INVALID=" << ENUM_VALUE << ", MAX=" << LargestValidValue() << ")";
                    return ss.str();
                }
            }
        }

        static const std::string Name(const EnumUnderlying_t ENUM_VALUE)
        {
            return EnumWrapper_t::ToString(static_cast<typename EnumWrapper_t::Enum>(ENUM_VALUE));
        }

        static EnumUnderlying_t FromString(const std::string & S)
        {
            for (EnumUnderlying_t i(0); i < EnumWrapper_t::Count; ++i)
            {
                if (boost::algorithm::iequals(
                        S, EnumWrapper_t::ToString(static_cast<typename EnumWrapper_t::Enum>(i))))
                {
                    return i;
                }
            }

            std::ostringstream ss;
            ss << "misc::EnumBaseCounting::FromString(\"" << S
               << "\")_InvalidValueError.  (typename=" << TypeName() << ")";

            throw std::runtime_error(ss.str());
        }

        static void Test(const bool WILL_DISPLAY_MSG_ON_SCREEN = true)
        {
            using UnderlyingTypeActual_t =
                typename std::underlying_type<typename EnumWrapper_t::Enum>::type;

            M_ASSERT_OR_LOGANDTHROW_SS(
                (std::is_same<EnumUnderlying_t, UnderlyingTypeActual_t>::value),
                TypeName() << "Underlying type was: "
                           << boost::typeindex::type_id<UnderlyingTypeActual_t>().pretty_name()
                           << " instead of what it should be: " << UnderlyingTypeName() << ".");

            if constexpr (std::is_same<EnumFirstValue_t, EnumFirstValueNot>::value)
            {
                M_ASSERT_OR_LOGANDTHROW_SS_CONSTEXPR(
                    (EnumWrapper_t::Not == 0),
                    TypeName() << "::Not=" << EnumWrapper_t::Not << " instead of zero.");
            }

            if constexpr (std::is_same<EnumFirstValue_t, EnumFirstValueNothing>::value)
            {
                M_ASSERT_OR_LOGANDTHROW_SS_CONSTEXPR(
                    (EnumWrapper_t::Nothing == 0),
                    TypeName() << "::Nothing=" << EnumWrapper_t::Nothing << " instead of zero.");
            }

            if constexpr (std::is_same<EnumFirstValue_t, EnumFirstValueNever>::value)
            {
                M_ASSERT_OR_LOGANDTHROW_SS_CONSTEXPR(
                    (EnumWrapper_t::Never == 0),
                    TypeName() << "::Never=" << EnumWrapper_t::Never << " instead of zero.");
            }

            if constexpr (std::is_same<EnumFirstValue_t, EnumFirstValueNone>::value)
            {
                M_ASSERT_OR_LOGANDTHROW_SS_CONSTEXPR(
                    (EnumWrapper_t::None == 0),
                    TypeName() << "::None=" << EnumWrapper_t::None << " instead of zero.");
            }

            M_ASSERT_OR_LOGANDTHROW_SS_CONSTEXPR(
                (EnumWrapper_t::Count > 0),
                TypeName() << "::Count=" << static_cast<EnumUnderlying_t>(EnumWrapper_t::Count)
                           << " is not > zero.");

            M_ASSERT_OR_LOGANDTHROW_SS_CONSTEXPR(
                (EnumWrapper_t::Count == LargestValidValue() + 1),
                TypeName() << "::Count=" << static_cast<EnumUnderlying_t>(EnumWrapper_t::Count)
                           << " is not one less than the largest valid value="
                           << LargestValidValue() << ".");

            enum_util::Test<EnumWrapper_t>(LargestValidValue(), false, WILL_DISPLAY_MSG_ON_SCREEN);
        }

        static const std::string TypeName()
        {
            return boost::typeindex::type_id<EnumWrapper_t>().pretty_name() + "::Enum";
        }

        static const std::string UnderlyingTypeName()
        {
            return boost::typeindex::type_id<EnumUnderlying_t>().pretty_name();
        }

        static const std::string InvalidEnumValueStringForFunction(
            const EnumUnderlying_t ENUM_VALUE, const std::string & FUNCTION_NAME)
        {
            std::ostringstream ss;

            ss << TypeName() << "::" << FUNCTION_NAME
               << "(enum_value=INVALID=" << UnderlyingTypeName() << "=" << ENUM_VALUE
               << ") largest_valid=" << LargestValidValue();

            return ss.str();
        }

        [[noreturn]] static void ThrowInvalidValueForFunction(
            const EnumUnderlying_t ENUM_VALUE, const std::string & FUNCTION_NAME)
        {
            throw std::range_error(InvalidEnumValueStringForFunction(ENUM_VALUE, FUNCTION_NAME));
        }
    };

    // Responsible for common operations of enums that are used as bit flags.
    template <typename EnumWrapper_t, NoneEmpty WILL_NONE_RETURN_EMPTY = NoneEmpty::Yes>
    class EnumBaseBitField
    {
    public:
        EnumBaseBitField() = delete;

        static bool IsValid(const EnumUnderlying_t ENUM_VALUE)
        {
            return (ENUM_VALUE <= LargestValidValue());
        }

        static bool IsValidAndNonZero(const EnumUnderlying_t ENUM_VALUE)
        {
            return ((ENUM_VALUE != 0) && IsValid(ENUM_VALUE));
        }

        static const std::string ToString(
            const EnumUnderlying_t ENUM_VALUE,
            const Wrap WILL_WRAP = Wrap::No,
            const std::string & SEPARATOR = ", ",
            const NoneEmpty WILL_NONE_RETURN_EMPTY_PARAM = WILL_NONE_RETURN_EMPTY,
            const bool WILL_THROW_ON_ERROR = true)
        {
            std::ostringstream ss;

            if (ENUM_VALUE == EnumWrapper_t::None)
            {
                if (WILL_NONE_RETURN_EMPTY_PARAM == NoneEmpty::Yes)
                {
                    return "";
                }
                else
                {
                    ss << "None";
                }
            }
            else
            {
                EnumWrapper_t::ToStringPopulate(ss, ENUM_VALUE, SEPARATOR);
            }

            if (ss.str().empty())
            {
                if (WILL_THROW_ON_ERROR)
                {
                    ThrowInvalidValueForFunction(ENUM_VALUE, "ToString");
                }
                else
                {
                    std::ostringstream ssError;
                    ssError << "(INVALID=" << ENUM_VALUE << ", MAX=" << LargestValidValue() << ")";
                    return ssError.str();
                }
            }
            else
            {
                if (WILL_WRAP == Wrap::Yes)
                {
                    return "(" + ss.str() + ")";
                }
                else
                {
                    return ss.str();
                }
            }
        }

        static const std::string ToStringNoThrow(
            const EnumUnderlying_t ENUM_VALUE,
            const Wrap WILL_WRAP = Wrap::No,
            const std::string & SEPARATOR = ", ",
            const NoneEmpty WILL_NONE_RETURN_EMPTY_PARAM = WILL_NONE_RETURN_EMPTY)
        {
            return ToString(ENUM_VALUE, WILL_WRAP, SEPARATOR, WILL_NONE_RETURN_EMPTY_PARAM, false);
        }

        static EnumUnderlying_t FromString(const std::string & S)
        {
            auto const TRIMMED { boost::algorithm::trim_copy(S) };

            if (TRIMMED.empty())
            {
                return EnumWrapper_t::None;
            }

            std::string seperatorChars { "" };

            for (auto const CHAR : TRIMMED)
            {
                // spaces are not valid separators because some names will have spaces
                if ((std::isalpha(CHAR) == false) && (std::isspace(CHAR) == false) && (CHAR != '-')
                    && (CHAR != ':'))
                {
                    seperatorChars.push_back(CHAR);
                }
            }

            std::vector<std::string> words;

            if (seperatorChars.empty())
            {
                words.emplace_back(TRIMMED);
            }
            else
            {
                appbase::stringhelp::SplitByChars(TRIMMED, words, seperatorChars, true, true);
            }

            EnumUnderlying_t result { 0 };
            for (auto const & WORD : words)
            {
                EnumUnderlying_t flag { 1 };

                while (flag <= EnumWrapper_t::Last)
                {
                    if (boost::algorithm::iequals(
                            WORD,
                            EnumBaseBitField<EnumWrapper_t, WILL_NONE_RETURN_EMPTY>::ToString(
                                flag, Wrap::No, "", NoneEmpty::No)))
                    {
                        result |= flag;
                        break;
                    }

                    flag <<= 1;
                }
            }

            return result;
        }

        static int CountBitsSet(const EnumUnderlying_t ENUM_VALUE)
        {
            int result { 0 };
            EnumUnderlying_t flag { 1 };

            while (flag <= EnumWrapper_t::Last)
            {
                if (ENUM_VALUE & flag)
                {
                    ++result;
                }

                flag <<= 1;
            }

            return result;
        }

        static const std::string Name(const EnumUnderlying_t ENUM_VALUE)
        {
            return EnumWrapper_t::ToString(ENUM_VALUE);
        }

        static EnumUnderlying_t LargestValidValue()
        {
            static EnumUnderlying_t finalValue { 0 };

            if (0 == finalValue)
            {
                EnumUnderlying_t shiftingValue { 1 };

                while (shiftingValue <= EnumWrapper_t::Last)
                {
                    finalValue |= shiftingValue;
                    shiftingValue <<= 1;
                }
            }

            return finalValue;
        }

        static void Test(const bool WILL_DISPLAY_MSG_ON_SCREEN = true)
        {
            using UnderlyingTypeActual_t =
                typename std::underlying_type<typename EnumWrapper_t::Enum>::type;

            M_ASSERT_OR_LOGANDTHROW_SS(
                (std::is_same<EnumUnderlying_t, UnderlyingTypeActual_t>::value),
                TypeName() << "Underlying type was: "
                           << boost::typeindex::type_id<UnderlyingTypeActual_t>().pretty_name()
                           << " instead of what it should be: " << UnderlyingTypeName() << ".");

            M_ASSERT_OR_LOGANDTHROW_SS_CONSTEXPR(
                (EnumWrapper_t::None == 0), TypeName() << "::None was not zero.");

            M_ASSERT_OR_LOGANDTHROW_SS_CONSTEXPR(
                (EnumWrapper_t::Last > 0),
                TypeName() << "::Last=" << static_cast<EnumUnderlying_t>(EnumWrapper_t::Last)
                           << " is not > zero.");

            if (EnumWrapper_t::Last > LargestValidValue())
            {
                std::ostringstream ss;
                ss << TypeName() << "::Last=" << static_cast<EnumUnderlying_t>(EnumWrapper_t::Last)
                   << " is not <= the largest valid value=" << LargestValidValue() << ".";

                throw std::runtime_error(ss.str());
            }

            enum_util::Test<EnumWrapper_t>(
                LargestValidValue(),
                (WILL_NONE_RETURN_EMPTY == NoneEmpty::Yes),
                WILL_DISPLAY_MSG_ON_SCREEN);
        }

        static const std::string TypeName()
        {
            return boost::typeindex::type_id<EnumWrapper_t>().pretty_name() + "::Enum";
        }

        static const std::string UnderlyingTypeName()
        {
            return boost::typeindex::type_id<EnumUnderlying_t>().pretty_name();
        }

        static const std::string InvalidEnumValueStringForFunction(
            const EnumUnderlying_t ENUM_VALUE, const std::string & FUNCTION_NAME)
        {
            std::ostringstream ss;

            ss << TypeName() << "::" << FUNCTION_NAME
               << "(enum_value=INVALID=" << UnderlyingTypeName() << "=" << ENUM_VALUE
               << ") largest_valid=" << LargestValidValue();

            return ss.str();
        }

        [[noreturn]] static void ThrowInvalidValueForFunction(
            const EnumUnderlying_t ENUM_VALUE, const std::string & FUNCTION_NAME)
        {
            throw std::range_error(InvalidEnumValueStringForFunction(ENUM_VALUE, FUNCTION_NAME));
        }

    protected:
        static void AppendNameIfBitIsSet(
            std::ostringstream & ss,
            const EnumUnderlying_t ENUM_VALUE,
            const EnumUnderlying_t BIT_TO_CHECK,
            const std::string & NAME,
            const std::string & SEPARATOR = ", ")
        {
            if (ENUM_VALUE & BIT_TO_CHECK)
            {
                if (ss.str().empty() == false)
                {
                    ss << SEPARATOR;
                }

                ss << NAME;
            }
        }
    };

} // namespace misc

template <typename T, typename = std::enable_if_t<std::is_enum<T>::value>>
inline constexpr T operator~(const T X)
{
    return static_cast<T>(~static_cast<misc::EnumUnderlying_t>(X));
}

template <typename T, typename = std::enable_if_t<std::is_enum<T>::value>>
inline constexpr T operator|(const T L, const T R)
{
    return static_cast<T>(
        static_cast<misc::EnumUnderlying_t>(L) | static_cast<misc::EnumUnderlying_t>(R));
}

template <typename T, typename = std::enable_if_t<std::is_enum<T>::value>>
inline constexpr T operator&(const T L, const T R)
{
    return static_cast<T>(
        static_cast<misc::EnumUnderlying_t>(L) & static_cast<misc::EnumUnderlying_t>(R));
}

template <typename T, typename = std::enable_if_t<std::is_enum<T>::value>>
inline constexpr T operator^(const T L, const T R)
{
    return static_cast<T>(
        static_cast<misc::EnumUnderlying_t>(L) ^ static_cast<misc::EnumUnderlying_t>(R));
}

template <typename T, typename = std::enable_if_t<std::is_enum<T>::value>>
inline constexpr T & operator|=(T & l, const T R)
{
    l = static_cast<T>(
        static_cast<misc::EnumUnderlying_t>(l) | static_cast<misc::EnumUnderlying_t>(R));
    return l;
}

template <typename T, typename = std::enable_if_t<std::is_enum<T>::value>>
inline constexpr T & operator&=(T & l, const T R)
{
    l = static_cast<T>(
        static_cast<misc::EnumUnderlying_t>(l) & static_cast<misc::EnumUnderlying_t>(R));
    return l;
}

template <typename T, typename = std::enable_if_t<std::is_enum<T>::value>>
inline constexpr T & operator^=(T & l, const T R)
{
    l = static_cast<T>(
        static_cast<misc::EnumUnderlying_t>(l) ^ static_cast<misc::EnumUnderlying_t>(R));
    return l;
}

template <
    typename T1,
    typename T2,
    typename std::enable_if<
        (std::is_enum<T1> {}
         && (std::is_same<T1, T2> {} || std::is_same<T2, misc::EnumUnderlying_t> {}))
            || (std::is_enum<T2> {}
                && (std::is_same<T1, T2> {} || std::is_same<T1, misc::EnumUnderlying_t> {}))
            || (std::is_same<T1, misc::EnumUnderlying_t> {}
                && std::is_same<T2, misc::EnumUnderlying_t> {}),
        int>::type
    = 0>
inline constexpr void BitSet(T1 & orig, const T2 BITS_TO_SET)
{
    orig |= BITS_TO_SET;
}

template <
    typename T1,
    typename T2,
    typename std::enable_if<
        (std::is_enum<T1> {}
         && (std::is_same<T1, T2> {} || std::is_same<T2, misc::EnumUnderlying_t> {}))
            || (std::is_enum<T2> {}
                && (std::is_same<T1, T2> {} || std::is_same<T1, misc::EnumUnderlying_t> {}))
            || (std::is_same<T1, misc::EnumUnderlying_t> {}
                && std::is_same<T2, misc::EnumUnderlying_t> {}),
        int>::type
    = 0>
inline constexpr T1 BitSetCopy(const T1 ORIG, const T2 BITS_TO_SET)
{
    T1 copy { ORIG };
    BitSet(copy, BITS_TO_SET);
    return copy;
}

template <
    typename T1,
    typename T2,
    typename std::enable_if<
        (std::is_enum<T1> {}
         && (std::is_same<T1, T2> {} || std::is_same<T2, misc::EnumUnderlying_t> {}))
            || (std::is_enum<T2> {}
                && (std::is_same<T1, T2> {} || std::is_same<T1, misc::EnumUnderlying_t> {}))
            || (std::is_same<T1, misc::EnumUnderlying_t> {}
                && std::is_same<T2, misc::EnumUnderlying_t> {}),
        int>::type
    = 0>
inline constexpr void BitClear(T1 & orig, const T2 BITS_TO_CLEAR)
{
    orig &= ~BITS_TO_CLEAR;
}

template <
    typename T1,
    typename T2,
    typename std::enable_if<
        (std::is_enum<T1> {}
         && (std::is_same<T1, T2> {} || std::is_same<T2, misc::EnumUnderlying_t> {}))
            || (std::is_enum<T2> {}
                && (std::is_same<T1, T2> {} || std::is_same<T1, misc::EnumUnderlying_t> {}))
            || (std::is_same<T1, misc::EnumUnderlying_t> {}
                && std::is_same<T2, misc::EnumUnderlying_t> {}),
        int>::type
    = 0>
inline constexpr T1 BitClearCopy(const T1 ORIG, const T2 BITS_TO_CLEAR)
{
    T1 copy { ORIG };
    BitClear(copy, BITS_TO_CLEAR);
    return copy;
}

template <
    typename T1,
    typename T2,
    typename std::enable_if<
        (std::is_enum<T1> {}
         && (std::is_same<T1, T2> {} || std::is_same<T2, misc::EnumUnderlying_t> {}))
            || (std::is_enum<T2> {}
                && (std::is_same<T1, T2> {} || std::is_same<T1, misc::EnumUnderlying_t> {}))
            || (std::is_same<T1, misc::EnumUnderlying_t> {}
                && std::is_same<T2, misc::EnumUnderlying_t> {}),
        int>::type
    = 0>
inline constexpr void BitToggle(T1 & orig, const T2 BITS_TO_TOGGLE)
{
    orig ^= BITS_TO_TOGGLE;
}

template <
    typename T1,
    typename T2,
    typename std::enable_if<
        (std::is_enum<T1> {}
         && (std::is_same<T1, T2> {} || std::is_same<T2, misc::EnumUnderlying_t> {}))
            || (std::is_enum<T2> {}
                && (std::is_same<T1, T2> {} || std::is_same<T1, misc::EnumUnderlying_t> {}))
            || (std::is_same<T1, misc::EnumUnderlying_t> {}
                && std::is_same<T2, misc::EnumUnderlying_t> {}),
        int>::type
    = 0>
inline constexpr T1 BitToggleCopy(const T1 ORIG, const T2 BITS_TO_TOGGLE)
{
    T1 copy { ORIG };
    BitToggle(copy, BITS_TO_TOGGLE);
    return copy;
}

} // namespace heroespath

#endif // HEROESPATH_MISC_ENUM_UTIL_HPP_INCLUDED
