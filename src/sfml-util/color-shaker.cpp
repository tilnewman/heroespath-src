// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// color-shaker.cpp
//
#include "color-shaker.hpp"

namespace heroespath
{
namespace sfml_util
{

    ColorShaker::ColorShaker(
        const sf::Color & COLOR_ORIG,
        const sf::Color & COLOR_ALT,
        const float SPEED,
        const bool WILL_START)
        : slider_(0.0f, 1.0f, static_cast<double>(SPEED))
        , colorOrig_(COLOR_ORIG)
        , colorAlt_(COLOR_ALT)
        , colorCurr_(COLOR_ORIG)
        , isShaking_(WILL_START)
        , redDiff_(static_cast<float>(COLOR_ALT.r - COLOR_ORIG.r))
        , greenDiff_(static_cast<float>(COLOR_ALT.g - COLOR_ORIG.g))
        , blueDiff_(static_cast<float>(COLOR_ALT.b - COLOR_ORIG.b))
        , alphaDiff_(static_cast<float>(COLOR_ALT.a - COLOR_ORIG.a))
    {}

    const sf::Color ColorShaker::Update(const float ELAPSED_TIME_SECONDS)
    {
        if (isShaking_)
        {
            const float NEW_RATIO{ slider_.Update(ELAPSED_TIME_SECONDS) };

            colorCurr_ = colorOrig_;

            colorCurr_.r = static_cast<sf::Uint8>(
                static_cast<int>(colorCurr_.r) + static_cast<int>(NEW_RATIO * redDiff_));

            colorCurr_.g = static_cast<sf::Uint8>(
                static_cast<int>(colorCurr_.g) + static_cast<int>(NEW_RATIO * greenDiff_));

            colorCurr_.b = static_cast<sf::Uint8>(
                static_cast<int>(colorCurr_.b) + static_cast<int>(NEW_RATIO * blueDiff_));

            colorCurr_.a = static_cast<sf::Uint8>(
                static_cast<int>(colorCurr_.a) + static_cast<int>(NEW_RATIO * alphaDiff_));
        }

        return colorCurr_;
    }

    void ColorShaker::Reset()
    {
        colorCurr_ = colorOrig_;
        slider_.Reset(0.0f, 1.0f);
    }
} // namespace sfml_util
} // namespace heroespath
