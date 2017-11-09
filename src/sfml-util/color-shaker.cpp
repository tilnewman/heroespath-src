// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
//
// color-shaker.cpp
//
#include "color-shaker.hpp"


namespace heroespath
{
namespace sfml_util
{

    ColorShaker::ColorShaker(const sf::Color & COLOR_ORIG,
                             const sf::Color & COLOR_ALT,
                             const float       SPEED,
                             const bool        WILL_START)
    :
        slider_   (0.0f, 1.0f, SPEED),
        colorOrig_(COLOR_ORIG),
        colorAlt_ (COLOR_ALT),
        colorCurr_(COLOR_ORIG),
        isShaking_(WILL_START),
        redDiff_  (static_cast<float>(COLOR_ALT.r - COLOR_ORIG.r)),
        greenDiff_(static_cast<float>(COLOR_ALT.g - COLOR_ORIG.g)),
        blueDiff_ (static_cast<float>(COLOR_ALT.b - COLOR_ORIG.b)),
        alphaDiff_(static_cast<float>(COLOR_ALT.a - COLOR_ORIG.a))
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

}
}
