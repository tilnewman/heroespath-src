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

    // the sfutil library:
    // always returns the type of the first parameter

    // assumes 'facing right' on the typical screen where horiz pixels increase to the right and
    // vert pixels increase down, all math done as float
    template <typename T>
    T GetAngleInDegrees(
        const sf::Vector2<T> & BEGIN_POS_V, const sf::Vector2<T> & END_POS_V) noexcept
    {
        const float BEGIN_POS_X { static_cast<float>(BEGIN_POS_V.x) };
        const float BEGIN_POS_Y { static_cast<float>(BEGIN_POS_V.y) };

        const float END_POS_X { static_cast<float>(END_POS_V.x) };
        const float END_POS_Y { static_cast<float>(END_POS_V.y) };

        const float ANGLE_RADIANS { std::atan(
            (misc::Max(BEGIN_POS_Y, END_POS_Y) - misc::Min(BEGIN_POS_Y, END_POS_Y))
            / (misc::Max(BEGIN_POS_X, END_POS_X) - misc::Min(BEGIN_POS_X, END_POS_X))) };

        const float ANGLE_DEGREES { misc::math::RadiansToDegrees(ANGLE_RADIANS) };

        if (BEGIN_POS_X < END_POS_X)
        {
            if (BEGIN_POS_Y > END_POS_Y)
            {
                return ANGLE_DEGREES * -1.0f; // QI
            }
            else
            {
                return ANGLE_DEGREES; // QIV
            }
        }
        else
        {
            if (BEGIN_POS_Y > END_POS_Y)
            {
                return (180.0f - ANGLE_DEGREES) * -1.0f; // QII
            }
            else
            {
                return (180.0f + ANGLE_DEGREES) * -1.0f; // QIII
            }
        }
    }

    // return either x will be 0 or SCREEN_SIZE_V.x, or y will be 0 or SCREEN_SIZE_V.y, all math as
    // float
    template <typename T>
    const sf::Vector2<T> ProjectToScreenEdge(
        const sf::Vector2<T> & FIRST_POS_V,
        const sf::Vector2<T> & SECOND_POS_V,
        const sf::Vector2<T> & SCREEN_SIZE_V) noexcept
    {
        float finalPosX { 0.0f };
        float finalPosY { 0.0f };

        const float FIRST_POS_X { static_cast<float>(FIRST_POS_V.x) };
        const float FIRST_POS_Y { static_cast<float>(FIRST_POS_V.y) };

        const float SECOND_POS_X { static_cast<float>(SECOND_POS_V.x) };
        const float SECOND_POS_Y { static_cast<float>(SECOND_POS_V.y) };

        const float SCREEN_SIZE_X { static_cast<float>(SCREEN_SIZE_V.x) };
        const float SCREEN_SIZE_Y { static_cast<float>(SCREEN_SIZE_V.y) };

        const bool IS_FIRST_X_LESS_SECOND_X { FIRST_POS_X < SECOND_POS_X };
        const bool IS_FIRST_Y_LESS_SECOND_Y { FIRST_POS_Y < SECOND_POS_Y };

        if (misc::IsRealClose(FIRST_POS_X, SECOND_POS_X))
        {
            finalPosX = FIRST_POS_X;

            if (IS_FIRST_Y_LESS_SECOND_Y)
            {
                finalPosY = SECOND_POS_Y;
            }
            else
            {
                finalPosY = 0.0f;
            }
        }
        else if (misc::IsRealClose(FIRST_POS_Y, SECOND_POS_Y))
        {
            finalPosY = static_cast<float>(FIRST_POS_Y);

            if (IS_FIRST_X_LESS_SECOND_X)
            {
                finalPosX = SECOND_POS_X;
            }
            else
            {
                finalPosX = 0.0f;
            }
        }
        else
        {
            // use linear relations to find the edge-of-screen miss point using horiz/vert
            finalPosY = ((IS_FIRST_Y_LESS_SECOND_Y) ? SCREEN_SIZE_Y : 0.0f);

            const float MAX_MINUS_MIN_POS_X { (
                misc::Max(FIRST_POS_X, SECOND_POS_X) - misc::Min(FIRST_POS_X, SECOND_POS_X)) };

            const float MAX_MINUS_MIN_POS_Y { (
                misc::Max(FIRST_POS_Y, SECOND_POS_Y) - misc::Min(FIRST_POS_Y, SECOND_POS_Y)) };

            const auto AV_HORIZ_OVER_VERT { MAX_MINUS_MIN_POS_X / MAX_MINUS_MIN_POS_Y };

            const auto AV_VERTICAL_EXTENT { (
                (IS_FIRST_Y_LESS_SECOND_Y) ? (SCREEN_SIZE_Y - FIRST_POS_Y) : FIRST_POS_Y) };

            const auto AV_HORIZ_EXTENT { AV_HORIZ_OVER_VERT * AV_VERTICAL_EXTENT };

            if (IS_FIRST_X_LESS_SECOND_X)
            {
                finalPosX = (FIRST_POS_X + AV_HORIZ_EXTENT);
            }
            else
            {
                finalPosX = (FIRST_POS_X - AV_HORIZ_EXTENT);
            }

            //...okay so that didn't work, so use vert/horiz instead
            if ((finalPosX < 0.0f) || (finalPosX > SCREEN_SIZE_X))
            {
                finalPosX = ((IS_FIRST_X_LESS_SECOND_X) ? SCREEN_SIZE_X : 0.0f);

                const auto AH_VERT_OVER_HORIZ { MAX_MINUS_MIN_POS_Y / MAX_MINUS_MIN_POS_X };

                const auto AH_HORIZ_EXTENT { (
                    (IS_FIRST_X_LESS_SECOND_X) ? (SCREEN_SIZE_X - FIRST_POS_X) : FIRST_POS_X) };

                const auto AH_VERT_EXTENT { AH_VERT_OVER_HORIZ * AH_HORIZ_EXTENT };

                if (IS_FIRST_Y_LESS_SECOND_Y)
                {
                    finalPosY = (FIRST_POS_Y + AH_VERT_EXTENT);
                }
                else
                {
                    finalPosY = (FIRST_POS_Y - AH_VERT_EXTENT);
                }
            }
        }

        return sf::Vector2<T>(static_cast<T>(finalPosX), static_cast<T>(finalPosY));
    }

    template <typename T>
    const typename std::enable_if_t<!misc::are_signed_v<T>, sf::Vector2<T>> ProjectToScreenEdge(
        const sf::Vector2<T> & FIRST_POS_V,
        const sf::Vector2<T> & SECOND_POS_V,
        const sf::Vector2<T> & SCREEN_SIZE_V) noexcept
    {
        return sf::Vector2<T>(ProjectToScreenEdge(
            sf::Vector2f(FIRST_POS_V), sf::Vector2f(SECOND_POS_V), sf::Vector2f(SCREEN_SIZE_V)));
    }

    // return either x will be 0 or SCREEN_SIZE_V.x, or y will be 0 or SCREEN_SIZE_V.y
    template <typename T>
    const sf::Vector2<T>
        ProjectToScreenEdge(const sf::Vector2<T> & FIRST_POS_V, const sf::Vector2<T> & SECOND_POS_V)
    {
        return ProjectToScreenEdge(FIRST_POS_V, SECOND_POS_V, sf::Vector2<T>(DisplaySize()));
    }

    // same as ProjectToScreenEdge() but result is nudged just off screen
    // the bigger PROJECTED_IMAGE_SIZE_V is the greater error in the angle
    template <typename T>
    const typename std::enable_if_t<misc::are_signed_v<T>, sf::Vector2<T>>
        ProjectJustPastScreenEdge(
            const sf::Vector2<T> & FIRST_POS_V,
            const sf::Vector2<T> & SECOND_POS_V,
            const sf::Vector2<T> & SCREEN_SIZE_V,
            const sf::Vector2<T> & PROJECTED_IMAGE_SIZE_V) noexcept
    {
        auto posV = ProjectToScreenEdge(FIRST_POS_V, SECOND_POS_V, SCREEN_SIZE_V);

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

    template <typename T>
    const typename std::enable_if_t<!misc::are_signed_v<T>, sf::Vector2<T>>
        ProjectJustPastScreenEdge(
            const sf::Vector2<T> & FIRST_POS_V,
            const sf::Vector2<T> & SECOND_POS_V,
            const sf::Vector2<T> & SCREEN_SIZE_V,
            const sf::Vector2<T> & PROJECTED_IMAGE_SIZE_V) noexcept
    {
        return sf::Vector2<T>(ProjectJustPastScreenEdge(
            sf::Vector2f(FIRST_POS_V),
            sf::Vector2f(SECOND_POS_V),
            sf::Vector2f(SCREEN_SIZE_V),
            sf::Vector2f(PROJECTED_IMAGE_SIZE_V)));
    }

    // same as ProjectToScreenEdge() but result is nudged just off screen
    // the bigger PROJECTED_IMAGE_SIZE_V is the greater error in the angle
    template <typename T>
    const sf::Vector2<T> ProjectJustPastScreenEdge(
        const sf::Vector2<T> & FIRST_POS_V,
        const sf::Vector2<T> & SECOND_POS_V,
        const sf::Vector2<T> & PROJECTED_IMAGE_SIZE_V)
    {
        return ProjectJustPastScreenEdge(
            FIRST_POS_V, SECOND_POS_V, sf::Vector2<T>(DisplaySize()), PROJECTED_IMAGE_SIZE_V);
    }

    // same as ProjectToScreenEdge() but result is nudged back on screen of not there already
    // the bigger PROJECTED_IMAGE_SIZE_V is the greater error in the angle
    template <typename T>
    const typename std::enable_if_t<misc::are_signed_v<T>, sf::Vector2<T>>
        ProjectJustWithinScreenEdge(
            const sf::Vector2<T> & FIRST_POS_V,
            const sf::Vector2<T> & SECOND_POS_V,
            const sf::Vector2<T> & SCREEN_SIZE_V,
            const sf::Vector2<T> & PROJECTED_IMAGE_SIZE_V) noexcept
    {
        auto posV = ProjectToScreenEdge(FIRST_POS_V, SECOND_POS_V, SCREEN_SIZE_V);

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

    template <typename T>
    const typename std::enable_if_t<!misc::are_signed_v<T>, sf::Vector2<T>>
        ProjectJustWithinScreenEdge(
            const sf::Vector2<T> & FIRST_POS_V,
            const sf::Vector2<T> & SECOND_POS_V,
            const sf::Vector2<T> & SCREEN_SIZE_V,
            const sf::Vector2<T> & PROJECTED_IMAGE_SIZE_V) noexcept
    {
        return sf::Vector2<T>(ProjectJustWithinScreenEdge(
            sf::Vector2f(FIRST_POS_V),
            sf::Vector2f(SECOND_POS_V),
            sf::Vector2f(SCREEN_SIZE_V),
            sf::Vector2f(PROJECTED_IMAGE_SIZE_V)));
    }

    // same as ProjectToScreenEdge() but result is nudged back on screen of not there already
    // the bigger PROJECTED_IMAGE_SIZE_V is the greater error in the angle
    template <typename T>
    const sf::Vector2<T> ProjectJustWithinScreenEdge(
        const sf::Vector2<T> & FIRST_POS_V,
        const sf::Vector2<T> & SECOND_POS_V,
        const sf::Vector2<T> & PROJECTED_IMAGE_SIZE_V)
    {
        return ProjectJustWithinScreenEdge(
            FIRST_POS_V, SECOND_POS_V, sf::Vector2<T>(DisplaySize()), PROJECTED_IMAGE_SIZE_V);
    }

} // namespace sfutil
} // namespace heroespath

#endif // HEROESPATH_SFUTIL_ANGLES_HPP_INCLUDED
