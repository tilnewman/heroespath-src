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
#include "sfml-util/sfml-graphics.hpp"

namespace heroespath
{
namespace sfml_util
{

    // A class allowing simple fading in and out of a color to any part of the screen.
    class ColoredRect : public sf::Drawable
    {
    public:
        ColoredRect(
            const sf::FloatRect & RECT = sf::FloatRect(),
            const sf::Color & COLOR = sf::Color::Black);

        const sf::FloatRect Rect() const;
        void Rect(const sf::FloatRect & NEW_RECT);

        const sf::Color Color() const;
        void Color(const sf::Color & NEW_COLOR);

        void draw(sf::RenderTarget &, sf::RenderStates) const override;

    private:
        sf::VertexArray quads_;
    };

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_COLORED_RECT_HPP_INCLUDED
