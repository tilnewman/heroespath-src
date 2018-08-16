// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// sfml-util-center.cpp
//
#include "sfml-util-center.hpp"

#include <SFML/Graphics/Sprite.hpp>

namespace heroespath
{
namespace sfml_util
{

    const sf::Vector2f CenterCopy(const sf::Sprite & S)
    {
        return CenterCopy(S.getGlobalBounds().width, S.getGlobalBounds().height) + S.getOrigin();
    }

    void Center(sf::Sprite & s) { s.setPosition(CenterCopy(s)); }

} // namespace sfml_util
} // namespace heroespath
