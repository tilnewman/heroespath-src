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
#include "sfml-util/animation-base.hpp"
#include "sfml-util/animation-factory.hpp"

#include "map/layout.hpp"
#include "map/layer.hpp"
#include "map/tiles-panel.hpp"
#include "map/tile-offsets.hpp"
#include "map/map-anim.hpp"
#include "misc/types.hpp"

#include <string>


namespace heroespath
{
namespace map
{

    class Map;


    //Encapsulates a tiled map, along with the player's position.
    class MapDisplay : public sf::Drawable
    {
        MapDisplay(const MapDisplay &) = delete;
        MapDisplay & operator=(const MapDisplay &) = delete;

    public:
        MapDisplay(const Map & MAP, const sf::FloatRect & REGION);

        void Load(const sf::Vector2f & STARTING_POS_V, const MapAnimVec_t &);

        bool Move(const sfml_util::Direction::Enum, const float ADJUSTMENT);

        virtual void draw(sf::RenderTarget &, sf::RenderStates) const override;

        inline const sf::Vector2f PlayerPosMap() const { return playerPosV_ + playerPosOffsetV_; }

        Layout & GetLayoutRef() { return layout_; }

        void Update(const float TIME_ELAPSED);

    private:
        bool MoveUp(const float ADJUSTMENT);
        bool MoveDown(const float ADJUSTMENT);
        bool MoveLeft(const float ADJUSTMENT);
        bool MoveRight(const float ADJUSTMENT);

        void DrawNormal(sf::RenderTarget &, sf::RenderStates) const;
        void DrawDebug(sf::RenderTarget &, sf::RenderStates) const;
        void DrawCharacterImages();
        void UpdateAndDrawAnimations(const float TIME_ELAPSED);
        void ReDraw(const float TIME_ELAPSED);
        void ResetMapSubsections();
        void DrawMapSubsectionOffscreen();

        void PositionMapSpriteTextureRect();

        void SetupOffScreenTexture();

        void SetupMapLayerSubsection(Layer &, const TileOffsets &);

        const TileOffsets TileOffsetsFromMapPos(const sf::Vector2f & MAP_POS_V) const;

        const sf::Vector2f PlayerPosScreen() const;

        const sf::Vector2f ScreenPosFromMapPos(const sf::Vector2f &) const;
        const sf::Vector2f OffScreenPosFromMapPos(const sf::Vector2f &) const;

        const TilesPanel & TilesPanelFromId(const int) const;

        void IncrementTileOffsetsInDirection(const sfml_util::Direction::Enum);

        const sf::Vector2f CalcOffScreenMapSize() const;

        void SetupNPCShadowImage();

        void SetupAnimations();

    public:
        //how many extra tiles to draw offscreen that are outside the visible map area
        static const int EXTRA_OFFSCREEN_TILE_COUNT_;

        static const std::size_t VERTS_PER_QUAD_;

    private:
        const Map & MAP_;

        //This is how close the player position can get to
        //the edge of the map before being forced to stop.
        const float BORDER_PAD_;

        const sf::Vector2f WIN_POS_V_;
        const sf::Vector2f WIN_SIZE_V_;
        const sf::Vector2f WIN_CENTER_V_;
        Layout             layout_;
        TileOffsets        tileOffsets_;
        sf::Vector2f       playerPosV_;
        sf::Vector2f       playerPosOffsetV_;
        sf::FloatRect      offScreenRect_;
        sf::Sprite         offScreenSpriteAbove_;
        sf::Sprite         offScreenSpriteBelow_;
        sf::RenderTexture  offScreenTextureAbove_;
        sf::RenderTexture  offScreenTextureBelow_;
        sf::Vector2f       offScreenMapSize_;
        sf::Texture        npcShadowTexture_;
        sf::Sprite         npcShadowSprite_;

        MapAnimVec_t animInfoVec_;
        std::vector<sfml_util::AnimationUPtr_t> animUPtrVec_;
    };

    using MapDisplayUPtr_t = std::unique_ptr<MapDisplay>;
}
}

#endif //HEROESPATH_MAP_MAPDISPLAY_HPP_INCLUDED
