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
// color-slider.cpp
//
#include "color-slider.hpp"

namespace heroespath
{
namespace sfml_util
{

    ColorSlider::ColorSlider()
        : fromColor_(sf::Color::Transparent)
        , toColor_(sf::Color::Transparent)
        , origFromColor_(sf::Color::Transparent)
        , origToColor_(sf::Color::Transparent)
        , direction_(sfml_util::Moving::Count)
        , isMoving_(false)
        , slider_()
    {}

    ColorSlider::ColorSlider(
        const sf::Color & FROM_COLOR, const sf::Color & TO_COLOR, const float SPEED)
        : fromColor_(FROM_COLOR)
        , toColor_(TO_COLOR)
        , origFromColor_(FROM_COLOR)
        , origToColor_(TO_COLOR)
        , direction_(sfml_util::Moving::Toward)
        , isMoving_(false)
        , slider_(SPEED)
    {}

    void ColorSlider::Setup(
        const sf::Color & FROM_COLOR, const sf::Color & TO_COLOR, const float SPEED)
    {
        fromColor_ = FROM_COLOR;
        toColor_ = TO_COLOR;
        origFromColor_ = FROM_COLOR;
        origToColor_ = TO_COLOR;
        direction_ = sfml_util::Moving::Toward;
        isMoving_ = false;
        slider_.Reset(SPEED);
    }

    void ColorSlider::ChangeDirection()
    {
        if (direction_ == Moving::Toward)
        {
            fromColor_ = Current();
            toColor_ = origFromColor_;
            slider_.Reset(slider_.Speed());
            direction_ = Moving::Away;
        }
        else if (direction_ == Moving::Away)
        {
            fromColor_ = Current();
            toColor_ = origToColor_;
            slider_.Reset(slider_.Speed());
            direction_ = Moving::Toward;
        }
    }

    bool ColorSlider::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        if (isMoving_)
        {
            slider_.Update(ELAPSED_TIME_SECONDS);
            isMoving_ = !slider_.IsDone();
            return true;
        }
        else
        {
            return false;
        }
    }

    const sf::Color ColorSlider::Current() const
    {
        auto const RATIO{ slider_.Current() };

        auto const RED_DIFF{ static_cast<float>(toColor_.r - fromColor_.r) };
        auto const GREEN_DIFF{ static_cast<float>(toColor_.g - fromColor_.g) };
        auto const BLUE_DIFF{ static_cast<float>(toColor_.b - fromColor_.b) };
        auto const ALPHA_DIFF{ static_cast<float>(toColor_.a - fromColor_.a) };

        auto const RED{ static_cast<float>(fromColor_.r) + (RED_DIFF * RATIO) };
        auto const GREEN{ static_cast<float>(fromColor_.g) + (GREEN_DIFF * RATIO) };
        auto const BLUE{ static_cast<float>(fromColor_.b) + (BLUE_DIFF * RATIO) };
        auto const ALPHA{ static_cast<float>(fromColor_.a) + (ALPHA_DIFF * RATIO) };

        return sf::Color(
            static_cast<sf::Uint8>(RED),
            static_cast<sf::Uint8>(GREEN),
            static_cast<sf::Uint8>(BLUE),
            static_cast<sf::Uint8>(ALPHA));
    }
}
}
