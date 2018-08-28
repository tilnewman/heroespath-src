// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_COLORED_RECT_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_COLORED_RECT_HPP_INCLUDED
//
// colored-rect.hpp
//
#include "misc/boost-optional-that-throws.hpp"
#include "sfml-util/corner-enum.hpp"
#include "sfml-util/side-enum.hpp"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/VertexArray.hpp>

#include <memory>

namespace heroespath
{
namespace sfml_util
{

    // Responsible for implementing a rectangle on screen with either a solid color or a gradient of
    // colors different colors at different corners.  Using the default constructed will result in a
    // valid object that draws nothing.
    class ColoredRect : public sf::Drawable
    {
    public:
        explicit ColoredRect(
            const sf::FloatRect & RECT = sf::FloatRect(0.0f, 0.0f, 0.0f, 0.0f),
            const sf::Color & COLOR = sf::Color::Transparent);

        // side colors are set before corner colors
        ColoredRect(
            const sf::FloatRect & RECT,
            const sf::Color & COLOR_FROM,
            const sf::Color & COLOR_TO,
            const Side::Enum SIDES_WITH_FROM_COLOR,
            const Corner::Enum CORNERS_WITH_FROM_COLOR = Corner::None);

        ColoredRect(const ColoredRect &) = default;
        ColoredRect(ColoredRect &&) = default;
        ColoredRect & operator=(const ColoredRect &) = default;
        ColoredRect & operator=(ColoredRect &&) = default;

        // changes to a default invalid state that draws nothing
        void Reset();

        void Setup(const sf::FloatRect & RECT, const sf::Color & COLOR);

        // side colors are set before corner colors, sets willDraw_ = true
        void Setup(
            const sf::FloatRect & RECT,
            const sf::Color & COLOR_FROM,
            const sf::Color & COLOR_TO,
            const Side::Enum SIDES_WITH_FROM_COLOR,
            const Corner::Enum CORNERS_WITH_FROM_COLOR = Corner::None);

        void draw(sf::RenderTarget &, sf::RenderStates) const override;

        bool IsGradient() const;

        bool IsSolidColor() const { return !IsGradient(); }

        const sf::Color Color() const;

        void Color(const sf::Color & NEW_COLOR);

        const sf::FloatRect Rect() const;
        void Rect(const sf::FloatRect & NEW_RECT);

        void SetPos(const float POS_LEFT, const float POS_TOP);
        void MovePos(const float HORIZ, const float VERT);

        bool WillDraw() const { return willDraw_; }
        void WillDraw(const bool WILL_DRAW) { willDraw_ = WILL_DRAW; }

        friend bool operator<(const ColoredRect & L, const ColoredRect & R);
        friend bool operator==(const ColoredRect & L, const ColoredRect & R);

    private:
        sf::VertexArray vertexes_;
        bool willDraw_;
    };

    using ColoredRectOpt_t = boost::optional<ColoredRect>;
    using ColoredRectUPtr_t = std::unique_ptr<ColoredRect>;

    // intentionally ignores willDraw_
    bool operator<(const ColoredRect & L, const ColoredRect & R);

    // intentionally ignores willDraw_
    bool operator==(const ColoredRect & L, const ColoredRect & R);

    // intentionally ignores willDraw_
    inline bool operator!=(const ColoredRect & L, const ColoredRect & R) { return !(L == R); }

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_COLORED_RECT_HPP_INCLUDED
