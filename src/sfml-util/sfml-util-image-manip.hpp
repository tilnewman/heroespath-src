// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_SFMLUTIL_IMAGE_MANIP_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_SFMLUTIL_IMAGE_MANIP_HPP_INCLUDED
//
// sfml-util-image-manip.hpp
//
#include <SFML/Config.hpp>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderStates.hpp>

namespace sf
{
class Sprite;
class Texture;
class Image;
class RenderTexture;
class RenderTarget;
} // namespace sf

namespace heroespath
{
namespace sfml_util
{

    void FlipHoriz(sf::Texture &);
    void FlipHorizCopy(const sf::Texture & ORIG_TEXTURE, sf::Texture & resultTexture);

    void FlipVert(sf::Texture &);
    void FlipVertCopy(const sf::Texture & ORIG_TEXTURE, sf::Texture & resultTexture);

    void Invert(sf::Image & image, const bool WILL_INVERT_ALPHA = false);

    void Invert(sf::Texture & texture, const bool WILL_INVERT_ALPHA = false);

    // changes the alpha value of all colors in texture that match COLOR_TO_MASK to NEW_ALPHA
    void Mask(
        sf::Texture & texture,
        const sf::Color & COLOR_TO_MASK = sf::Color::Black,
        const sf::Uint8 NEW_ALPHA = 0);

    void SpriteToRenderTexture(
        sf::Sprite & sprite,
        sf::RenderTexture & renderTexture,
        const sf::RenderStates & RENDER_STATES = sf::RenderStates(),
        const bool WILL_DISPLAY = true);

    void SpriteToImage(
        sf::Sprite & sprite,
        sf::Image & image,
        const sf::RenderStates & RENDER_STATES = sf::RenderStates());

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_SFMLUTIL_IMAGE_MANIP_HPP_INCLUDED
