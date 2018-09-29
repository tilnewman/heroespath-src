// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFUTIL_VECTOR_MATH_HPP_INCLUDED
#define HEROESPATH_SFUTIL_VECTOR_MATH_HPP_INCLUDED
//
// vector-math.hpp
//
#include <SFML/System/Vector2.hpp>

#include <cmath>

namespace heroespath
{
namespace sfutil
{

    // returns the vector magnitude of V
    template <typename T>
    constexpr T Magnitude(const sf::Vector2<T> & V)
    {
        return static_cast<T>(std::sqrt(
            static_cast<double>(V.x) * static_cast<double>(V.x)
            + static_cast<double>(V.y) * static_cast<double>(V.y)));
    }

    // normalizes v
    template <typename T>
    constexpr void Normalize(sf::Vector2<T> & v)
    {
        auto const MAGNITUDE { Magnitude(sf::Vector2<double>(v)) };

        if (misc::IsRealZero(MAGNITUDE) == false)
        {
            v.x = static_cast<T>(static_cast<double>(v.x) / MAGNITUDE);
            v.y = static_cast<T>(static_cast<double>(v.y) / MAGNITUDE);
        }
    }

} // namespace sfutil
} // namespace heroespath

#endif // HEROESPATH_SFUTIL_VECTOR_MATH_HPP_INCLUDED
