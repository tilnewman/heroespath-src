// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// angles.cpp
//
#include "angles.hpp"

#include "sfutil/display.hpp"

#include <boost/math/constants/constants.hpp>

#include <algorithm>
#include <cmath>

namespace heroespath
{
namespace sfutil
{

    float GetAngleInDegrees(const sf::Vector2f & BEGIN_POS_V, const sf::Vector2f & END_POS_V)
    {
        auto const ANGLE_RADIANS { std::atan(
            (std::max(BEGIN_POS_V.y, END_POS_V.y) - std::min(BEGIN_POS_V.y, END_POS_V.y))
            / (std::max(BEGIN_POS_V.x, END_POS_V.x) - std::min(BEGIN_POS_V.x, END_POS_V.x))) };

        auto const ANGLE_DEGREES { (ANGLE_RADIANS * 180.0f) / boost::math::constants::pi<float>() };

        if (BEGIN_POS_V.x < END_POS_V.x)
        {
            if (BEGIN_POS_V.y > END_POS_V.y)
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
            if (BEGIN_POS_V.y > END_POS_V.y)
            {
                return (180.0f - ANGLE_DEGREES) * -1.0f; // QII
            }
            else
            {
                return (180.0f + ANGLE_DEGREES) * -1.0f; // QIII
            }
        }
    }

    const sf::Vector2f ProjectToScreenEdge(
        const sf::Vector2f & FIRST_POS_V,
        const sf::Vector2f & SECOND_POS_V,
        const sf::Vector2f & PROJECTED_IMAGE_SIZE)
    {
        sf::Vector2f finalPosV { 0.0f, 0.0f };

        auto const SCREEN_SIZE_V { sfutil::DisplaySize() };

        // use linear relations to find the edge-of-screen miss point, first assuming the vertical
        // position is the vertical screen edge...
        auto const SPRITE_HEIGHT { PROJECTED_IMAGE_SIZE.y };

        finalPosV.y
            = ((FIRST_POS_V.y < SECOND_POS_V.y) ? (SCREEN_SIZE_V.y + SPRITE_HEIGHT + 1.0f)
                                                : ((-1.0f * SPRITE_HEIGHT) - 1.0f));

        auto const AV_HORIZ_OVER_VERT {
            (std::max(FIRST_POS_V.x, SECOND_POS_V.x) - std::min(FIRST_POS_V.x, SECOND_POS_V.x))
            / (std::max(FIRST_POS_V.y, SECOND_POS_V.y) - std::min(FIRST_POS_V.y, SECOND_POS_V.y))
        };

        auto const AV_VERTICAL_EXTENT { (
            (SECOND_POS_V.y > FIRST_POS_V.y) ? (SCREEN_SIZE_V.y - FIRST_POS_V.y) : FIRST_POS_V.y) };

        auto const AV_HORIZ_EXTENT { AV_HORIZ_OVER_VERT * AV_VERTICAL_EXTENT };

        if (SECOND_POS_V.x > FIRST_POS_V.x)
        {
            finalPosV.x = FIRST_POS_V.x + AV_HORIZ_EXTENT;
        }
        else
        {
            finalPosV.x = FIRST_POS_V.x - AV_HORIZ_EXTENT;
        }

        //...but if that is too far, then assume the horizontal screen edge
        if (std::abs(finalPosV.x - FIRST_POS_V.x) > SCREEN_SIZE_V.x)
        {
            auto const SPRITE_WIDTH { PROJECTED_IMAGE_SIZE.x };

            finalPosV.x
                = ((FIRST_POS_V.x < SECOND_POS_V.x) ? (SCREEN_SIZE_V.x + SPRITE_WIDTH + 1.0f)
                                                    : (0.0f - SPRITE_WIDTH - 1.0f));

            auto const AH_VERT_OVER_HORIZ { (std::max(FIRST_POS_V.y, SECOND_POS_V.y)
                                             - std::min(FIRST_POS_V.y, SECOND_POS_V.y))
                                            / (std::max(FIRST_POS_V.x, SECOND_POS_V.x)
                                               - std::min(FIRST_POS_V.x, SECOND_POS_V.x)) };

            auto const AH_HORIZ_EXTENT { (
                (SECOND_POS_V.x > FIRST_POS_V.x) ? (SCREEN_SIZE_V.x - FIRST_POS_V.x)
                                                 : FIRST_POS_V.x) };

            auto const AH_VERT_EXTENT { AH_VERT_OVER_HORIZ * AH_HORIZ_EXTENT };

            if (SECOND_POS_V.y > FIRST_POS_V.y)
            {
                finalPosV.y = FIRST_POS_V.y + AH_VERT_EXTENT;
            }
            else
            {
                finalPosV.y = FIRST_POS_V.y - AH_VERT_EXTENT;
            }
        }

        return finalPosV;
    }

} // namespace sfutil
} // namespace heroespath
