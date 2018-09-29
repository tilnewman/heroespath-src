// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// overlap.cpp
//
#include "overlap.hpp"

#include <SFML/Graphics/Sprite.hpp>

namespace heroespath
{
namespace sfutil
{

    bool intersects(const sf::Sprite & A, const sf::Sprite & B)
    {
        return A.getGlobalBounds().intersects(B.getGlobalBounds());
    }

    bool intersects(const sf::Sprite & A, const sf::FloatRect & R)
    {
        return A.getGlobalBounds().intersects(sf::FloatRect(R));
    }

} // namespace sfutil
} // namespace heroespath
