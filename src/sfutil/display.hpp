// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFUTIL_DISPLAY_HPP_INCLUDED
#define HEROESPATH_SFUTIL_DISPLAY_HPP_INCLUDED
//
// display.hpp
//
#include <SFML/Graphics/Rect.hpp>

namespace heroespath
{
namespace sfutil
{

    // returns the current display resolution
    const sf::Vector2f DisplaySize();

    // returns a rect at position (0,0) with size matching the current display resolution
    inline const sf::Rect<float> DisplayRect()
    {
        return { sf::Vector2f(0.0f, 0.0f), DisplaySize() };
    }

    // map a VAL within [IN_MIN, IN_MAX] to the range [OUT_MIN, OUT_MAX]
    template <typename T>
    constexpr T
        Map(const T VAL_ORIG,
            const T IN_MIN_ORIG,
            const T IN_MAX_ORIG,
            const T OUT_MIN_ORIG,
            const T OUT_MAX_ORIG)
    {
        auto const VAL { static_cast<double>(VAL_ORIG) };
        auto const IN_MIN { static_cast<double>(IN_MIN_ORIG) };
        auto const IN_MAX { static_cast<double>(IN_MAX_ORIG) };
        auto const OUT_MIN { static_cast<double>(OUT_MIN_ORIG) };
        auto const OUT_MAX { static_cast<double>(OUT_MAX_ORIG) };

        return static_cast<T>(
            OUT_MIN + (((VAL - IN_MIN) * (OUT_MAX - OUT_MIN)) / (IN_MAX - IN_MIN)));
    }

    /*
    load-game-menu-stage.cpp(127)       sfutil::MapByRes(0.0f, 800.0f);

    party-stage.cpp(401)                sfutil::MapByRes(0.0f, 800.0f) };

    party-stage.cpp(535)                sfutil::MapByRes(0.0f, 20.0f) };

    party-stage.cpp(657)                sfutil::MapByRes(0.0f, 600.0f);

    treasure-display-stage.cpp(829)     sfutil::MapByRes(0.0f, 50.0f) };

    adventure-stage-char-list.cpp(574)  sfutil::MapByRes(0.0f, 10.0f) };

    inventory-stage.cpp(3736)           sfutil::MapByRes(0.0f, 50.0f));

    combat-stage.cpp(3485)              sfutil::MapByRes(0.0f, 16.0f));

    */

    // maps[(1280*900), (7680*4800)/2] at the current resolution width*height to [THE_MIN, THE_MAX],
    // which with THE_MIN=0.0f and THE_MAX=(below) and a current res of 2880x1880 yeilds:
    //  1000 = 233
    //  500  = 117
    //  100  = 23
    //  50   = 11.65
    //  20   = 4.65
    //  10   = 2.33
    template <typename T>
    T MapByRes(const T THE_MIN_ORIG, const T THE_MAX_ORIG)
    {
        auto const THE_MIN { static_cast<double>(THE_MIN_ORIG) };
        auto const THE_MAX { static_cast<double>(THE_MAX_ORIG) };
        auto const RES_AREA_MIN { 1280.0 * 900.0 };
        auto const RES_AREA_MAX { (7680.0 * 4800.0) * 0.5 };
        const sf::Vector2<double> SCREEN_SIZE_V { DisplaySize() };
        auto const RES_AREA_CURR { SCREEN_SIZE_V.x * SCREEN_SIZE_V.y };

        return static_cast<T>(Map(RES_AREA_CURR, RES_AREA_MIN, RES_AREA_MAX, THE_MIN, THE_MAX));
    }

    template <typename T>
    T SpacerOld(const T AMOUNT)
    {
        return MapByRes(T(0), AMOUNT);
    }

    // returns RATIO (percent) number of horizontal screen pixels, same as (DisplaySize().x * RATIO)
    inline float ScreenRatioToPixelsHoriz(const float RATIO) { return (RATIO * DisplaySize().x); }

    // returns RATIO (percent) number of vertical screen pixels, same as (DisplaySize().y * RATIO)
    inline float ScreenRatioToPixelsVert(const float RATIO) { return (RATIO * DisplaySize().y); }

    inline const sf::Vector2f ScreenRatiosToPixels(const float RATIO_HORIZ, const float RATIO_VERT)
    {
        return sf::Vector2f(
            ScreenRatioToPixelsHoriz(RATIO_HORIZ), ScreenRatioToPixelsVert(RATIO_VERT));
    }

} // namespace sfutil
} // namespace heroespath

#endif // HEROESPATH_SFUTIL_DISPLAY_HPP_INCLUDED
