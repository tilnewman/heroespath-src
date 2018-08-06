// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// sfml-util-position.cpp
//
#include "sfml-util-position.hpp"

#include <SFML/Graphics/Sprite.hpp>

namespace heroespath
{
namespace sfml_util
{

    const sf::Vector2f Position(const sf::Sprite & S) { return S.getPosition(); }

    float Right(const sf::Sprite & SPRITE) { return Right(SPRITE.getGlobalBounds()); }

    float Bottom(const sf::Sprite & SPRITE) { return Bottom(SPRITE.getGlobalBounds()); }

} // namespace sfml_util
} // namespace heroespath
