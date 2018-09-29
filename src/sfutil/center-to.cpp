// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// center-to.cpp
//
#include "center-to.hpp"
#include "sfutil/size-and-scale.hpp"

#include <SFML/Graphics/Sprite.hpp>

namespace heroespath
{
namespace sfutil
{

    void CenterTo(sf::Sprite & s, const sf::FloatRect & R)
    {
        s.setPosition((CenterOf(R) - (Size(s) * 0.5f)) + s.getOrigin());
    }

} // namespace sfutil
} // namespace heroespath
