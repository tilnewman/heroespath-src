// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFUTIL_SPRITE_TEXTURE_HPP_INCLUDED
#define HEROESPATH_SFUTIL_SPRITE_TEXTURE_HPP_INCLUDED
//
// sprite-texture.hpp
//
#include "misc/boost-optional-that-throws.hpp"
#include "sfutil/color.hpp"
#include "sfutil/common.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace sf
{

bool operator<(const sf::Sprite & L, const sf::Sprite & R);
bool operator==(const sf::Sprite & L, const sf::Sprite & R);
bool operator!=(const sf::Sprite & L, const sf::Sprite & R);

} // namespace sf

namespace heroespath
{

using SpriteOpt_t = boost::optional<sf::Sprite>;
using TextureOpt_t = boost::optional<sf::Texture>;

} // namespace heroespath

#endif // HEROESPATH_SFUTIL_SPRITE_HPP_INCLUDED
