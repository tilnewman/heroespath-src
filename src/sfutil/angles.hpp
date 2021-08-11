// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFUTIL_ANGLES_HPP_INCLUDED
#define HEROESPATH_SFUTIL_ANGLES_HPP_INCLUDED
//
// angles.hpp
//
#include "misc/math-constants.hpp"
#include "sfutil/display.hpp"

#include <cmath>

#include <SFML/System/Vector2.hpp>

namespace heroespath
{
namespace sfutil
{

    // assumes 'facing right' on the typical screen where horiz pixels increase to the right and
    // vert pixels increase down
    template <typename T>
    constexpr T
        GetAngleInDegrees(const sf::Vector2<T> & BEGIN_POS_V, const sf::Vector2<T> & END_POS_V)
    {
        const auto ANGLE_RADIANS { std::atan(
            (std::max(BEGIN_POS_V.y, END_POS_V.y) - std::min(BEGIN_POS_V.y, END_POS_V.y))
            / (std::max(BEGIN_POS_V.x, END_POS_V.x) - std::min(BEGIN_POS_V.x, END_POS_V.x))) };

        const auto ANGLE_DEGREES { misc::math::RadiansToDegrees(ANGLE_RADIANS) };

        if (BEGIN_POS_V.x < END_POS_V.x)
        {
            if (BEGIN_POS_V.y > END_POS_V.y)
            {
                return ANGLE_DEGREES * T(-1); // QI
            }
            else
            {
                return ANGLE_DEGREES; // QIV
            }
        }
        else
        {
            if (BEGIN_POS_V.y > END_POS_V.y)
            {
                return (T(180) - ANGLE_DEGREES) * -T(-1); // QII
            }
            else
            {
                return (T(180) + ANGLE_DEGREES) * -T(-1); // QIII
            }
        }
    }

    // return either x will be 0 or SCREEN_SIZE_V.x, or y will be 0 or SCREEN_SIZE_V.y
    template <typename T>
    constexpr sf::Vector2<T> ProjectToScreenEdge(
        const sf::Vector2<T> & FIRST_POS_V,
        const sf::Vector2<T> & SECOND_POS_V,
        const sf::Vector2<T> & SCREEN_SIZE_V)
    {
        sf::Vector2<T> finalPosV { 0, 0 };

        if (misc::IsRealClose(FIRST_POS_V.x, SECOND_POS_V.x))
        {
            finalPosV.x = FIRST_POS_V.x;
            if (FIRST_POS_V.y < SECOND_POS_V.y)
            {
                finalPosV.y = SCREEN_SIZE_V.y;
            }
            else
            {
                finalPosV.y = T(0);
            }
        }
        else if (misc::IsRealClose(FIRST_POS_V.y, SECOND_POS_V.y))
        {
            finalPosV.y = FIRST_POS_V.y;
            if (FIRST_POS_V.x < SECOND_POS_V.x)
            {
                finalPosV.x = SCREEN_SIZE_V.x;
            }
            else
            {
                finalPosV.x = T(0);
            }
        }
        else
        {
            // use linear relations to find the edge-of-screen miss point using horiz/vert
            finalPosV.y = ((FIRST_POS_V.y < SECOND_POS_V.y) ? SCREEN_SIZE_V.y : T(0));

            const auto AV_HORIZ_OVER_VERT { (std::max(FIRST_POS_V.x, SECOND_POS_V.x)
                                             - std::min(FIRST_POS_V.x, SECOND_POS_V.x))
                                            / (std::max(FIRST_POS_V.y, SECOND_POS_V.y)
                                               - std::min(FIRST_POS_V.y, SECOND_POS_V.y)) };

            const auto AV_VERTICAL_EXTENT { (
                (SECOND_POS_V.y > FIRST_POS_V.y) ? (SCREEN_SIZE_V.y - FIRST_POS_V.y)
                                                 : FIRST_POS_V.y) };

            const auto AV_HORIZ_EXTENT { AV_HORIZ_OVER_VERT * AV_VERTICAL_EXTENT };

            if (SECOND_POS_V.x > FIRST_POS_V.x)
            {
                finalPosV.x = FIRST_POS_V.x + AV_HORIZ_EXTENT;
            }
            else
            {
                finalPosV.x = FIRST_POS_V.x - AV_HORIZ_EXTENT;
            }

            //...okay so that didn't work, so use vert/horiz instead
            if ((finalPosV.x < T(0)) || (finalPosV.x > SCREEN_SIZE_V.x))
            {
                finalPosV.x = ((FIRST_POS_V.x < SECOND_POS_V.x) ? SCREEN_SIZE_V.x : T(0));

                const auto AH_VERT_OVER_HORIZ { (std::max(FIRST_POS_V.y, SECOND_POS_V.y)
                                                 - std::min(FIRST_POS_V.y, SECOND_POS_V.y))
                                                / (std::max(FIRST_POS_V.x, SECOND_POS_V.x)
                                                   - std::min(FIRST_POS_V.x, SECOND_POS_V.x)) };

                const auto AH_HORIZ_EXTENT { (
                    (SECOND_POS_V.x > FIRST_POS_V.x) ? (SCREEN_SIZE_V.x - FIRST_POS_V.x)
                                                     : FIRST_POS_V.x) };

                const auto AH_VERT_EXTENT { AH_VERT_OVER_HORIZ * AH_HORIZ_EXTENT };

                if (SECOND_POS_V.y > FIRST_POS_V.y)
                {
                    finalPosV.y = FIRST_POS_V.y + AH_VERT_EXTENT;
                }
                else
                {
                    finalPosV.y = FIRST_POS_V.y - AH_VERT_EXTENT;
                }
            }
        }

        return finalPosV;
    }

    // return either x will be 0 or SCREEN_SIZE_V.x, or y will be 0 or SCREEN_SIZE_V.y
    template <typename T>
    constexpr sf::Vector2<T>
        ProjectToScreenEdge(const sf::Vector2<T> & FIRST_POS_V, const sf::Vector2<T> & SECOND_POS_V)
    {
        return ProjectToScreenEdge(FIRST_POS_V, SECOND_POS_V, sf::Vector2<T> { DisplaySize() });
    }

    // same as ProjectToScreenEdge() but result is nudged just off screen
    // the bigger PROJECTED_IMAGE_SIZE_V is the greater error in the angle
    template <typename T>
    constexpr sf::Vector2<T> ProjectJustPastScreenEdge(
        const sf::Vector2<T> & FIRST_POS_V,
        const sf::Vector2<T> & SECOND_POS_V,
        const sf::Vector2<T> & SCREEN_SIZE_V,
        const sf::Vector2<T> & PROJECTED_IMAGE_SIZE_V)
    {
        auto posV { ProjectToScreenEdge(FIRST_POS_V, SECOND_POS_V, SCREEN_SIZE_V) };

        if (misc::IsRealZero(posV.x))
        {
            posV.x -= PROJECTED_IMAGE_SIZE_V.x;
        }
        else if (misc::IsRealClose(posV.x, SCREEN_SIZE_V.x))
        {
            posV.x += PROJECTED_IMAGE_SIZE_V.x;
        }

        if (misc::IsRealZero(posV.y))
        {
            posV.y -= PROJECTED_IMAGE_SIZE_V.y;
        }
        else if (misc::IsRealClose(posV.y, SCREEN_SIZE_V.y))
        {
            posV.y += PROJECTED_IMAGE_SIZE_V.y;
        }

        return posV;
    }

    // same as ProjectToScreenEdge() but result is nudged just off screen
    // the bigger PROJECTED_IMAGE_SIZE_V is the greater error in the angle
    template <typename T>
    constexpr sf::Vector2<T> ProjectJustPastScreenEdge(
        const sf::Vector2<T> & FIRST_POS_V,
        const sf::Vector2<T> & SECOND_POS_V,
        const sf::Vector2<T> & PROJECTED_IMAGE_SIZE_V)
    {
        return ProjectJustPastScreenEdge(
            FIRST_POS_V, SECOND_POS_V, sf::Vector2<T> { DisplaySize() }, PROJECTED_IMAGE_SIZE_V);
    }

    // same as ProjectToScreenEdge() but result is nudged back on screen of not there already
    // the bigger PROJECTED_IMAGE_SIZE_V is the greater error in the angle
    template <typename T>
    constexpr sf::Vector2<T> ProjectJustWithinScreenEdge(
        const sf::Vector2<T> & FIRST_POS_V,
        const sf::Vector2<T> & SECOND_POS_V,
        const sf::Vector2<T> & SCREEN_SIZE_V,
        const sf::Vector2<T> & PROJECTED_IMAGE_SIZE_V)
    {
        auto posV { ProjectToScreenEdge(FIRST_POS_V, SECOND_POS_V, SCREEN_SIZE_V) };

        if ((posV.x + PROJECTED_IMAGE_SIZE_V.x) > SCREEN_SIZE_V.x)
        {
            posV.x = (SCREEN_SIZE_V.x - PROJECTED_IMAGE_SIZE_V.x);
        }

        if ((posV.y + PROJECTED_IMAGE_SIZE_V.y) > SCREEN_SIZE_V.y)
        {
            posV.y = (SCREEN_SIZE_V.y - PROJECTED_IMAGE_SIZE_V.y);
        }

        return posV;
    }

    // same as ProjectToScreenEdge() but result is nudged back on screen of not there already
    // the bigger PROJECTED_IMAGE_SIZE_V is the greater error in the angle
    template <typename T>
    constexpr sf::Vector2<T> ProjectJustWithinScreenEdge(
        const sf::Vector2<T> & FIRST_POS_V,
        const sf::Vector2<T> & SECOND_POS_V,
        const sf::Vector2<T> & PROJECTED_IMAGE_SIZE_V)
    {
        return ProjectJustWithinScreenEdge(
            FIRST_POS_V, SECOND_POS_V, sf::Vector2<T> { DisplaySize() }, PROJECTED_IMAGE_SIZE_V);
    }

} // namespace sfutil
} // namespace heroespath

#endif // HEROESPATH_SFUTIL_ANGLES_HPP_INCLUDED
