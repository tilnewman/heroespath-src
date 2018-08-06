// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_SFMLUTIL_DIRECTION_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_SFMLUTIL_DIRECTION_HPP_INCLUDED
//
// sfml-util-direction.hpp
//
#include "misc/real.hpp"
#include "sfml-util/direction-enum.hpp"
#include "sfml-util/sfml-util-center.hpp"

#include <SFML/Graphics/Rect.hpp>

namespace sf
{
class Sprite;
}

namespace heroespath
{
namespace sfml_util
{

    // returns the screen coordinate direction that B is from A, returns DEFAULT if A and B are
    // equal
    template <typename T1, typename T2>
    constexpr sfml_util::Direction::Enum DirectionFromAToB(
        const sf::Vector2<T1> & A,
        const sf::Vector2<T2> & B,
        const sfml_util::Direction::Enum DEFAULT = sfml_util::Direction::Count)
    {
        auto const A_X { static_cast<double>(A.x) };
        auto const A_Y { static_cast<double>(A.y) };
        auto const B_X { static_cast<double>(B.x) };
        auto const B_Y { static_cast<double>(B.y) };

        auto const DIFF_X_ABS { misc::ABS(B_X - A_X) };
        auto const DIFF_Y_ABS { misc::ABS(B_Y - A_Y) };

        if (DIFF_X_ABS > DIFF_Y_ABS)
        {
            if (B_X < A_X)
            {
                return sfml_util::Direction::Left;
            }
            else
            {
                return sfml_util::Direction::Right;
            }
        }
        else if (DIFF_Y_ABS > DIFF_X_ABS)
        {
            if (B_Y < A_Y)
            {
                return sfml_util::Direction::Up;
            }
            else
            {
                return sfml_util::Direction::Down;
            }
        }
        else
        {
            if (B_X < A_X)
            {
                return sfml_util::Direction::Left;
            }
            else if (A_X < B_X)
            {
                return sfml_util::Direction::Right;
            }
            else
            {
                return DEFAULT;
            }
        }
    }

    // returns the screen coordinate direction that the center of B is from the center of A, returns
    // DEFAULT if A and B are equal
    template <typename T1, typename T2>
    constexpr sfml_util::Direction::Enum DirectionFromAToB(
        const sf::Rect<T1> & A,
        const sf::Rect<T2> & B,
        const sfml_util::Direction::Enum DEFAULT = sfml_util::Direction::Count)
    {
        return DirectionFromAToB(CenterOf(A), CenterOf(B), DEFAULT);
    }

    // returns the screen coordinate direction that the global center of B is from the global center
    // of A, returns DEFAULT if A and B are equal
    sfml_util::Direction::Enum DirectionFromAToB(
        const sf::Sprite & A,
        const sf::Sprite & B,
        const sfml_util::Direction::Enum DEFAULT = sfml_util::Direction::Count);

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_SFMLUTIL_DIRECTION_HPP_INCLUDED
