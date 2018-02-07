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
#ifndef HEROESPATH_MAP_MAPDISPLAY_HPP_INCLUDED
#define HEROESPATH_MAP_MAPDISPLAY_HPP_INCLUDED
//
// map-display.hpp
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/direction-enum.hpp"
#include "sfml-util/collision-quad-tree.hpp"

#include "map/layout.hpp"
#include "map/layer.hpp"
#include "map/tiles-panel.hpp"
#include "map/tile-offsets.hpp"
#include "misc/types.hpp"

#include <string>


namespace heroespath
{
namespace map
{

    //Encapsulates a tiled map, along with the player's position.
    class MapDisplay : public sf::Drawable
    {
        MapDisplay(const MapDisplay &) = delete;
        MapDisplay & operator=(const MapDisplay &) = delete;

    public:
        MapDisplay(const sf::Vector2f & WIN_POS_V, const sf::Vector2f & WIN_SIZE_V);

        void Load(
            const std::string & MAP_FILE_PATH_STR,
            const sf::Vector2f & STARTING_POS_V,
            sfml_util::QuadTree & collisionQTree);

        bool MoveUp(const float ADJUSTMENT);
        bool MoveDown(const float ADJUSTMENT);
        bool MoveLeft(const float ADJUSTMENT);
        bool MoveRight(const float ADJUSTMENT);

        virtual void draw(sf::RenderTarget &, sf::RenderStates) const override;

        inline const sf::Vector2f PlayerPosMap() const { return playerPosV_ + playerPosOffsetV_; }

    private:
        void DrawNormal(sf::RenderTarget &, sf::RenderStates) const;
        void DrawDebug(sf::RenderTarget &, sf::RenderStates) const;
        void DrawPlayerImage(sf::RenderTarget &, const sf::Vector2f &) const;
        void ReDraw();
        void ResetMapSubsections();
        void DrawMapSubsectionOffscreen();

        void PositionMapSpriteTextureRect();

        void SetupOffScreenTexture();

        void SetupMapLayerSubsection(map::Layer &, const map::TileOffsets &);

        const map::TileOffsets TileOffsetsFromMapPos(const sf::Vector2f & MAP_POS_V) const;

        const sf::Vector2f PlayerPosScreen() const;

        const sf::Vector2f ScreenPosFromMapPos(const sf::Vector2f &) const;

        const map::TilesPanel & TilesPanelFromId(const int) const;

        void IncrementTileOffsetsInDirection(const sfml_util::Direction::Enum);

        const sf::Vector2f CalcOffScreenMapSize() const;

    public:
        //This is how close the player position can get to
        //the edge of the map before being forced to stop.
        static const float BORDER_PAD_;

        //how many tiles to draw offscreen that are outside the visible map area
        static const int EXTRA_OFFSCREEN_TILE_COUNT_;

    private:
        const sf::Vector2f WIN_POS_V_;
        const sf::Vector2f WIN_SIZE_V_;
        const sf::Vector2f WIN_CENTER_V_;
        map::Layout        layout_;
        map::TileOffsets   tileOffsets_;
        sf::Vector2f       playerPosV_;
        sf::Vector2f       playerPosOffsetV_;
        sf::FloatRect      offScreenRect_;
        sf::Sprite         offScreenSprite_;
        sf::RenderTexture  offScreenTexture_;
        sf::Vector2f       offScreenMapSize_;
    };

    using MapDisplayUPtr_t = std::unique_ptr<MapDisplay>;
}
}

#endif //HEROESPATH_MAP_MAPDISPLAY_HPP_INCLUDED
