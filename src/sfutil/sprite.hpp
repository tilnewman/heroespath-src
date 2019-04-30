// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFUTIL_SPRITE_HPP_INCLUDED
#define HEROESPATH_SFUTIL_SPRITE_HPP_INCLUDED
//
// sprite.hpp
//

namespace sf
{

class Sprite;

bool operator<(const sf::Sprite & L, const sf::Sprite & R);
bool operator==(const sf::Sprite & L, const sf::Sprite & R);
inline bool operator!=(const sf::Sprite & L, const sf::Sprite & R) { return !(L == R); }

} // namespace sf

#endif // HEROESPATH_SFUTIL_SPRITE_HPP_INCLUDED
