// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
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
        const auto RATIO { slider_.Current() };

        const auto RED_DIFF { static_cast<float>(toColor_.r - fromColor_.r) };
        const auto GREEN_DIFF { static_cast<float>(toColor_.g - fromColor_.g) };
        const auto BLUE_DIFF { static_cast<float>(toColor_.b - fromColor_.b) };
        const auto ALPHA_DIFF { static_cast<float>(toColor_.a - fromColor_.a) };

        const auto RED { static_cast<float>(fromColor_.r) + (RED_DIFF * RATIO) };
        const auto GREEN { static_cast<float>(fromColor_.g) + (GREEN_DIFF * RATIO) };
        const auto BLUE { static_cast<float>(fromColor_.b) + (BLUE_DIFF * RATIO) };
        const auto ALPHA { static_cast<float>(fromColor_.a) + (ALPHA_DIFF * RATIO) };

        return sf::Color(
            static_cast<sf::Uint8>(RED),
            static_cast<sf::Uint8>(GREEN),
            static_cast<sf::Uint8>(BLUE),
            static_cast<sf::Uint8>(ALPHA));
    }
} // namespace sfml_util
} // namespace heroespath
