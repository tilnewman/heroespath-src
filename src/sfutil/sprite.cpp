// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// sprite.cpp
//

#include "sprite.hpp"

#include "sfutil/color.hpp"
#include "sfutil/vector-and-rect.hpp"

#include <SFML/Graphics/Sprite.hpp>

#include <tuple>

namespace sf
{

bool operator<(const sf::Sprite & L, const sf::Sprite & R)
{
    return (
        std::make_tuple(
            L.getGlobalBounds(), L.getTexture(), L.getColor(), L.getLocalBounds(), L.getRotation())
        < std::make_tuple(
              R.getGlobalBounds(),
              R.getTexture(),
              R.getColor(),
              R.getLocalBounds(),
              R.getRotation()));
}

bool operator==(const sf::Sprite & L, const sf::Sprite & R)
{
    return (
        std::make_tuple(
            L.getGlobalBounds(), L.getTexture(), L.getColor(), L.getLocalBounds(), L.getRotation())
        == std::make_tuple(
               R.getGlobalBounds(),
               R.getTexture(),
               R.getColor(),
               R.getLocalBounds(),
               R.getRotation()));
}

} // namespace sf
