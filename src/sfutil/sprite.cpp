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

namespace sf
{

bool operator<(const sf::Sprite & L, const sf::Sprite & R)
{
    if ((L.getGlobalBounds() < R.getGlobalBounds()) || (L.getGlobalBounds() > R.getGlobalBounds()))
    {
        return (L.getGlobalBounds() < R.getGlobalBounds());
    }
    else if (L.getTexture() != R.getTexture())
    {
        return (L.getTexture() < R.getTexture());
    }
    else if (L.getColor() != R.getColor())
    {
        return (L.getColor() < R.getColor());
    }
    else if ((L.getLocalBounds() < R.getLocalBounds()) || (L.getLocalBounds() > R.getLocalBounds()))
    {
        return (L.getLocalBounds() < R.getLocalBounds());
    }
    else if ((L.getRotation() < R.getRotation()) || (L.getRotation() > R.getRotation()))
    {
        return (L.getRotation() < R.getRotation());
    }
    else
    {
        return (L.getTextureRect() < R.getTextureRect());
    }
}

bool operator==(const sf::Sprite & L, const sf::Sprite & R)
{
    return !(
        ((L.getGlobalBounds() < R.getGlobalBounds()) || (L.getGlobalBounds() > R.getGlobalBounds()))
        || (L.getTexture() != R.getTexture()) || (L.getColor() != R.getColor())
        || ((L.getLocalBounds() < R.getLocalBounds()) || (L.getLocalBounds() > R.getLocalBounds()))
        || ((L.getRotation() < R.getRotation()) || (L.getRotation() > R.getRotation()))
        || (L.getRotation() < R.getRotation()));
}

} // namespace sf
