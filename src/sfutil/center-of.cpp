// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// center-of.cpp
//
#include "sfutil/center-of.hpp"

#include <SFML/Graphics/Sprite.hpp>

namespace heroespath
{
namespace sfutil
{

    const sf::Vector2f CenterOf(const sf::Sprite & S) { return CenterOf(S.getGlobalBounds()); }

    float CenterOfHoriz(const sf::Sprite & S) { return CenterOfHoriz(S.getGlobalBounds()); }

    float CenterOfVert(const sf::Sprite & S) { return CenterOfVert(S.getGlobalBounds()); }

} // namespace sfutil
} // namespace heroespath
