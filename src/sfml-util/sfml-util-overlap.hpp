// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_SFMLUTIL_OVERLAP_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_SFMLUTIL_OVERLAP_HPP_INCLUDED
//
// sfml-util-overlap.hpp
//
#include <SFML/Graphics/Sprite.hpp>

namespace sf
{

// returns true if the two sprites intersect (global)
inline bool intersects(const sf::Sprite & A, const sf::Sprite & B)
{
    return A.getGlobalBounds().intersects(B.getGlobalBounds());
}

// returns true if the sprite intersects with the rect (global)
template <typename T>
bool intersects(const sf::Sprite & A, const sf::Rect<T> & R)
{
    return A.getGlobalBounds().intersects(sf::FloatRect(R));
}

} // namespace sf

#endif // HEROESPATH_SFMLUTIL_SFMLUTIL_OVERLAP_HPP_INCLUDED
