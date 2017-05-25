///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef SFMLUTIL_TILE_INCLUDED
#define SFMLUTIL_TILE_INCLUDED
//
// tile.hpp
//  Tile drawing utility functions
//
#include "sfml-graphics.hpp"
#include "utilz/random.hpp"
#include "utilz/vectors.hpp"


namespace sfml_util
{

    //Slower version
    //Draw a tile image from origin to the given width and hieght.
    //If their sizes are not evenly divisible with the given
    //tileSprite's dimmensions then this function will draw passed
    //the given width and hieght.
    //Must call .display() on the renderTarget after.
    template<typename RenderTarget_t, typename Size_t>
    void Tile(  const Size_t     WIN_WIDTH,
                const Size_t     WIN_HEIGHT,
                sf::Sprite &     tileSprite,
                RenderTarget_t & renderTarget )
    {
        auto const TILE_SPRITE_BOUNDS{ tileSprite.getLocalBounds() };
        const std::size_t COUNT_X(static_cast<std::size_t>(static_cast<float>(WIN_WIDTH) / TILE_SPRITE_BOUNDS.width) + 1);
        const std::size_t COUNT_Y(static_cast<std::size_t>(static_cast<float>(WIN_HEIGHT) / TILE_SPRITE_BOUNDS.height) + 1);

        for (std::size_t iY(0); iY<COUNT_Y; ++iY)
        {
            for (std::size_t iX(0); iX<COUNT_X; ++iX)
            {
                const float POS_X(static_cast<float>(iX) * TILE_SPRITE_BOUNDS.width);
                const float POS_Y(static_cast<float>(iY) * TILE_SPRITE_BOUNDS.height);
                tileSprite.setPosition(POS_X, POS_Y);
                renderTarget.draw(tileSprite);
            }
        }
    }


    //Faster versions of the tile drawing functions
    //sprite will be modified to the params given
    //The sf::Texture used to create the sprite must have setRepeated(true) called.
    //Must call .display() on the renderTarget after.
    template<typename RenderTarget_t, typename Pos_t, typename Size_t>
    void Tile2( const Pos_t      START_X,
                const Pos_t      START_Y,
                const Size_t     WIDTH,
                const Size_t     HEIGHT,
                sf::Sprite &     sprite,
                RenderTarget_t & renderTarget,
                const sf::RenderStates & STATES)
    {
        sprite.setTextureRect(sf::IntRect(0, 0, static_cast<int>(WIDTH), static_cast<int>(HEIGHT)));
        sprite.setPosition(static_cast<float>(START_X), static_cast<float>(START_Y));
        renderTarget.draw(sprite, STATES);
    }
    //
    template<typename RenderTarget_t, typename Rect_t>
    void Tile2(const sf::Rect<Rect_t> & RECT,
               sf::Sprite &             sprite,
               RenderTarget_t &         renderTarget,
               const sf::RenderStates & STATES)
    {
        Tile2(  static_cast<float>(RECT.left),
                static_cast<float>(RECT.top),
                static_cast<int>(RECT.width),
                static_cast<int>(RECT.height),
                sprite,
                renderTarget,
                STATES);
    }


    //Tiles with the given textures in order or at random.
    //Each tile must be the same size or gaps and sheering will appear.
    //If RECT.width or RECT.height are not evenly divisible by the texture
    //size then a gap will appear rather than drawing past the boundaries.
    //RenderTarget_t can be either an sf::Window or an sf::RenderTexture,
    //anything that implements a draw() function.
    //TODO keep track of current sprite position x and y to allow for different sized textures
    //Must call .display() on the renderTarget after.
    template<typename RenderTarget_t>
    void TileFromVec(const sf::FloatRect & RECT,
                     SpriteSVec_t &        spriteSVec,
                     RenderTarget_t &      renderTarget,
                     const bool            WILL_RANDOM_ORDER = false)
    {
        if (spriteSVec.empty())
        {
            std::cout << "sfml-util::TileFromVec() was given an empty vector!" << std::endl;
            return;
        }

        if (WILL_RANDOM_ORDER)
            utilz::Vector::ShuffleVec(spriteSVec);

        const std::size_t SPRITE_COUNT(spriteSVec.size());

        const float SPRITE_WIDTH (spriteSVec[0]->getLocalBounds().width);
        const float SPRITE_HEIGHT(spriteSVec[0]->getLocalBounds().height);

        const std::size_t COUNT_X(static_cast<std::size_t>(RECT.width / SPRITE_WIDTH));
        const std::size_t COUNT_Y(static_cast<std::size_t>(RECT.height / SPRITE_HEIGHT));

        std::size_t spriteVecIndex(0);
        for (std::size_t x(0); x < COUNT_X; ++x)
        {
            for (std::size_t y(0); y < COUNT_Y; ++y)
            {
                const float POS_X((static_cast<float>(x) * SPRITE_WIDTH)  + static_cast<float>(RECT.left));
                const float POS_Y((static_cast<float>(y) * SPRITE_HEIGHT) + static_cast<float>(RECT.top));

                spriteSVec[spriteVecIndex]->setPosition(POS_X, POS_Y);

                renderTarget.draw( * spriteSVec[spriteVecIndex] );

                if (++spriteVecIndex >= SPRITE_COUNT)
                {
                    spriteVecIndex = 0;

                    if (WILL_RANDOM_ORDER)
                        utilz::Vector::ShuffleVec(spriteSVec);
                }
            }
        }
    }
    //See comments above
    template<typename RenderTarget_t>
    void TileFromVec(const sf::FloatRect & RECT,
                     TextureSVec_t &       textureSVec,
                     RenderTarget_t &      renderTarget,
                     const bool            WILL_RANDOM_ORDER = false)
    {
        SpriteSVec_t spriteSVec;
        for (auto const & NEXT_TEXTURE_SPTR : textureSVec)
            spriteSVec.push_back( std::shared_ptr<sf::Sprite>(new sf::Sprite( * NEXT_TEXTURE_SPTR)) );

        TileFromVec(RECT, spriteSVec, renderTarget, WILL_RANDOM_ORDER);
    }

}
#endif //SFMLUTIL_TILE_INCLUDED
