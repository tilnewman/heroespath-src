// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_SFMLUTIL_RECTANGLE_SHAPE_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_SFMLUTIL_RECTANGLE_SHAPE_HPP_INCLUDED
//
// sfml-util-rectangle-shape.hpp
//
#include "sfml-util/sfml-util-color.hpp"
#include "sfml-util/sfml-util-sprite-texture.hpp"
#include "sfml-util/sfml-util-vector-rect.hpp"

#include <SFML/Graphics/RectangleShape.hpp>

#include <tuple>

namespace sf
{

inline bool operator<(const sf::RectangleShape & L, const sf::RectangleShape & R)
{
    return std::make_tuple(
               L.getFillColor(),
               L.getGlobalBounds(),
               L.getLocalBounds(),
               L.getOutlineColor(),
               L.getTexture(),
               L.getTextureRect())
        < std::make_tuple(
               R.getFillColor(),
               R.getGlobalBounds(),
               R.getLocalBounds(),
               R.getOutlineColor(),
               R.getTexture(),
               R.getTextureRect());
}

inline bool operator==(const sf::RectangleShape & L, const sf::RectangleShape & R)
{
    return std::make_tuple(
               L.getFillColor(),
               L.getGlobalBounds(),
               L.getLocalBounds(),
               L.getOutlineColor(),
               L.getTexture(),
               L.getTextureRect())
        == std::make_tuple(
               R.getFillColor(),
               R.getGlobalBounds(),
               R.getLocalBounds(),
               R.getOutlineColor(),
               R.getTexture(),
               R.getTextureRect());
}

inline bool operator!=(const sf::RectangleShape & L, const sf::RectangleShape & R)
{
    return !(L == R);
}

} // namespace sf

#endif // HEROESPATH_SFMLUTIL_SFMLUTIL_RECTANGLE_SHAPE_HPP_INCLUDED
