// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_MISC_REALUTILS_HPP_INCLUDED
#define HEROESPATH_MISC_REALUTILS_HPP_INCLUDED
//
// real-utils.hpp
//
#include <cmath> //for nextafter()/nexttoward()
#include <limits> //for epsilon

#include "misc/type-helpers.hpp"

namespace heroespath
{
namespace misc
{

    template <typename T>
    constexpr std::enable_if_t<are_arithmetic_signed_nobool_v<T>, T> Abs(const T X) noexcept
    {
        return ((X < T(0)) ? -X : X);
    }

    template <typename T>
    constexpr std::enable_if_t<are_arithmetic_unsigned_nobool_v<T>, T> Abs(const T X) noexcept
    {
        return X;
    }

    template <typename T>
    constexpr std::enable_if_t<are_arithmetic_nobool_v<T>, T> Max(const T A, const T B) noexcept
    {
        return ((B < A) ? A : B);
    }

    template <typename T, typename... Ts>
    constexpr std::enable_if_t<are_arithmetic_nobool_v<T>, T>
        Max(const T FIRST, const Ts... OTHERS) noexcept
    {
        return Max(FIRST, Max(OTHERS...));
    }

    template <typename T>
    constexpr std::enable_if_t<are_arithmetic_nobool_v<T>, T> Min(const T A, const T B) noexcept
    {
        return ((A < B) ? A : B);
    }

    template <typename T, typename... Ts>
    constexpr std::enable_if_t<are_arithmetic_nobool_v<T>, T>
        Min(const T FIRST, const Ts... OTHERS) noexcept
    {
        return Min(FIRST, Min(OTHERS...));
    }

    template <typename T>
    constexpr std::enable_if_t<are_floating_point_v<T>, T> ZERO_MINUS_EPSILON
        = (T(0) - std::numeric_limits<T>::epsilon());

    template <typename T>
    constexpr std::enable_if_t<are_floating_point_v<T>, T> ZERO_PLUS_EPSILON
        = (T(0) + std::numeric_limits<T>::epsilon());

    template <typename T>
    constexpr std::enable_if_t<are_floating_point_v<T>, T> ONE_MINUS_EPSILON
        = (T(1) - std::numeric_limits<T>::epsilon());

    template <typename T>
    constexpr std::enable_if_t<are_floating_point_v<T>, T> ONE_PLUS_EPSILON
        = (T(1) + std::numeric_limits<T>::epsilon());

    template <typename T>
    constexpr std::enable_if_t<are_floating_point_v<T>, bool>
        IsRealClose(const T A, const T B) noexcept
    {
        return (Abs(A - B) < (std::numeric_limits<T>::epsilon() * Max(Abs(A), Abs(B), T(1))));
    }

    template <typename T>
    constexpr std::enable_if_t<are_integral_nobool_v<T>, bool>
        IsRealClose(const T A, const T B) noexcept
    {
        return (A == B);
    }

    template <typename T>
    constexpr std::enable_if_t<are_floating_point_v<T>, bool>
        IsRealLessOrClose(const T A, const T B) noexcept
    {
        return ((A < B) || IsRealClose(A, B));
    }

    template <typename T>
    constexpr std::enable_if_t<are_integral_nobool_v<T>, bool>
        IsRealLessOrClose(const T A, const T B) noexcept
    {
        return (A <= B);
    }

    template <typename T>
    constexpr std::enable_if_t<are_floating_point_v<T>, bool>
        IsRealGreaterOrClose(const T A, const T B) noexcept
    {
        return ((B < A) || IsRealClose(A, B));
    }

    template <typename T>
    constexpr std::enable_if_t<are_integral_nobool_v<T>, bool>
        IsRealGreaterOrClose(const T A, const T B) noexcept
    {
        return (A >= B);
    }

    template <typename T>
    constexpr std::enable_if_t<are_floating_point_v<T>, bool> IsRealZero(const T X) noexcept
    {
        return !((X < ZERO_MINUS_EPSILON<T>) || (X > ZERO_PLUS_EPSILON<T>));
    }

    template <typename T>
    constexpr std::enable_if_t<are_integral_nobool_v<T>, bool> IsRealZero(const T X) noexcept
    {
        return (X == T(0));
    }

    template <typename T>
    constexpr std::enable_if_t<are_floating_point_v<T>, bool> IsRealZeroOrLess(const T X) noexcept
    {
        return !(X > ZERO_PLUS_EPSILON<T>);
    }

    template <typename T>
    constexpr std::enable_if_t<are_integral_nobool_v<T>, bool> IsRealZeroOrLess(const T X) noexcept
    {
        return (X <= T(0));
    }

    template <typename T>
    constexpr std::enable_if_t<are_floating_point_v<T>, bool> IsRealZeroOrMore(const T X) noexcept
    {
        return !(X < ZERO_MINUS_EPSILON<T>);
    }

    template <typename T>
    constexpr std::enable_if_t<are_integral_signed_nobool_v<T>, bool>
        IsRealZeroOrMore(const T X) noexcept
    {
        return (X >= T(0));
    }

    template <typename T>
    constexpr std::enable_if_t<are_integral_unsigned_nobool_v<T>, bool>
        IsRealZeroOrMore(const T) noexcept
    {
        return true;
    }

    template <typename T>
    constexpr std::enable_if_t<are_floating_point_v<T>, bool> IsRealOne(const T X) noexcept
    {
        return !((X < ONE_MINUS_EPSILON<T>) || (X > ONE_PLUS_EPSILON<T>));
    }

    template <typename T>
    constexpr std::enable_if_t<are_integral_nobool_v<T>, bool> IsRealOne(const T X) noexcept
    {
        return (X == T(1));
    }

    template <typename T>
    constexpr std::enable_if_t<are_floating_point_v<T>, T> NextIfReal(const T X)
    {
        return (
            (X < std::numeric_limits<T>::max()) ? std::nextafter(X, std::numeric_limits<T>::max())
                                                : X);
    }

    template <typename T>
    constexpr std::enable_if_t<are_integral_nobool_v<T>, T> NextIfReal(const T X)
    {
        return X;
    }

    template <typename T>
    constexpr std::enable_if_t<are_floating_point_v<T>, T> BeforeIfReal(const T X)
    {
        return (
            (X > std::numeric_limits<T>::lowest())
                ? std::nexttoward(X, std::numeric_limits<long double>::lowest())
                : X);
    }

    template <typename T>
    constexpr std::enable_if_t<are_integral_nobool_v<T>, T> BeforeIfReal(const T X)
    {
        return X;
    }

} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_REALUTILS_HPP_INCLUDED
