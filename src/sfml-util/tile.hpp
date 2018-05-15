// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_TILE_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_TILE_HPP_INCLUDED
//
// tile.hpp
//  Tile drawing utility functions
//
#include "misc/random.hpp"
#include "misc/vectors.hpp"
#include "sfml-graphics.hpp"

namespace heroespath
{
namespace sfml_util
{

    // Slower version
    // Draw a tile image from origin to the given width and hieght.
    // If their sizes are not evenly divisible with the given
    // tileSprite's dimmensions then this function will draw passed
    // the given width and hieght.
    // Must call .display() on the renderTarget after.
    template <typename RenderTarget_t, typename Size_t>
    void Tile(
        const Size_t WIN_WIDTH,
        const Size_t WIN_HEIGHT,
        sf::Sprite & tileSprite,
        RenderTarget_t & renderTarget)
    {
        auto const TILE_SPRITE_BOUNDS{ tileSprite.getLocalBounds() };
        auto const COUNT_X{
            static_cast<std::size_t>(static_cast<float>(WIN_WIDTH) / TILE_SPRITE_BOUNDS.width) + 1
        };
        auto const COUNT_Y{
            static_cast<std::size_t>(static_cast<float>(WIN_HEIGHT) / TILE_SPRITE_BOUNDS.height) + 1
        };

        for (std::size_t iY(0); iY < COUNT_Y; ++iY)
        {
            for (std::size_t iX(0); iX < COUNT_X; ++iX)
            {
                auto const POS_X{ static_cast<float>(iX) * TILE_SPRITE_BOUNDS.width };
                auto const POS_Y{ static_cast<float>(iY) * TILE_SPRITE_BOUNDS.height };
                tileSprite.setPosition(POS_X, POS_Y);
                renderTarget.draw(tileSprite);
            }
        }
    }

    // Faster versions of the tile drawing functions
    // sprite will be modified to the params given
    // The sf::Texture used to create the sprite must have setRepeated(true) called.
    // Must call .display() on the renderTarget after.
    template <typename RenderTarget_t, typename Pos_t, typename Size_t>
    void Tile2(
        const Pos_t START_X,
        const Pos_t START_Y,
        const Size_t WIDTH,
        const Size_t HEIGHT,
        sf::Sprite & sprite,
        RenderTarget_t & renderTarget,
        const sf::RenderStates & STATES)
    {
        sprite.setTextureRect(sf::IntRect(0, 0, static_cast<int>(WIDTH), static_cast<int>(HEIGHT)));
        sprite.setPosition(static_cast<float>(START_X), static_cast<float>(START_Y));
        renderTarget.draw(sprite, STATES);
    }
    //
    template <typename RenderTarget_t, typename Rect_t>
    void Tile2(
        const sf::Rect<Rect_t> & RECT,
        sf::Sprite & sprite,
        RenderTarget_t & renderTarget,
        const sf::RenderStates & STATES)
    {
        Tile2(
            static_cast<float>(RECT.left),
            static_cast<float>(RECT.top),
            static_cast<int>(RECT.width),
            static_cast<int>(RECT.height),
            sprite,
            renderTarget,
            STATES);
    }

    // Tiles with the given textures in order or at random.
    // Each tile must be the same size or gaps and sheering will appear.
    // If RECT.width or RECT.height are not evenly divisible by the texture
    // size then a gap will appear rather than drawing past the boundaries.
    // RenderTarget_t can be either an sf::Window or an sf::RenderTexture,
    // anything that implements a draw() function.
    // TODO keep track of current sprite position x and y to allow for different sized textures
    // Must call .display() on the renderTarget after.
    template <typename RenderTarget_t>
    void TileFromVec(
        const sf::FloatRect & RECT,
        SpriteVec_t & spriteVec,
        RenderTarget_t & renderTarget,
        const bool WILL_RANDOM_ORDER = false)
    {
        if (spriteVec.empty())
        {
            std::cout << "sfml-util::TileFromVec() was given an empty vector!" << std::endl;
            return;
        }

        if (WILL_RANDOM_ORDER)
        {
            misc::Vector::ShuffleVec(spriteVec);
        }

        const std::size_t SPRITE_COUNT(spriteVec.size());

        auto const SPRITE_WIDTH{ spriteVec[0].getLocalBounds().width };
        auto const SPRITE_HEIGHT{ spriteVec[0].getLocalBounds().height };

        auto const COUNT_X{ static_cast<std::size_t>(RECT.width / SPRITE_WIDTH) };
        auto const COUNT_Y{ static_cast<std::size_t>(RECT.height / SPRITE_HEIGHT) };

        std::size_t spriteVecIndex(0);
        for (std::size_t x(0); x < COUNT_X; ++x)
        {
            for (std::size_t y(0); y < COUNT_Y; ++y)
            {
                auto const POS_X{ (static_cast<float>(x) * SPRITE_WIDTH)
                                  + static_cast<float>(RECT.left) };

                auto const POS_Y{ (static_cast<float>(y) * SPRITE_HEIGHT)
                                  + static_cast<float>(RECT.top) };

                spriteVec[spriteVecIndex].setPosition(POS_X, POS_Y);

                renderTarget.draw(spriteVec[spriteVecIndex]);

                if (++spriteVecIndex >= SPRITE_COUNT)
                {
                    spriteVecIndex = 0;

                    if (WILL_RANDOM_ORDER)
                    {
                        misc::Vector::ShuffleVec(spriteVec);
                    }
                }
            }
        }
    }

    // See comments above
    template <typename RenderTarget_t>
    void TileFromVec(
        const sf::FloatRect & RECT,
        TextureVec_t & textureVec,
        RenderTarget_t & renderTarget,
        const bool WILL_RANDOM_ORDER = false)
    {
        SpriteVec_t spriteVec;
        spriteVec.reserve(textureVec.size());
        for (auto const & NEXT_TEXTURE : textureVec)
        {
            spriteVec.emplace_back(sf::Sprite(NEXT_TEXTURE));
        }

        TileFromVec(RECT, spriteVec, renderTarget, WILL_RANDOM_ORDER);
    }
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_TILE_HPP_INCLUDED
