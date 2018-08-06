// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// sfml-util-center-of.cpp
//
#include "sfml-util/sfml-util-center-of.hpp"

#include <SFML/Graphics/Sprite.hpp>

namespace heroespath
{
namespace sfml_util
{

    const sf::Vector2f CenterOf(const sf::Sprite & S) { return CenterOf(S.getGlobalBounds()); }

    float CenterOfHoriz(const sf::Sprite & S) { return CenterOfHoriz(S.getGlobalBounds()); }

    float CenterOfVert(const sf::Sprite & S) { return CenterOfVert(S.getGlobalBounds()); }

} // namespace sfml_util
} // namespace heroespath
