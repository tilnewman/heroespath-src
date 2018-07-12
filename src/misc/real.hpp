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
#include <type_traits> // for enable_if

namespace heroespath
{
namespace misc
{

    template <class T>
    constexpr typename std::enable_if_t<std::is_signed<T>::value, T> ABS(const T X)
    {
        return ((X < 0) ? -X : X);
    }

    template <class T>
    constexpr typename std::enable_if_t<!std::is_signed<T>::value, T> ABS(const T X)
    {
        return X;
    }

    template <typename T>
    constexpr typename std::
        enable_if_t<(std::is_integral<T>::value && !std::is_same<T, bool>::value), bool>
        IsRealClose(const T A, const T B)
    {
        return (A == B);
    }

    template <typename T>
    constexpr typename std::enable_if_t<std::is_floating_point<T>::value, bool>
        IsRealClose(const T A, const T B)
    {
        auto const MAX_OR_ONE { std::max({ T(1), ABS(A), ABS(B) }) };
        return (ABS(A - B) < std::numeric_limits<T>::epsilon() * MAX_OR_ONE);
    }

    template <typename T>
    constexpr typename std::
        enable_if_t<(std::is_integral<T>::value && !std::is_same<T, bool>::value), bool>
        IsRealOne(const T X)
    {
        return (X == 1);
    }

    template <typename T>
    constexpr typename std::enable_if_t<std::is_floating_point<T>::value, bool> IsRealOne(const T X)
    {
        return IsRealClose(X, T(1));
    }

    template <typename T>
    constexpr typename std::
        enable_if_t<(std::is_integral<T>::value && !std::is_same<T, bool>::value), bool>
        IsRealZero(const T X)
    {
        return (X == 0);
    }

    template <typename T>
    constexpr typename std::enable_if_t<std::is_floating_point<T>::value, bool>
        IsRealZero(const T X)
    {
        return IsRealClose(X, T(0));
    }

} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_REALUTILS_HPP_INCLUDED
