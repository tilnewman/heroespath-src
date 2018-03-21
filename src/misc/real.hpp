///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
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
    constexpr T abs(const T X)
    {
        return ((X < 0) ? -X : X);
    }

    template <typename T>
    constexpr typename std::enable_if<std::is_integral<T>::value, bool>::type
        IsRealClose(const T A, const T B)
    {
        return (A == B);
    }

    template <typename T>
    constexpr typename std::enable_if<!std::is_integral<T>::value, bool>::type
        IsRealClose(const T A, const T B)
    {
        auto const MAX_OR_ONE{ std::max({ T(1), abs(A), abs(B) }) };
        return (abs(A - B) < std::numeric_limits<T>::epsilon() * MAX_OR_ONE);
    }

    template <typename T>
    constexpr typename std::enable_if<std::is_integral<T>::value, bool>::type IsRealOne(const T X)
    {
        return (X == 1);
    }

    template <typename T>
    constexpr typename std::enable_if<!std::is_integral<T>::value, bool>::type IsRealOne(const T X)
    {
        return IsRealClose(X, T(1));
    }

    template <typename T>
    constexpr typename std::enable_if<std::is_integral<T>::value, bool>::type IsRealZero(const T X)
    {
        return (X == 0);
    }

    template <typename T>
    constexpr typename std::enable_if<!std::is_integral<T>::value, bool>::type IsRealZero(const T X)
    {
        return IsRealClose(X, T(0));
    }

} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_REALUTILS_HPP_INCLUDED
