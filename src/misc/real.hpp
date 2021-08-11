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
#include <algorithm>
#include <cmath> //for std::fabs
#include <limits> //for epsilon

#include "misc/compile-assert.hpp"
#include "misc/type-helpers.hpp"

namespace heroespath
{
namespace misc
{

    template <typename T>
    constexpr T ConstexprAbs(const T X) noexcept
    {
        if constexpr (std::is_signed_v<T>)
        {
            return ((X < T(0)) ? -X : X);
        }
        else
        {
            return X;
        }
    }

    template <typename T>
    constexpr bool IsRealClose(const T A, const T B) noexcept
    {
        if constexpr (are_integral_nobool_v<T>)
        {
            return (A == B);
        }
        else if constexpr (are_floating_point_v<T>)
        {
            const auto MAX_OR_ONE { std::max({ T(1), ConstexprAbs(A), ConstexprAbs(B) }) };
            return (ConstexprAbs(A - B) < (std::numeric_limits<T>::epsilon() * MAX_OR_ONE));
        }
        else
        {
            M_HP_COMPILE_ERROR_INVALID_TYPES(T);
        }
    }

    template <typename T>
    constexpr bool IsRealOne(const T X) noexcept
    {
        return IsRealClose(X, T(1));
    }

    template <typename T>
    constexpr bool IsRealZero(const T X) noexcept
    {
        return IsRealClose(X, T(0));
    }

    template <typename T>
    constexpr bool IsRealZeroOrLess(const T X) noexcept
    {
        return ((X < T(0)) || IsRealZero(X));
    }

    template <typename T>
    constexpr bool IsRealZeroOrMore(const T X) noexcept
    {
        return ((X > T(0)) || IsRealZero(X));
    }

    template <typename T>
    T NextAfterIfReal(const T X)
    {
        if constexpr (are_floating_point_v<T>)
        {
            return std::nextafter(X, std::numeric_limits<T>::max());
        }
        else
        {
            return X;
        }
    }

} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_REALUTILS_HPP_INCLUDED
