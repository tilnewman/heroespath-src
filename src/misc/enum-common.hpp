// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_MISC_ENUM_COMMON_HPP_INCLUDED
#define HEROESPATH_MISC_ENUM_COMMON_HPP_INCLUDED
//
// enum-common.hpp
//
//  Note:  enum-common.hpp is meant to be included in *.hpp only, and enum-util.hpp is meant to be
//         included in *.cpp files only.  Breaking this rule will not break the build, but the whole
//         point of splitting enum functions into those two files was to speed build times and make
//         creating and including enums as light-weight as possible.  So while you CAN include
//         enum-util.hpp in a header file, it would defeat the design and slow down the build.
//
#include <cstddef> //for std::size_t
#include <string>
#include <type_traits> //for enable_if

namespace heroespath
{

using EnumUnderlying_t = std::size_t;

enum class Wrap : bool
{
    No = false,
    Yes = true
};

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

struct EnumCounting_t
{
    EnumCounting_t() = delete;
};

struct EnumBitField_t
{
    EnumBitField_t() = delete;
};

template <EnumFirstValue FIRST_VALUE_TYPE>
struct EnumBaseCounting
{
    using EnumBase_t = EnumCounting_t;
    static constexpr EnumFirstValue first_value_t = FIRST_VALUE_TYPE;
};

struct EnumBaseBitField
{
    using EnumBase_t = EnumBitField_t;

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

// see misc/enum-util.hpp
struct EnumStringHow;

template <typename T, typename = std::enable_if_t<std::is_enum_v<T>>>
inline constexpr T operator~(const T X)
{
    return static_cast<T>(~static_cast<EnumUnderlying_t>(X));
}

template <typename T, typename = std::enable_if_t<std::is_enum_v<T>>>
inline constexpr T operator|(const T L, const T R)
{
    return static_cast<T>(static_cast<EnumUnderlying_t>(L) | static_cast<EnumUnderlying_t>(R));
}

template <typename T, typename = std::enable_if_t<std::is_enum_v<T>>>
inline constexpr T operator&(const T L, const T R)
{
    return static_cast<T>(static_cast<EnumUnderlying_t>(L) & static_cast<EnumUnderlying_t>(R));
}

template <typename T, typename = std::enable_if_t<std::is_enum_v<T>>>
inline constexpr T operator^(const T L, const T R)
{
    return static_cast<T>(static_cast<EnumUnderlying_t>(L) ^ static_cast<EnumUnderlying_t>(R));
}

template <typename T, typename = std::enable_if_t<std::is_enum_v<T>>>
inline constexpr T & operator|=(T & l, const T R)
{
    l = static_cast<T>(static_cast<EnumUnderlying_t>(l) | static_cast<EnumUnderlying_t>(R));
    return l;
}

template <typename T, typename = std::enable_if_t<std::is_enum_v<T>>>
inline constexpr T & operator&=(T & l, const T R)
{
    l = static_cast<T>(static_cast<EnumUnderlying_t>(l) & static_cast<EnumUnderlying_t>(R));
    return l;
}

template <typename T, typename = std::enable_if_t<std::is_enum_v<T>>>
inline constexpr T & operator^=(T & l, const T R)
{
    l = static_cast<T>(static_cast<EnumUnderlying_t>(l) ^ static_cast<EnumUnderlying_t>(R));
    return l;
}

template <
    typename T1,
    typename T2,
    typename std::enable_if<
        (std::is_enum_v<T1> && (std::is_same_v<T1, T2> || std::is_same_v<T2, EnumUnderlying_t>))
            || (std::is_enum_v<
                    T2> && (std::is_same_v<T1, T2> || std::is_same_v<T1, EnumUnderlying_t>))
            || (std::is_same_v<T1, EnumUnderlying_t> && std::is_same_v<T2, EnumUnderlying_t>),
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
        (std::is_enum_v<T1> && (std::is_same_v<T1, T2> || std::is_same_v<T2, EnumUnderlying_t>))
            || (std::is_enum_v<
                    T2> && (std::is_same_v<T1, T2> || std::is_same_v<T1, EnumUnderlying_t>))
            || (std::is_same_v<T1, EnumUnderlying_t> && std::is_same_v<T2, EnumUnderlying_t>),
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
        (std::is_enum_v<T1> && (std::is_same_v<T1, T2> || std::is_same_v<T2, EnumUnderlying_t>))
            || (std::is_enum_v<
                    T2> && (std::is_same_v<T1, T2> || std::is_same_v<T1, EnumUnderlying_t>))
            || (std::is_same_v<T1, EnumUnderlying_t> && std::is_same_v<T2, EnumUnderlying_t>),
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
        (std::is_enum_v<T1> && (std::is_same_v<T1, T2> || std::is_same_v<T2, EnumUnderlying_t>))
            || (std::is_enum_v<
                    T2> && (std::is_same_v<T1, T2> || std::is_same_v<T1, EnumUnderlying_t>))
            || (std::is_same_v<T1, EnumUnderlying_t> && std::is_same_v<T2, EnumUnderlying_t>),
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
        (std::is_enum_v<T1> && (std::is_same_v<T1, T2> || std::is_same_v<T2, EnumUnderlying_t>))
            || (std::is_enum_v<
                    T2> && (std::is_same_v<T1, T2> || std::is_same_v<T1, EnumUnderlying_t>))
            || (std::is_same_v<T1, EnumUnderlying_t> && std::is_same_v<T2, EnumUnderlying_t>),
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
        (std::is_enum_v<T1> && (std::is_same_v<T1, T2> || std::is_same_v<T2, EnumUnderlying_t>))
            || (std::is_enum_v<
                    T2> && (std::is_same_v<T1, T2> || std::is_same_v<T1, EnumUnderlying_t>))
            || (std::is_same_v<T1, EnumUnderlying_t> && std::is_same_v<T2, EnumUnderlying_t>),
        int>::type
    = 0>
inline constexpr T1 BitToggleCopy(const T1 ORIG, const T2 BITS_TO_TOGGLE)
{
    T1 copy { ORIG };
    BitToggle(copy, BITS_TO_TOGGLE);
    return copy;
}

} // namespace heroespath

#endif // HEROESPATH_MISC_ENUM_COMMON_HPP_INCLUDED
