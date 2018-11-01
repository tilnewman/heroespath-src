// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_SLIDER_COLOR_HPP_INCLUDED
#define HEROESPATH_GUI_SLIDER_COLOR_HPP_INCLUDED
//
// slider-color.hpp
//
#include "sfml-util/slider-base.hpp"

#include <SFML/Graphics/Color.hpp>

namespace heroespath
{
namespace gui
{

    // Uses the smooth motion slider method to oscillate between two colors.
    class ColorSlider : public SliderBase
    {
    public:
        // default constructor sets all colors transparent and is stopped
        ColorSlider()
            : SliderBase()
            , from_(sf::Color::Transparent)
            , to_(sf::Color::Transparent)
            , value_(sf::Color::Transparent)
        {}

        ColorSlider(
            const sf::Color & COLOR_FROM,
            const sf::Color & COLOR_TO,
            const float SPEED,
            const WillOscillate WILL_OSCILLATE,
            const WillAutoStart WILL_AUTO_START,
            const std::size_t CYCLE_COUNT_LIMIT = 0)
            : SliderBase(SPEED, WILL_OSCILLATE, WILL_AUTO_START, CYCLE_COUNT_LIMIT)
            , from_(COLOR_FROM)
            , to_(COLOR_TO)
            , value_(COLOR_FROM)
        {}

        virtual ~ColorSlider() = default;

        ColorSlider(const ColorSlider &) = default;
        ColorSlider(ColorSlider &&) = default;
        ColorSlider & operator=(const ColorSlider &) = default;
        ColorSlider & operator=(ColorSlider &&) = default;

        bool IsAtFrom() const override { return (value_ == from_); }
        bool IsAtTo() const override { return (value_ == to_); }

        const sf::Color From() const { return from_; }
        const sf::Color To() const { return to_; }
        const sf::Color Value() const { return value_; }

        const sf::Color UpdateAndReturnValue(const float ADJUSTMENT)
        {
            Update(ADJUSTMENT);
            return Value();
        }

    protected:
        bool UpdateImpl() override
        {
            const auto VALUE_BEFORE { value_ };

            const auto RATIO { PositionRatio() };

            const auto RED_DIFF { static_cast<float>(To().r - From().r) };
            const auto GREEN_DIFF { static_cast<float>(To().g - From().g) };
            const auto BLUE_DIFF { static_cast<float>(To().b - From().b) };
            const auto ALPHA_DIFF { static_cast<float>(To().a - From().a) };

            const auto RED { static_cast<float>(From().r) + (RED_DIFF * RATIO) };
            const auto GREEN { static_cast<float>(From().g) + (GREEN_DIFF * RATIO) };
            const auto BLUE { static_cast<float>(From().b) + (BLUE_DIFF * RATIO) };
            const auto ALPHA { static_cast<float>(From().a) + (ALPHA_DIFF * RATIO) };

            value_ = sf::Color(
                static_cast<sf::Uint8>(RED),
                static_cast<sf::Uint8>(GREEN),
                static_cast<sf::Uint8>(BLUE),
                static_cast<sf::Uint8>(ALPHA));

            return (VALUE_BEFORE != value_);
        }

        void SetFromImpl() override { value_ = from_; }
        void SetToImpl() override { value_ = to_; }

    private:
        sf::Color from_;
        sf::Color to_;
        sf::Color value_;
    };

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_SLIDER_COLOR_HPP_INCLUDED
