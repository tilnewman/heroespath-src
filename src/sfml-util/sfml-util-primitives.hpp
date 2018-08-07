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
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

namespace heroespath
{
namespace sfml_util
{

    // the line thickness grows inside/within the REGION
    const sf::RectangleShape MakeRectangle(
        const sf::FloatRect & REGION,
        const sf::Color & LINE_COLOR = sf::Color::White,
        const float LINE_THICKNESS = 1.0f,
        const sf::Color & FILL_COLOR = sf::Color::Transparent,
        const bool WILL_GROW_BORDER_TO_CONTAIN_REGION = false);

    const sf::RectangleShape MakeRectangleSolid(
        const sf::FloatRect & REGION,
        const sf::Color & COLOR,
        const bool WILL_GROW_BORDER_TO_CONTAIN_REGION = false);

    const sf::RectangleShape MakeRectangleHollow(
        const sf::FloatRect & REGION,
        const sf::Color & COLOR,
        const float LINE_THICKNESS = 1.0f,
        const bool WILL_GROW_BORDER_TO_CONTAIN_REGION = false);

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_SFMLUTIL_PRIMITIVES_HPP_INCLUDED
