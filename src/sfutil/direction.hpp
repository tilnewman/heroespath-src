// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFUTIL_DIRECTION_HPP_INCLUDED
#define HEROESPATH_SFUTIL_DIRECTION_HPP_INCLUDED
//
// direction.hpp
//
#include "misc/real.hpp"
#include "sfml-util/direction-enum.hpp"
#include "sfutil/center.hpp"

#include <SFML/Graphics/Rect.hpp>

namespace sf
{
class Sprite;
}

namespace heroespath
{
namespace sfutil
{

    // returns the screen coordinate direction that B is from A, returns DEFAULT if A and B are
    // equal
    template <typename T1, typename T2>
    constexpr gui::Direction::Enum DirectionFromAToB(
        const sf::Vector2<T1> & A,
        const sf::Vector2<T2> & B,
        const gui::Direction::Enum DEFAULT = gui::Direction::Count)
    {
        const auto A_X { static_cast<double>(A.x) };
        const auto A_Y { static_cast<double>(A.y) };
        const auto B_X { static_cast<double>(B.x) };
        const auto B_Y { static_cast<double>(B.y) };

        const auto DIFF_X_ABS { misc::ABS(B_X - A_X) };
        const auto DIFF_Y_ABS { misc::ABS(B_Y - A_Y) };

        if (DIFF_X_ABS > DIFF_Y_ABS)
        {
            if (B_X < A_X)
            {
                return gui::Direction::Left;
            }
            else
            {
                return gui::Direction::Right;
            }
        }
        else if (DIFF_Y_ABS > DIFF_X_ABS)
        {
            if (B_Y < A_Y)
            {
                return gui::Direction::Up;
            }
            else
            {
                return gui::Direction::Down;
            }
        }
        else
        {
            if (B_X < A_X)
            {
                return gui::Direction::Left;
            }
            else if (A_X < B_X)
            {
                return gui::Direction::Right;
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
    constexpr gui::Direction::Enum DirectionFromAToB(
        const sf::Rect<T1> & A,
        const sf::Rect<T2> & B,
        const gui::Direction::Enum DEFAULT = gui::Direction::Count)
    {
        return DirectionFromAToB(CenterOf(A), CenterOf(B), DEFAULT);
    }

    // returns the screen coordinate direction that the global center of B is from the global center
    // of A, returns DEFAULT if A and B are equal
    gui::Direction::Enum DirectionFromAToB(
        const sf::Sprite & A,
        const sf::Sprite & B,
        const gui::Direction::Enum DEFAULT = gui::Direction::Count);

} // namespace sfutil
} // namespace heroespath

#endif // HEROESPATH_SFUTIL_DIRECTION_HPP_INCLUDED
