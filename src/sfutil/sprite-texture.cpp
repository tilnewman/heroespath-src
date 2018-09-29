// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// sprite-texture.cpp
//
#include "sprite-texture.hpp"

#include <tuple>

namespace sf
{

bool operator<(const sf::Sprite & L, const sf::Sprite & R)
{
    return std::make_tuple(
               L.getColor(), L.getGlobalBounds(), L.getScale(), L.getTexture(), L.getTextureRect())
        < std::make_tuple(
               R.getColor(), R.getGlobalBounds(), R.getScale(), R.getTexture(), R.getTextureRect());
}

bool operator==(const sf::Sprite & L, const sf::Sprite & R)
{
    return std::make_tuple(
               L.getColor(), L.getGlobalBounds(), L.getScale(), L.getTexture(), L.getTextureRect())
        == std::make_tuple(
               R.getColor(), R.getGlobalBounds(), R.getScale(), R.getTexture(), R.getTextureRect());
}

bool operator!=(const sf::Sprite & L, const sf::Sprite & R) { return !(L == R); }

} // namespace sf
