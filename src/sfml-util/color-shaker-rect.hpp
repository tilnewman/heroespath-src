// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_COLOR_SHAKER_RECT_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_COLOR_SHAKER_RECT_HPP_INCLUDED
//
// color-shaker-rect.cpp
//
#include "sfml-util/color-shaker.hpp"
#include "sfml-util/colored-rect.hpp"
#include "sfml-util/sfml-graphics.hpp"

namespace heroespath
{
namespace sfml_util
{

    // Responsible for alternating between two colors at the given speed on a given rect.
    class ColorShakerRect : public sf::Drawable
    {
    public:
        explicit ColorShakerRect(
            const sf::FloatRect & RECT = sf::FloatRect(),
            const sf::Color & COLOR_START = sf::Color::Transparent,
            const sf::Color & COLOR_ALT = sf::Color::White,
            const float SPEED = 50.0f,
            const std::size_t CYCLE_COUNT_MAX = 0);

        void draw(sf::RenderTarget &, sf::RenderStates) const override;

        // returns true if the new current color matches COLOR_START, or put another way, after each
        // complete color cycle
        bool Update(const float ELAPSED_TIME_SECONDS);

        void Start() { colorShaker_.Start(); }
        void Pause() { colorShaker_.Stop(); }
        void Stop() { Reset(); }
        void Reset();

        void Reset(
            const sf::FloatRect & RECT,
            const sf::Color & COLOR_START,
            const sf::Color & COLOR_ALT,
            const float SPEED,
            const std::size_t CYCLE_COUNT_MAX = 0);

        bool IsShaking() const { return colorShaker_.IsShaking(); }

        const sf::Color Color() const { return colorShaker_.Current(); }

        void Color(const sf::Color & NEW_COLOR) { coloredRect_.Color(NEW_COLOR); }

        std::size_t CycleCount() const { return cycleCount_; }

        const sf::FloatRect Rect() const { return coloredRect_.Rect(); }

        void Rect(const sf::FloatRect & NEW_RECT) { coloredRect_.Rect(NEW_RECT); }

    private:
        std::size_t cycleCount_;
        std::size_t cycleCountMax_;
        sf::Color colorStart_;
        sf::Color colorAlt_;
        ColoredRect coloredRect_;
        ColorShaker colorShaker_;
    };

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_COLOR_SHAKER_RECT_HPP_INCLUDED
