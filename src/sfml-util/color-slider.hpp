// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_COLOR_SLIDER_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_COLOR_SLIDER_HPP_INCLUDED
//
// color-slider.hpp
//
#include "sfml-util/moving-enum.hpp"
#include "sfml-util/pos-slider.hpp"
#include "sfml-util/sliders.hpp"

#include <SFML/Graphics/Color.hpp>

namespace heroespath
{
namespace sfml_util
{

    // Similar to the PositionSlider, ColorSlider presents a simple interface for controlling a
    // color that starts at a given FROM and then moves toward a TO color.  Calling Update()
    // periodically with the time elapsed advances the position.  ChangeDirection() does not
    // change the current position, it sets FROM to the current position and sets TO to either
    // the original FROM or TO.
    class ColorSlider
    {
    public:
        ColorSlider();

        explicit ColorSlider(
            const sf::Color & FROM_COLOR, const sf::Color & TO_COLOR, const float SPEED);

        void Setup(const sf::Color & FROM_COLOR, const sf::Color & TO_COLOR, const float SPEED);

        void ChangeDirection();

        // returns true if the position was changed
        bool UpdateTime(const float ELAPSED_TIME_SECONDS);

        const sf::Color Current() const;

        float ProgressRatio() const { return slider_.Current(); }
        sfml_util::Moving::Enum Direction() const { return direction_; }
        const sf::Color To() const { return toColor_; }
        const sf::Color From() const { return fromColor_; }
        float Speed() const { return slider_.Speed(); }
        bool IsMoving() const { return isMoving_; }
        void Start() { isMoving_ = true; }
        void Stop() { isMoving_ = false; }

        void Speed(const float S) { slider_.Reset(S, slider_.Current()); }

    private:
        sf::Color fromColor_;
        sf::Color toColor_;
        sf::Color origFromColor_;
        sf::Color origToColor_;
        sfml_util::Moving::Enum direction_;
        bool isMoving_;
        sfml_util::sliders::ZeroSliderOnce<float> slider_;
    };

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_COLOR_SLIDER_HPP_INCLUDED
