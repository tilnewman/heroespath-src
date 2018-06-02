// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// color-shaker-rect.cpp
//
#include "color-shaker-rect.hpp"

namespace heroespath
{
namespace sfml_util
{

    ColorShakerRect::ColorShakerRect(
        const sf::FloatRect & RECT,
        const sf::Color & COLOR_START,
        const sf::Color & COLOR_ALT,
        const float SPEED,
        const std::size_t CYCLE_COUNT_MAX)
        : cycleCount_(0)
        , cycleCountMax_(CYCLE_COUNT_MAX)
        , colorStart_(COLOR_START)
        , colorAlt_(COLOR_ALT)
        , coloredRect_(RECT, COLOR_START)
        , colorShaker_(COLOR_START, COLOR_ALT, SPEED, false)
    {}

    void ColorShakerRect::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        target.draw(coloredRect_, states);
    }

    bool ColorShakerRect::Update(const float ELAPSED_TIME_SECONDS)
    {
        if (colorShaker_.IsShaking())
        {
            auto const WAS_MOVING_TOWARD{ colorShaker_.IsMovingToward() };
            auto const NEW_COLOR{ colorShaker_.Update(ELAPSED_TIME_SECONDS) };
            auto const IS_MOVING_TOWARD{ colorShaker_.IsMovingToward() };

            coloredRect_.Color(NEW_COLOR);

            if ((WAS_MOVING_TOWARD != IS_MOVING_TOWARD) && (NEW_COLOR == colorStart_))
            {
                if ((++cycleCount_ >= cycleCountMax_) && (cycleCountMax_ > 0))
                {
                    Reset();
                }

                return true;
            }
        }

        return false;
    }

    void ColorShakerRect::Reset()
    {
        Reset(coloredRect_.Rect(), colorStart_, colorAlt_, colorShaker_.Speed(), cycleCountMax_);
    }

    void ColorShakerRect::Reset(
        const sf::FloatRect & RECT,
        const sf::Color & COLOR_START,
        const sf::Color & COLOR_ALT,
        const float SPEED,
        const std::size_t CYCLE_COUNT_MAX)
    {
        cycleCount_ = 0;
        cycleCountMax_ = CYCLE_COUNT_MAX;
        colorStart_ = COLOR_START;
        colorAlt_ = COLOR_ALT;
        coloredRect_.Rect(RECT);
        coloredRect_.Color(COLOR_START);
        colorShaker_ = ColorShaker(COLOR_START, COLOR_ALT, SPEED, false);
    }

} // namespace sfml_util
} // namespace heroespath
