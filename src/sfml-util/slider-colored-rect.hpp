// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_SLIDER_COLORED_RECT_HPP_INCLUDED
#define HEROESPATH_GUI_SLIDER_COLORED_RECT_HPP_INCLUDED
//
// slider-colored-rect.hpp
//
#include "sfml-util/colored-rect.hpp"
#include "sfml-util/slider-color.hpp"

#include <SFML/Graphics/Drawable.hpp>

namespace heroespath
{
namespace gui
{

    // Uses the smooth motion slider method to oscillate a ColoredRect between two colors.
    class ColoredRectSlider
        : public ColorSlider
        , public sf::Drawable
    {
    public:
        // default constructor has an empty region, all transparent colors, and is stopped
        ColoredRectSlider()
            : ColorSlider()
            , coloredRect_(sf::FloatRect(), sf::Color::Transparent)
        {}

        ColoredRectSlider(
            const sf::FloatRect & REGION,
            const sf::Color & COLOR_FROM,
            const sf::Color & COLOR_TO,
            const float SPEED,
            const WillOscillate WILL_OSCILLATE,
            const WillAutoStart WILL_AUTO_START,
            const std::size_t CYCLE_COUNT_LIMIT = 0)
            : ColorSlider(
                COLOR_FROM, COLOR_TO, SPEED, WILL_OSCILLATE, WILL_AUTO_START, CYCLE_COUNT_LIMIT)
            , coloredRect_(REGION, COLOR_FROM)
        {
            OnUpdateChange();
        }

        virtual ~ColoredRectSlider() = default;

        ColoredRectSlider(const ColoredRectSlider &) = default;
        ColoredRectSlider(ColoredRectSlider &&) = default;
        ColoredRectSlider & operator=(const ColoredRectSlider &) = default;
        ColoredRectSlider & operator=(ColoredRectSlider &&) = default;

        void draw(sf::RenderTarget & target, sf::RenderStates states) const override
        {
            coloredRect_.draw(target, states);
        }

        const sf::FloatRect Rect() const { return coloredRect_.Rect(); }
        void Rect(const sf::FloatRect & NEW_RECT) { coloredRect_.Rect(NEW_RECT); }

        void SetPos(const float POS_LEFT, const float POS_TOP)
        {
            coloredRect_.SetPos(POS_LEFT, POS_TOP);
        }

        void MovePos(const float HORIZ, const float VERT) { coloredRect_.MovePos(HORIZ, VERT); }

        bool WillDraw() const { return coloredRect_.WillDraw(); }
        void WillDraw(const bool WILL_DRAW) { coloredRect_.WillDraw(WILL_DRAW); }

        const gui::ColoredRect RectColored() const { return coloredRect_; }

    protected:
        void OnUpdateChange() override { coloredRect_.Color(Value()); }

    private:
        gui::ColoredRect coloredRect_;
    };

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_SLIDER_COLORED_RECT_HPP_INCLUDED
