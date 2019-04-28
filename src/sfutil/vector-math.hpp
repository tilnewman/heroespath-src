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

    template <typename T>
    T Magnitude(const sf::Vector2<T> & V) noexcept
    {
        return static_cast<T>(std::sqrt((V.x * V.x) + (V.y * V.y)));
    }

    template <typename T>
    const sf::Vector2<T> NormalizeCopy(const sf::Vector2<T> & V) noexcept
    {
        const T MAGNITUDE { Magnitude(V) };

        if (misc::IsRealZero(MAGNITUDE))
        {
            return V;
        }
        else
        {
            return sf::Vector2<T>((V.x / MAGNITUDE), (V.y / MAGNITUDE));
        }
    }

    template <typename T>
    void Normalize(sf::Vector2<T> & v)
    {
        v = NormalizeCopy(v);
    }

} // namespace sfutil
} // namespace heroespath

#endif // HEROESPATH_SFUTIL_VECTOR_MATH_HPP_INCLUDED
