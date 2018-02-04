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
#ifndef HEROESPATH_SFMLUTIL_TILEMAP_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_TILEMAP_HPP_INCLUDED
//
// tilemap.hpp
//
#include "sfml-util/sfml-graphics.hpp"
#include "map/parser.hpp"
#include "map/layout.hpp"
#include "map/layer.hpp"
#include "map/tiles-panel.hpp"
#include "map/tile-offsets.hpp"

#include "misc/types.hpp"

#include <string>


namespace heroespath
{
namespace sfml_util
{

    //Encapsulates a tiled map, along with the player's position.
    class TileMap
    {
        TileMap(const TileMap &) =delete;
        TileMap & operator=(const TileMap &) =delete;

    public:
        TileMap(
            const std::string & MAP_PATH_STR,
            const sf::Vector2f & WIN_POS_V,
            const sf::Vector2u & WIN_SIZE_V,
            const sf::Vector2f & PLAYER_POS_V);

        bool MoveUp(const float ADJUSTMENT);
        bool MoveDown(const float ADJUSTMENT);
        bool MoveLeft(const float ADJUSTMENT);
        bool MoveRight(const float ADJUSTMENT);

        void Draw(sf::RenderTarget &, const sf::RenderStates &);

    private:
        void SetupMapSprite();

        void EstablishMapSubsection(map::Layer &, const map::TileOffsets &);

        const map::TileOffsets GetTileOffsetsFromMapPos(const sf::Vector2f & MAP_POS_V) const;
        const map::TileOffsets GetTileOffsetsOfPlayerPos() const;
        const sf::Vector2f GetPlayerPosCollision() const;
        const sf::Vector2f GetPosScreen(const map::TileOffsets &) const;
        const sf::Vector2f GetPosScreen(const sf::Vector2f &) const;
        const sf::Vector2f GetPlayerPosScreen() const;

        bool IsPointWithinCollision(const sf::Vector2f &) const;

        const map::TilesPanel & GetTilesPanelFromId(const int) const;

        void DrawPlayer(sf::RenderTarget & target);

        void AdjustOffscreenRectsToPreventDrift(const map::TileOffsets &);

    public:
        //This is how close the player position can get to
        //the edge of the map before being forced to stop.
        static const float BORDER_PAD_;

        //how many tiles to draw offscreen that are outside the visible map area
        static const int EXTRA_OFFSCREEN_TILE_COUNT_;

    private:
        map::Parser        mapParser_;
        map::Layout        mapLayout_;
        map::TileOffsets   prevTileOffsets_;
        const sf::Vector2f WIN_POS_V_;
        const sf::Vector2u WIN_SIZE_V_;
        sf::Vector2f       playerPosMapV_;
        sf::Vector2f       playerPosOffsetScreenV_;
        sf::RenderStates   renderStates_;
        sf::FloatRect      offScreenRect_;
        sf::Sprite         mapSprite_;
        sf::RenderTexture  offScreenTexture_;
    };

    using TileMapUPtr_t = std::unique_ptr<TileMap>;

}
}

#endif //HEROESPATH_SFMLUTIL_TILEMAP_HPP_INCLUDED
