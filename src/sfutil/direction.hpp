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
#include "gui/direction-enum.hpp"
#include "misc/real.hpp"
#include "sfutil/center.hpp"

#include <SFML/Graphics/Rect.hpp>

namespace heroespath
{
namespace sfutil
{

    namespace helpers
    {

        template <typename T1, typename T2>
        gui::Direction::Enum DirectionFromAToBImpl(
            const sf::Vector2<T1> & A,
            const sf::Vector2<T2> & B,
            const gui::Direction::Enum DEFAULT = gui::Direction::Count)
        {
            const T1 A_X { A.x };
            const T1 A_Y { A.y };
            const T1 B_X { static_cast<T1>(B.x) };
            const T1 B_Y { static_cast<T1>(B.y) };

            const T1 ABS_DIFF_X { misc::Abs(B_X - A_X) };
            const T1 ABS_DIFF_Y { misc::Abs(B_Y - A_Y) };

            if (ABS_DIFF_X > ABS_DIFF_Y)
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
            else if (ABS_DIFF_Y > ABS_DIFF_X)
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
    } // namespace helpers

    // screen coordinate direction that B is from A, returns DEFAULT if A and B are equal
    template <typename T1, typename T2>
    gui::Direction::Enum DirectionFromAToB(
        const sf::Vector2<T1> & A,
        const sf::Vector2<T2> & B,
        const gui::Direction::Enum DEFAULT = gui::Direction::Count)
    {
        if constexpr (misc::are_floating_point_v<T1> || misc::are_floating_point_v<T2>)
        {
            return helpers::DirectionFromAToBImpl(sf::Vector2f { A }, sf::Vector2f { B }, DEFAULT);
        }
        else
        {
            return helpers::DirectionFromAToBImpl(
                sf::Vector2<long long> { A }, sf::Vector2<long long> { B }, DEFAULT);
        }
    }

    // returns the screen coordinate direction that the center of B is from the center of A, returns
    // DEFAULT if A and B are equal
    template <typename T1, typename T2>
    gui::Direction::Enum DirectionFromAToB(
        const sf::Rect<T1> & A,
        const sf::Rect<T2> & B,
        const gui::Direction::Enum DEFAULT = gui::Direction::Count)
    {
        return DirectionFromAToB(CenterOf(A), CenterOf(B), DEFAULT);
    }

    // returns the screen coordinate direction that the global center of B is from the global center
    // of A, returns DEFAULT if A and B are equal
    template <typename T>
    std::enable_if_t<has_getglobalbounds_v<T>, gui::Direction::Enum> DirectionFromAToB(
        const T & A, const T & B, const gui::Direction::Enum DEFAULT = gui::Direction::Count)
    {
        return DirectionFromAToB(
            CenterOf(A.getGlobalBounds()), CenterOf(B.getGlobalBounds()), DEFAULT);
    }

} // namespace sfutil
} // namespace heroespath

#endif // HEROESPATH_SFUTIL_DIRECTION_HPP_INCLUDED
