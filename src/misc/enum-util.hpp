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
//  WARNING:
//      To avoid circular dependancies this file cannot include:
//          "misc/log-macros.hpp"
//          "misc/assertlogandthrow.hpp"
//          -or any other header that includes these!
//
//      See the work-around code below.
//
#include "misc/wrap-enum.hpp"

#include <boost/type_index.hpp>

#include <map>
#include <string>
#include <vector>

namespace heroespath
{
namespace misc
{

    using EnumUnderlying_t = std::size_t;

    enum class NoneEmpty : bool
    {
        No = false,
        Yes = true
    };

    enum class EnumFirstValue
    {
        Valid,
        Not,
        Nothing,
        Never,
        None
    };

    struct EnumStringHow
    {
        explicit EnumStringHow(
            const Wrap WILL_WRAP = Wrap::No,
            const std::string & SEPARATOR = ", ",
            const NoneEmpty WILL_NONE_RETURN_EMPT = NoneEmpty::Yes)
            : will_wrap(WILL_WRAP)
            , separator(SEPARATOR)
            , will_none_return_empty(WILL_NONE_RETURN_EMPT)
        {}

        explicit EnumStringHow(
            const std::string & SEPARATOR,
            const Wrap WILL_WRAP = Wrap::No,
            const NoneEmpty WILL_NONE_RETURN_EMPT = NoneEmpty::Yes)
            : will_wrap(WILL_WRAP)
            , separator(SEPARATOR)
            , will_none_return_empty(WILL_NONE_RETURN_EMPT)
        {}

        EnumStringHow(const EnumStringHow &) = default;
        EnumStringHow(EnumStringHow &&) = default;
        EnumStringHow & operator=(const EnumStringHow &) = default;
        EnumStringHow & operator=(EnumStringHow &&) = default;

        Wrap will_wrap;
        std::string separator;
        NoneEmpty will_none_return_empty;
    };

    // This namespace holds a collection of functions that only exist in order to move code out of
    // this header to eliminate includes either to reduce the weight of all enums or to eliminate
    // circular dependancies.
    namespace enum_helpers
    {

        const std::string TrimAndMakeLowerCase(const std::string &);

        const std::vector<std::string> BitwiseEnumFromStringSplit(const std::string &);

        const std::string ValueOutOfRangeErrorString(
            const EnumUnderlying_t ENUM_VALUE,
            const EnumUnderlying_t LARGEST_VALID_VALUE,
            const EnumUnderlying_t COUNT_VALUE);

        EnumUnderlying_t ReturnValueOfStringOr(
            const std::map<std::string, EnumUnderlying_t> & STRING_TO_VALUE_MAP,
            const std::string & STRING_TO_FIND,
            const EnumUnderlying_t TO_RETURN_IF_NOT_FOUND);

        EnumUnderlying_t WordsToBitFlags(
            const std::vector<std::string> & WORDS_TO_FIND,
            const std::map<std::string, EnumUnderlying_t> & WORDS_TO_BIT_FLAG_MAP);

    } // namespace enum_helpers

    // responsible for common operations of counting enums
    template <typename EnumWrapper_t, EnumFirstValue FIRST_VALUE_TYPE>
    class EnumBaseCounting
    {
    public:
        EnumBaseCounting() = delete;

        static constexpr EnumFirstValue first_value_type = FIRST_VALUE_TYPE;

        static const std::string TypeName()
        {
            return boost::typeindex::type_id<EnumWrapper_t>().pretty_name() + "::Enum";
        }

        static const std::string UnderlyingTypeName()
        {
            return boost::typeindex::type_id<EnumUnderlying_t>().pretty_name();
        }

        // this may look redundant but callers can pass in the enum type (EnumWrapper::Enum)
        // and get the underlying type (EnumUnderlying_t) back
        static constexpr EnumUnderlying_t ToUnderlyingType(const EnumUnderlying_t ENUM_VALUE)
        {
            return ENUM_VALUE;
        }

        static constexpr bool IsValid(const EnumUnderlying_t ENUM_VALUE)
        {
            return (ENUM_VALUE <= LargestValidValue());
        }

        static constexpr bool IsValidAndNonZero(const EnumUnderlying_t ENUM_VALUE)
        {
            return ((ENUM_VALUE != 0) && IsValid(ENUM_VALUE));
        }

        static constexpr EnumUnderlying_t LargestValidValue() { return (EnumWrapper_t::Count - 1); }

        static const std::string ValueOutOfRangeErrorString(const EnumUnderlying_t ENUM_VALUE)
        {
            return enum_helpers::ValueOutOfRangeErrorString(
                ENUM_VALUE, LargestValidValue(), EnumWrapper_t::Count);
        }

        // if EnumWrapper_t does not provide its own Name() then use ToString() by default
        static const std::string Name(const EnumUnderlying_t ENUM_VALUE)
        {
            return EnumWrapper_t::ToString(static_cast<typename EnumWrapper_t::Enum>(ENUM_VALUE));
        }

        // case insensitive, returns Count on error
        static EnumUnderlying_t FromString(const std::string & STR_ORIG)
        {
            const auto STR_TRIMMED_LOWER { enum_helpers::TrimAndMakeLowerCase(STR_ORIG) };

            if (nameToValueMap_.empty())
            {
                ClearAndPopulateNameToValueMap();
            }

            return enum_helpers::ReturnValueOfStringOr(
                nameToValueMap_, STR_TRIMMED_LOWER, EnumWrapper_t::Count);
        }

    private:
        static void ClearAndPopulateNameToValueMap()
        {
            nameToValueMap_.clear();

            for (EnumUnderlying_t index(0); index < EnumWrapper_t::Count; ++index)
            {
                const std::string ENUM_TRIMMED_LOWER_STRING { enum_helpers::TrimAndMakeLowerCase(
                    EnumWrapper_t::ToString(static_cast<typename EnumWrapper_t::Enum>(index))) };

                nameToValueMap_[ENUM_TRIMMED_LOWER_STRING] = index;
            }
        }

        static std::map<std::string, EnumUnderlying_t> nameToValueMap_;
    };

    template <typename EnumWrapper_t, EnumFirstValue FIRST_VALUE_TYPE>
    std::map<std::string, EnumUnderlying_t>
        EnumBaseCounting<EnumWrapper_t, FIRST_VALUE_TYPE>::nameToValueMap_;

    // Responsible for common operations of enums that are used as bit flags
    template <typename EnumWrapper_t>
    class EnumBaseBitField
    {
    public:
        EnumBaseBitField() = delete;

        static const std::string TypeName()
        {
            return boost::typeindex::type_id<EnumWrapper_t>().pretty_name() + "::Enum";
        }

        static const std::string UnderlyingTypeName()
        {
            return boost::typeindex::type_id<EnumUnderlying_t>().pretty_name();
        }

        // this may look redundant but callers can pass in the enum type (EnumWrapper::Enum)
        // and get the underlying type (EnumUnderlying_t) back
        static constexpr EnumUnderlying_t ToUnderlyingType(const EnumUnderlying_t ENUM_VALUE)
        {
            return ENUM_VALUE;
        }

        static constexpr bool IsValid(const EnumUnderlying_t ENUM_VALUE)
        {
            return (ENUM_VALUE <= LargestValidValue());
        }

        static constexpr bool IsValidAndNonZero(const EnumUnderlying_t ENUM_VALUE)
        {
            return ((ENUM_VALUE != 0) && IsValid(ENUM_VALUE));
        }

        static constexpr EnumUnderlying_t LargestValidValue()
        {
            if (0 == largestValidValue_)
            {
                largestValidValue_ = CalcLargestValidValue();
            }

            return largestValidValue_;
        }

        static const std::string
            ToString(const EnumUnderlying_t ENUM_VALUE, const EnumStringHow & HOW = EnumStringHow())
        {
            if (ENUM_VALUE == EnumWrapper_t::None)
            {
                if (HOW.will_none_return_empty == NoneEmpty::Yes)
                {
                    return "";
                }
                else
                {
                    if (HOW.will_wrap == Wrap::Yes)
                    {
                        return "(None)";
                    }
                    else
                    {
                        return "None";
                    }
                }
            }

            const std::string POPULATED_STR { EnumWrapper_t::ToStringPopulate(
                ENUM_VALUE, HOW.separator) };

            if ((HOW.will_wrap == Wrap::Yes) && !POPULATED_STR.empty())
            {
                return "(" + POPULATED_STR + ")";
            }
            else
            {
                return POPULATED_STR;
            }
        }

        static EnumUnderlying_t FromString(const std::string & STR)
        {
            // these are the words in the original STR that MIGHT correspond to bit flags of this
            // enum, the strings returned by this function have all been trimmed and made lower case
            const auto FROM_STR_SPLIT_VEC { enum_helpers::BitwiseEnumFromStringSplit(STR) };

            if (FROM_STR_SPLIT_VEC.empty())
            {
                return EnumWrapper_t::None;
            }

            if (nameToBitFlagMap_.empty())
            {
                ClearAndPopulateNameToBitFlagMap();
            }

            // loop over all words that might correspond to bit flags (FROM_STR_SPLIT_VEC) and set
            // all that match a flag (nameToBitFlagMap_)
            return enum_helpers::WordsToBitFlags(FROM_STR_SPLIT_VEC, nameToBitFlagMap_);
        }

        static constexpr std::size_t CountBitsSet(const EnumUnderlying_t ENUM_VALUE)
        {
            std::size_t count { 0 };
            EnumUnderlying_t flag { 1 };

            while (flag <= EnumWrapper_t::Last)
            {
                if (ENUM_VALUE & flag)
                {
                    ++count;
                }

                flag <<= 1;
            }

            return count;
        }

    protected:
        static void AppendNameIfBitIsSet(
            std::string & stringSoFar,
            const EnumUnderlying_t ENUM_VALUE,
            const EnumUnderlying_t BIT_TO_CHECK,
            const std::string & NAME,
            const std::string & SEPARATOR = ", ")
        {
            if (ENUM_VALUE & BIT_TO_CHECK)
            {
                if (stringSoFar.empty() == false)
                {
                    stringSoFar.append(SEPARATOR);
                }

                stringSoFar.append(NAME);
            }
        }

    private:
        static constexpr EnumUnderlying_t CalcLargestValidValue()
        {
            EnumUnderlying_t finalValue { 1 };
            EnumUnderlying_t shiftingValue { 1 };

            while (shiftingValue <= EnumWrapper_t::Last)
            {
                finalValue |= shiftingValue;
                shiftingValue <<= 1;
            }

            return finalValue;
        }

        static void ClearAndPopulateNameToBitFlagMap()
        {
            nameToBitFlagMap_.clear();

            EnumUnderlying_t flag { 1 };

            while (flag <= EnumWrapper_t::Last)
            {
                const std::string SINGLE_BIT_TO_STRING_ORIG { EnumWrapper_t::ToString(
                    static_cast<typename EnumWrapper_t::Enum>(flag),
                    EnumStringHow(Wrap::No, "", NoneEmpty::No)) };

                const std::string SINGLE_BIT_TO_STRING_TRIMMED_LOWER {
                    enum_helpers::TrimAndMakeLowerCase(SINGLE_BIT_TO_STRING_ORIG)
                };

                nameToBitFlagMap_[SINGLE_BIT_TO_STRING_TRIMMED_LOWER] = flag;

                flag <<= 1;
            }
        }

        static EnumUnderlying_t largestValidValue_;

        // all the words (trimmed and lower-case) that correspond to one bit position in this enum
        static std::map<std::string, EnumUnderlying_t> nameToBitFlagMap_;
    };

    template <typename EnumWrapper_t>
    EnumUnderlying_t EnumBaseBitField<EnumWrapper_t>::largestValidValue_ { 0 };

    template <typename EnumWrapper_t>
    std::map<std::string, EnumUnderlying_t> EnumBaseBitField<EnumWrapper_t>::nameToBitFlagMap_;

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
