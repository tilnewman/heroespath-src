// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFUTIL_DISTANCE_HPP_INCLUDED
#define HEROESPATH_SFUTIL_DISTANCE_HPP_INCLUDED
//
// distance.hpp
//
#include "sfutil/center.hpp"

#include <cmath>

namespace sf
{
class Sprite;
}

namespace heroespath
{
namespace sfutil
{

    // returns the distance from position A to position B
    template <typename T1, typename T2>
    constexpr T1 Distance(const sf::Vector2<T1> & A, const sf::Vector2<T2> & B)
    {
        const auto DIST_HORIZ { static_cast<double>(B.x) - static_cast<double>(A.x) };
        const auto DIST_VERT { static_cast<double>(B.y) - static_cast<double>(A.y) };

        return static_cast<T1>(std::sqrt((DIST_HORIZ * DIST_HORIZ) + (DIST_VERT * DIST_VERT)));
    }

    // returns the distance from the center of A to the center of B
    template <typename T1, typename T2>
    constexpr T1 Distance(const sf::Rect<T1> & A, const sf::Rect<T2> & B)
    {
        return Distance(CenterOf(A), CenterOf(B));
    }

    // returns the distance from the center of A to the center of B (global)
    float Distance(const sf::Sprite & A, const sf::Sprite & B);

} // namespace sfutil
} // namespace heroespath

#endif // HEROESPATH_SFUTIL_DISTANCE_HPP_INCLUDED
