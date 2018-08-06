// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// sfml-util-primitives.cpp
//
#include "sfml-util-primitives.hpp"

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

    const sf::RectangleShape MakeRectangle(
        const sf::FloatRect & REGION,
        const sf::Color & LINE_COLOR,
        const float LINE_THICKNESS,
        const sf::Color & FILL_COLOR)
    {
        const sf::Vector2f THICK_V(LINE_THICKNESS, LINE_THICKNESS);

        sf::RectangleShape rs;
        rs.setPosition(Position(REGION) + THICK_V);
        rs.setSize(Size(REGION) - (THICK_V * 2.0f));
        rs.setFillColor(FILL_COLOR);
        rs.setOutlineColor(LINE_COLOR);
        rs.setOutlineThickness(LINE_THICKNESS);

        return rs;
    }

    const sf::RectangleShape
        MakeRectangleSolid(const sf::FloatRect & REGION, const sf::Color & COLOR)
    {
        return MakeRectangle(REGION, COLOR, 0.0f, COLOR);
    }

} // namespace sfml_util
} // namespace heroespath
