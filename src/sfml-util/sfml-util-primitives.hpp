// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_SFMLUTIL_PRIMITIVES_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_SFMLUTIL_PRIMITIVES_HPP_INCLUDED
//
// sfml-util-primitives.hpp
//
#include "sfml-util/sfml-util-position.hpp"
#include "sfml-util/sfml-util-size-and-scale.hpp"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

namespace heroespath
{
namespace sfml_util
{

    template <typename Region_t, typename Thickness_t = float>
    void DrawRectangle(
        sf::RenderTarget & target,
        const sf::RenderStates & STATES,
        const sf::Rect<Region_t> REGION,
        const sf::Color & LINE_COLOR = sf::Color::White,
        const Thickness_t LINE_THICKNESS = Thickness_t(1),
        const sf::Color & FILL_COLOR = sf::Color::Transparent)
    {
        sf::RectangleShape rs;
        rs.setPosition(sf::Vector2f(Position<Region_t>(REGION)));
        rs.setSize(sf::Vector2f(Size<Region_t>(REGION)));
        rs.setFillColor(FILL_COLOR);
        rs.setOutlineColor(LINE_COLOR);
        rs.setOutlineThickness(static_cast<float>(LINE_THICKNESS));
        target.draw(rs, STATES);
    }

    void DrawRectangleWithLineVerts(
        const sf::FloatRect & RECT, const sf::Color & COLOR, std::vector<sf::Vertex> & verts);

    void DrawQuad(
        const sf::FloatRect & RECT,
        const sf::Color & COLOR_LEFT,
        const sf::Color & COLOR_RIGHT,
        std::vector<sf::Vertex> & verts);

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_SFMLUTIL_PRIMITIVES_HPP_INCLUDED
