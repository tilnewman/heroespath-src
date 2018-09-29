// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// image-manip.cpp
//
#include "image-manip.hpp"

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace heroespath
{
namespace sfutil
{

    void FlipHoriz(sf::Texture & texture)
    {
        sf::Image tempImage(texture.copyToImage());
        tempImage.flipHorizontally();
        texture.loadFromImage(tempImage);
    }

    void FlipHorizCopy(const sf::Texture & ORIG_TEXTURE, sf::Texture & resultTexture)
    {
        sf::Image tempImage(ORIG_TEXTURE.copyToImage());
        tempImage.flipHorizontally();
        resultTexture.loadFromImage(tempImage);
    }

    void FlipVert(sf::Texture & texture)
    {
        sf::Image tempImage(texture.copyToImage());
        tempImage.flipVertically();
        texture.loadFromImage(tempImage);
    }

    void FlipVertCopy(const sf::Texture & ORIG_TEXTURE, sf::Texture & resultTexture)
    {
        sf::Image tempImage(ORIG_TEXTURE.copyToImage());
        tempImage.flipVertically();
        resultTexture.loadFromImage(tempImage);
    }

    void Invert(sf::Image & image, const bool WILL_INVERT_ALPHA)
    {
        for (unsigned int y(0); y < image.getSize().y; ++y)
        {
            for (unsigned int x(0); x < image.getSize().x; ++x)
            {
                sf::Color color(image.getPixel(x, y));
                color.r = static_cast<sf::Uint8>(255 - static_cast<int>(color.r));
                color.g = static_cast<sf::Uint8>(255 - static_cast<int>(color.g));
                color.b = static_cast<sf::Uint8>(255 - static_cast<int>(color.b));

                if (WILL_INVERT_ALPHA)
                {
                    color.a = static_cast<sf::Uint8>(255 - static_cast<int>(color.a));
                }

                image.setPixel(x, y, color);
            }
        }
    }

    void Invert(sf::Texture & texture, const bool WILL_INVERT_ALPHA)
    {
        sf::Image image(texture.copyToImage());
        Invert(image, WILL_INVERT_ALPHA);
        texture.loadFromImage(image);
    }

    void Mask(sf::Texture & texture, const sf::Color & COLOR_TO_MASK, const sf::Uint8 NEW_ALPHA)
    {
        sf::Image image(texture.copyToImage());
        image.createMaskFromColor(COLOR_TO_MASK, NEW_ALPHA);
        texture.loadFromImage(image);
    }

    void SpriteToRenderTexture(
        sf::Sprite & sprite,
        sf::RenderTexture & renderTexture,
        const sf::RenderStates & RENDER_STATES,
        const bool WILL_DISPLAY)
    {
        auto const WIDTH { static_cast<unsigned int>(sprite.getGlobalBounds().width) };
        auto const HEIGHT { static_cast<unsigned int>(sprite.getGlobalBounds().height) };
        renderTexture.create(WIDTH, HEIGHT);

        const sf::Vector2f ORIG_POS(sprite.getPosition());
        sprite.setPosition(0.0f, 0.0f);
        renderTexture.draw(sprite, RENDER_STATES);

        if (WILL_DISPLAY)
        {
            renderTexture.display();
        }

        sprite.setPosition(ORIG_POS);
    }

    void SpriteToImage(
        sf::Sprite & sprite, sf::Image & image, const sf::RenderStates & RENDER_STATES)
    {
        sf::RenderTexture renderTexture;
        SpriteToRenderTexture(sprite, renderTexture, RENDER_STATES, true);
        image = renderTexture.getTexture().copyToImage();
    }

} // namespace sfutil
} // namespace heroespath
