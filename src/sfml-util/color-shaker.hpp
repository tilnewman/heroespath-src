// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_COLORSHAKER_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_COLORSHAKER_HPP_INCLUDED
//
// color-shaker.cpp
//
#include "sfml-util/sliders.hpp"

#include <SFML/Graphics/Color.hpp>

namespace heroespath
{
namespace sfml_util
{

    // Responsible for alternating between two colors at the given speed.
    class ColorShaker
    {
    public:
        explicit ColorShaker(
            const sf::Color & COLOR_ORIG,
            const sf::Color & COLOR_ALT = sf::Color::White,
            const float SPEED = 50.0f,
            const bool WILL_START = true);

        const sf::Color Update(const float ELAPSED_TIME_SECONDS);

        void Reset();

        void Start() { isShaking_ = true; }
        void Stop() { isShaking_ = false; }
        float Speed() const { return static_cast<float>(slider_.Speed()); }
        bool IsShaking() const { return isShaking_; }
        const sf::Color Current() const { return colorCurr_; }

    private:
        sliders::Slider<float> slider_;
        sf::Color colorOrig_;
        sf::Color colorAlt_;
        sf::Color colorCurr_;
        bool isShaking_;
        float redDiff_;
        float greenDiff_;
        float blueDiff_;
        float alphaDiff_;
    };
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_COLORSHAKER_HPP_INCLUDED
