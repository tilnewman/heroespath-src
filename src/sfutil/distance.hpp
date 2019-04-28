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
#include "sfutil/common.hpp"

#include <cmath>

namespace heroespath
{
namespace sfutil
{

    // returns the distance from position A to position B
    template <typename T1, typename T2>
    const T1 Distance(const sf::Vector2<T1> & A, const sf::Vector2<T2> & B) noexcept
    {
        const float DIFF_HORIZ { static_cast<float>(B.x) - static_cast<float>(A.x) };
        const float DIFF_VERT { static_cast<float>(B.y) - static_cast<float>(A.y) };
        return static_cast<T1>(std::sqrt((DIFF_HORIZ * DIFF_HORIZ) + (DIFF_VERT * DIFF_VERT)));
    }

    // returns the distance from the center of A to the center of B
    template <typename T1, typename T2>
    const T1 Distance(const sf::Rect<T1> & A, const sf::Rect<T2> & B) noexcept
    {
        return Distance(CenterOf(A), CenterOf(B));
    }

    // returns the distance from the center of A to the center of B (global)
    template <typename T>
    std::enable_if_t<has_getglobalbounds_v<T>, float> Distance(const T & A, const T & B)
    {
        return Distance(CenterOf(A.getGlobalBounds()), CenterOf(B.getGlobalBounds()));
    }

} // namespace sfutil
} // namespace heroespath

#endif // HEROESPATH_SFUTIL_DISTANCE_HPP_INCLUDED
