//This is an open source non-commercial project. Dear PVS-Studio, please check it.
//PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
///////////////////////////////////////////////////////////////////////////////
//
//Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
//Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
//This software is provided 'as-is', without any express or implied warranty.
//In no event will the authors be held liable for any damages arising from
//the use of this software.
//
//Permission is granted to anyone to use this software for any purpose,
//including commercial applications, and to alter it and redistribute it
//freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software.  If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
//
//tilemap.cpp
//
#include "tilemap.hpp"

#include "sfml-util/sfml-util.hpp"
#include "misc/assertlogandthrow.hpp"

#include <boost/algorithm/algorithm.hpp>
#include <boost/algorithm/string.hpp>

#include <sstream>
#include <exception>


namespace heroespath
{
namespace sfml_util
{

    const float TileMap::BORDER_PAD_                { 75.0f };
    const int   TileMap::EXTRA_OFFSCREEN_TILE_COUNT_{ 2 };


    TileMap::TileMap(const sf::Vector2f & WIN_POS_V, const sf::Vector2f & WIN_SIZE_V)
    :
        WIN_POS_V_       (WIN_POS_V),
        WIN_SIZE_V_      (WIN_SIZE_V),
        WIN_CENTER_V_    (
            WIN_POS_V_.x + (WIN_SIZE_V_.x * 0.5f),
            WIN_POS_V_.y + (WIN_SIZE_V_.y * 0.5f)),
        mapLayout_       (),
        tileOffsets_     (),
        playerPosV_      (0.0f, 0.0f),
        playerPosOffsetV_(0.0f, 0.0f),
        offScreenRect_   (0.0f, 0.0f, 0.0f, 0.0f),
        mapSprite_       (),
        offScreenTexture_(),
        offScreenMapSize_(0.0f, 0.0f)
    {}


    void TileMap::Load(const std::string & MAP_FILE_PATH_STR, const sf::Vector2f & PLAYER_POS_V)
    {
        mapLayout_.Reset();
        tileOffsets_ = map::TileOffsets();
        playerPosOffsetV_ = sf::Vector2f(0.0f, 0.0f);

        playerPosV_ = PLAYER_POS_V;
        
        map::Parser mapParser;
        mapParser.Parse(MAP_FILE_PATH_STR, mapLayout_);

        tileOffsets_ = GetTileOffsetsFromMapPos(PLAYER_POS_V);

        offScreenMapSize_ = CalcOffScreenMapSize();

        SetupOffScreenTexture();
        ReDraw();
    }


    bool TileMap::MoveUp(const float ADJUSTMENT)
    {
        if (DoesAdjPlayerPosCollide(Direction::Up, ADJUSTMENT))
        {
            return false;
        }

        auto const CURR_SCREEN_POS{ GetPlayerPosScreen() };

        if (CURR_SCREEN_POS.y > (WIN_POS_V_.y + BORDER_PAD_))
        {
            //move within the view before moving the view
            playerPosOffsetV_.y -= ADJUSTMENT;
        }
        else
        {
            if (offScreenRect_.top > 0.0f)
            {
                offScreenRect_.top = std::max(0.0f, offScreenRect_.top - ADJUSTMENT);
                PositionMapSpriteTextureRect();
                playerPosV_.y = std::max(0.0f, playerPosV_.y - ADJUSTMENT);

                if (misc::IsRealZero(offScreenRect_.top))
                {
                    IncrementTileOffsetsInDirection(Direction::Up);
                }
            }
            else
            {
                return false;
            }
        }

        return true;
    }


    bool TileMap::MoveDown(const float ADJUSTMENT)
    {
        if (DoesAdjPlayerPosCollide(Direction::Down, ADJUSTMENT))
        {
            return false;
        }

        auto const CURR_SCREEN_POS{ GetPlayerPosScreen() };

        if (CURR_SCREEN_POS.y < ((WIN_POS_V_.y + WIN_SIZE_V_.y) - BORDER_PAD_))
        {
            //move within the view before moving the view
            playerPosOffsetV_.y += ADJUSTMENT;
        }
        else
        {
            auto const VERT_POS{ offScreenRect_.top +
                static_cast<float>(tileOffsets_.begin_v.y * mapLayout_.tile_size_v.y) };

            auto const VERT_LIMIT{
                static_cast<float>(mapLayout_.tile_count_v.y * mapLayout_.tile_size_v.y) -
                    static_cast<float>(WIN_SIZE_V_.y) };

            if (VERT_POS >= VERT_LIMIT)
            {
                return false;
            }
            else
            {
                offScreenRect_.top += ADJUSTMENT;
                PositionMapSpriteTextureRect();
                playerPosV_.y += ADJUSTMENT;

                if ((offScreenRect_.top + offScreenRect_.height) > offScreenMapSize_.y)
                {
                    IncrementTileOffsetsInDirection(Direction::Down);
                }
            }
        }

        return true;
    }


    bool TileMap::MoveLeft(const float ADJUSTMENT)
    {
        if (DoesAdjPlayerPosCollide(Direction::Left, ADJUSTMENT))
        {
            return false;
        }

        auto const CURR_SCREEN_POS{ GetPlayerPosScreen() };

        if (CURR_SCREEN_POS.x > (WIN_POS_V_.x + BORDER_PAD_))
        {
            //move within the view before moving the view
            playerPosOffsetV_.x -= ADJUSTMENT;
        }
        else
        {
            if (offScreenRect_.left > 0.0f)
            {
                offScreenRect_.left = std::max(0.0f, offScreenRect_.left - ADJUSTMENT);
                PositionMapSpriteTextureRect();
                playerPosV_.x = std::max(0.0f, playerPosV_.x - ADJUSTMENT);

                if (misc::IsRealZero(offScreenRect_.left))
                {
                    IncrementTileOffsetsInDirection(Direction::Left);
                }
            }
            else
            {
                return false;
            }
        }

        return true;
    }


    bool TileMap::MoveRight(const float ADJUSTMENT)
    {
        if (DoesAdjPlayerPosCollide(Direction::Right, ADJUSTMENT))
        {
            return false;
        }

        auto const CURR_SCREEN_POS{ GetPlayerPosScreen() };

        if (CURR_SCREEN_POS.x < ((WIN_POS_V_.x + WIN_SIZE_V_.x) - BORDER_PAD_))
        {
            //move within the view before moving the view
            playerPosOffsetV_.x += ADJUSTMENT;
        }
        else
        {
            auto const HORIZ_POS{ (offScreenRect_.left + static_cast<float>(
                tileOffsets_.begin_v.x * mapLayout_.tile_size_v.x)) };

            auto const HORIZ_LIMIT{ static_cast<float>(
                (mapLayout_.tile_count_v.x * mapLayout_.tile_size_v.x) -
                    static_cast<int>(WIN_SIZE_V_.x)) };

            if (HORIZ_POS >= HORIZ_LIMIT)
            {
                return false;
            }
            else
            {
                offScreenRect_.left += ADJUSTMENT;
                PositionMapSpriteTextureRect();
                playerPosV_.x += ADJUSTMENT;

                if ((offScreenRect_.left + offScreenRect_.width) > offScreenMapSize_.x)
                {
                    IncrementTileOffsetsInDirection(Direction::Right);
                }
            }
        }

        return true;
    }


    void TileMap::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        DrawNormal(target, STATES);
    }


    void TileMap::DrawNormal(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        target.draw(mapSprite_, STATES);
        DrawPlayer(target);
    }


    void TileMap::DrawDebug(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        sf::Sprite offScreenSprite(offScreenTexture_.getTexture());
        offScreenSprite.setPosition(WIN_POS_V_);
        target.draw(offScreenSprite);

        sf::FloatRect offScreenInnerWindowRect(
            WIN_POS_V_.x + offScreenRect_.left,
            WIN_POS_V_.y + offScreenRect_.top,
            offScreenRect_.width,
            offScreenRect_.height);

        sfml_util::DrawRectangle(target, STATES, offScreenInnerWindowRect);
    }


    void TileMap::DrawPlayer(sf::RenderTarget & target)
    {
        sf::CircleShape c(5);
        c.setFillColor(sf::Color(255, 0, 0));
        c.setPosition(GetPlayerPosScreen());
        target.draw(c);
    }


    void TileMap::ReDraw()
    {
        ResetMapSubsections();
        DrawMapSubsectionOffscreen();
    }


    void TileMap::ResetMapSubsections()
    {
        for (auto & nextMapLayer : mapLayout_.layer_vec)
        {
            nextMapLayer.ResetForReDraw();
            SetupMapLayerSubsection(nextMapLayer, tileOffsets_);
        }
    }


    void TileMap::DrawMapSubsectionOffscreen()
    {
        offScreenTexture_.clear(sf::Color::Black);

        sf::RenderStates renderStates{ sf::RenderStates::Default };
        sf::VertexArray quads(sf::Quads, 4);

        //draw all vertex arrays to the off-screen texture
        for (auto const & LAYER : mapLayout_.layer_vec)
        {
            std::size_t tilesPanelIndex{ 0 };
            auto const NUM_VERTS{ LAYER.vert_array.getVertexCount() };
            for (std::size_t vertIndex(0); vertIndex < NUM_VERTS; )
            {
                //skip drawing empty tiles
                if (LAYER.tiles_panel_vec[tilesPanelIndex].name != mapLayout_.EmptyTilesPanelName())
                {
                    quads[0].position = LAYER.vert_array[vertIndex].position;
                    quads[0].texCoords = LAYER.vert_array[vertIndex++].texCoords;

                    quads[1].position = LAYER.vert_array[vertIndex].position;
                    quads[1].texCoords = LAYER.vert_array[vertIndex++].texCoords;

                    quads[2].position = LAYER.vert_array[vertIndex].position;
                    quads[2].texCoords = LAYER.vert_array[vertIndex++].texCoords;

                    quads[3].position = LAYER.vert_array[vertIndex].position;
                    quads[3].texCoords = LAYER.vert_array[vertIndex++].texCoords;

                    renderStates.texture = &mapLayout_.texture_vec[
                        LAYER.tiles_panel_vec[tilesPanelIndex++].texture_index];

                    offScreenTexture_.draw(quads, renderStates);
                }
                else
                {
                    ++tilesPanelIndex;
                    auto const VERTEXES_PER_QUAD{ 4 };
                    vertIndex += VERTEXES_PER_QUAD;
                }
            }
        }

        offScreenTexture_.display();
    }


    void TileMap::PositionMapSpriteTextureRect()
    {
        
        mapSprite_.setTextureRect(sfml_util::ConvertRect<float, int>(offScreenRect_));
    }


    void TileMap::SetupOffScreenTexture()
    {
        auto const EXTRA_WIDTH{ mapLayout_.tile_size_v.x  * (EXTRA_OFFSCREEN_TILE_COUNT_ * 2) };
        auto const WIDTH{ static_cast<unsigned>(WIN_SIZE_V_.x) + static_cast<unsigned>(EXTRA_WIDTH) };

        auto const EXTRA_HEIGHT{ mapLayout_.tile_size_v.y * (EXTRA_OFFSCREEN_TILE_COUNT_ * 2) };
        auto const HEIGHT{ static_cast<unsigned>(WIN_SIZE_V_.y) + static_cast<unsigned>(EXTRA_HEIGHT) };

        M_ASSERT_OR_LOGANDTHROW_SS(offScreenTexture_.create(WIDTH, HEIGHT),
            "sfml_util::TileMap::Load(), failed to sf::RenderTexture::create("
                << WIDTH << "x" << HEIGHT << ").");

        //set the initial position of what is drawn on-screen from the off-screen texture
        offScreenRect_.width = WIN_SIZE_V_.x;
        offScreenRect_.height = WIN_SIZE_V_.y;

        offScreenRect_.left =
            static_cast<float>(EXTRA_OFFSCREEN_TILE_COUNT_ * mapLayout_.tile_size_v.x);

        offScreenRect_.top =
            static_cast<float>(EXTRA_OFFSCREEN_TILE_COUNT_ * mapLayout_.tile_size_v.y);

        mapSprite_.setTexture(offScreenTexture_.getTexture());
        mapSprite_.setPosition(WIN_POS_V_);

        PositionMapSpriteTextureRect();
    }


    void TileMap::SetupMapLayerSubsection(
        map::Layer & mapLayer,
        const map::TileOffsets & TILE_OFFSETS)
    {
        //resize the vertex array to fit the level size
        auto const TILE_WIDTH{  TILE_OFFSETS.end_v.x - TILE_OFFSETS.begin_v.x };
        auto const TILE_HEIGHT{ TILE_OFFSETS.end_v.y - TILE_OFFSETS.begin_v.y };
        mapLayer.vert_array.setPrimitiveType(sf::Quads);
        const int VERTEXES_PER_QUAD{ 4 };

        mapLayer.vert_array.resize( static_cast<std::size_t>(
            (TILE_WIDTH * TILE_HEIGHT) * VERTEXES_PER_QUAD) );

        mapLayer.tiles_panel_vec.reserve( static_cast<std::size_t>(TILE_WIDTH * TILE_HEIGHT) );

        //populate the vertex array with one quad per tile
        unsigned vertIndex(0);
        for (int x(TILE_OFFSETS.begin_v.x); x < TILE_OFFSETS.end_v.x; ++x)
        {
            for (int y(TILE_OFFSETS.begin_v.y); y < TILE_OFFSETS.end_v.y; ++y)
            {
                //get the current tile number
                auto const TILE_INDEX{ x + (y * mapLayout_.tile_count_v.x) };

                auto const TILE_NUM_ORIG{
                    mapLayer.mapid_vec[static_cast<std::size_t>(TILE_INDEX)] };

                //This -1 comes from the Tiled app that starts tile ids at 1 instead of 0.
                auto const TILE_NUM_ID_ADJ{
                    (TILE_NUM_ORIG == 0) ? 0 : (TILE_NUM_ORIG - 1) };

                //get the texture/image this tile can be found in
                const map::TilesPanel & TILES_PANEL{ GetTilesPanelFromId(TILE_NUM_ID_ADJ) };
                mapLayer.tiles_panel_vec.push_back(TILES_PANEL);

                //adjust the tile number to start at one
                auto const TILE_ID{ (TILE_NUM_ID_ADJ - TILES_PANEL.first_id) + 1 };

                //get a pointer to the current tile's quad
                auto const VERT_ARRAY_INDEX{ static_cast<std::size_t>((vertIndex++) * 4) };
                sf::Vertex * quadPtr{ & mapLayer.vert_array[VERT_ARRAY_INDEX] };

                //define its 4 corners on screen
                auto const POS_WIDTH{
                    static_cast<float>((x - TILE_OFFSETS.begin_v.x) * mapLayout_.tile_size_v.x) };

                auto const POS_WIDTH_PLUS_ONE_TILE{
                    POS_WIDTH + static_cast<float>(mapLayout_.tile_size_v.x) };

                auto const POS_HEIGHT{
                    static_cast<float>((y - TILE_OFFSETS.begin_v.y) * mapLayout_.tile_size_v.y) };

                auto const POS_HEIGHT_PLUS_ONE_TILE{
                    POS_HEIGHT + static_cast<float>(mapLayout_.tile_size_v.y) };

                quadPtr[0].position =
                    sf::Vector2f(POS_WIDTH, POS_HEIGHT);

                quadPtr[1].position =
                    sf::Vector2f(POS_WIDTH_PLUS_ONE_TILE, POS_HEIGHT);

                quadPtr[2].position =
                    sf::Vector2f(POS_WIDTH_PLUS_ONE_TILE, POS_HEIGHT_PLUS_ONE_TILE);

                quadPtr[3].position =
                    sf::Vector2f(POS_WIDTH, POS_HEIGHT_PLUS_ONE_TILE);

                //find its position in the tileset texture
                auto const TEXTURE_TILE_COUNT_HORIZ{ static_cast<int>(
                    mapLayout_.texture_vec[TILES_PANEL.texture_index].getSize().x) / mapLayout_.tile_size_v.x };

                auto const TILE_COUNT_U{
                    static_cast<float>(TILE_ID % TEXTURE_TILE_COUNT_HORIZ) };

                auto const TILE_COUNT_V{
                    static_cast<float>(TILE_ID / TEXTURE_TILE_COUNT_HORIZ) };

                auto const TEXTCOORD_WIDTH{
                    TILE_COUNT_U * static_cast<float>(mapLayout_.tile_size_v.x) };

                auto const TEXTCOORD_WIDTH_PLUS_ONE_TILE{
                    TEXTCOORD_WIDTH + static_cast<float>(mapLayout_.tile_size_v.x) };

                auto const TEXTCOORD_HEIGHT{
                    TILE_COUNT_V * static_cast<float>(mapLayout_.tile_size_v.y) };

                auto const TEXTCOORD_HEIGHT_PLUS_ONE_TILE{
                    TEXTCOORD_HEIGHT + static_cast<float>(mapLayout_.tile_size_v.y) };

                //define its 4 texture coordinates
                quadPtr[0].texCoords =
                    sf::Vector2f(TEXTCOORD_WIDTH, TEXTCOORD_HEIGHT);

                quadPtr[1].texCoords =
                    sf::Vector2f(TEXTCOORD_WIDTH_PLUS_ONE_TILE, TEXTCOORD_HEIGHT);

                quadPtr[2].texCoords =
                    sf::Vector2f(TEXTCOORD_WIDTH_PLUS_ONE_TILE, TEXTCOORD_HEIGHT_PLUS_ONE_TILE);

                quadPtr[3].texCoords =
                    sf::Vector2f(TEXTCOORD_WIDTH, TEXTCOORD_HEIGHT_PLUS_ONE_TILE);
            }
        }
    }


    const map::TileOffsets TileMap::GetTileOffsetsFromMapPos(const sf::Vector2f & MAP_POS_V) const
    {
        map::TileOffsets offsets;

        {
            auto const PLAYER_POS_IN_TILES_X{ static_cast<int>(MAP_POS_V.x) / mapLayout_.tile_size_v.x };
            auto const WINDOW_TILE_SIZE_X{ (static_cast<int>(WIN_SIZE_V_.x) / mapLayout_.tile_size_v.x) };
            auto const OFFSCREEN_TILE_SIZE_X{ WINDOW_TILE_SIZE_X + (EXTRA_OFFSCREEN_TILE_COUNT_ * 2) };

            offsets.begin_v.x = std::max(0, (PLAYER_POS_IN_TILES_X - (OFFSCREEN_TILE_SIZE_X / 2)));
            offsets.end_v.x = offsets.begin_v.x + OFFSCREEN_TILE_SIZE_X;

            if (offsets.end_v.x > mapLayout_.tile_count_v.x)
            {
                offsets.end_v.x = mapLayout_.tile_count_v.x;
                offsets.begin_v.x = offsets.end_v.x - OFFSCREEN_TILE_SIZE_X;
            }
        }

        {
            auto const PLAYER_POS_IN_TILES_Y{ static_cast<int>(MAP_POS_V.y) / mapLayout_.tile_size_v.y };
            auto const WINDOW_TILE_SIZE_Y{ (static_cast<int>(WIN_SIZE_V_.y) / mapLayout_.tile_size_v.y) };
            auto const OFFSCREEN_TILE_SIZE_Y{ WINDOW_TILE_SIZE_Y + (EXTRA_OFFSCREEN_TILE_COUNT_ * 2) };

            offsets.begin_v.y = std::max(0, (PLAYER_POS_IN_TILES_Y - (OFFSCREEN_TILE_SIZE_Y / 2)));
            offsets.end_v.y = offsets.begin_v.y + OFFSCREEN_TILE_SIZE_Y;

            if (offsets.end_v.y > mapLayout_.tile_count_v.y)
            {
                offsets.end_v.y = mapLayout_.tile_count_v.y;
                offsets.begin_v.y = offsets.end_v.y - OFFSCREEN_TILE_SIZE_Y;
            }
        }

        return offsets;
    }


    const sf::Vector2f TileMap::GetPlayerPosScreen() const
    {
        return WIN_CENTER_V_ + playerPosOffsetV_;
    }


    bool TileMap::DoesAdjPlayerPosCollide(const Direction::Enum DIR, const float ADJ) const
    {
        auto posToTest{ GetPlayerPos() };

        switch (DIR)
        {
            case Direction::Left:   { posToTest.x -= ADJ; break; }
            case Direction::Right:  { posToTest.x += ADJ; break; }
            case Direction::Up:     { posToTest.y -= ADJ; break; }
            case Direction::Down:
            case Direction::Count:
            default:                { posToTest.y += ADJ; break; }
        }

        return mapLayout_.collision_qtree.IsPointWithinCollisionRect(posToTest);
    }


    const map::TilesPanel & TileMap::GetTilesPanelFromId(const int ID) const
    {
        for (auto const & TILES_PANEL : mapLayout_.tiles_panel_vec)
        {
            if (TILES_PANEL.OwnsId(ID))
            {
                return TILES_PANEL;
            }
        }

        std::ostringstream ss;
        ss << "sfml_util::TileMap::GetTilesPanelFromId(id=" << ID
            << ") failed to find the owning TilesPanel.";

        throw std::runtime_error(ss.str());
    }


    void TileMap::IncrementTileOffsetsInDirection(const Direction::Enum DIR)
    {
        auto wereChangesMade{ false };

        auto const TILE_SIZE_X{ static_cast<float>(mapLayout_.tile_size_v.x) };
        auto const TILE_SIZE_Y{ static_cast<float>(mapLayout_.tile_size_v.y) };

        switch (DIR)
        {
            case Direction::Up:
            {
                if (tileOffsets_.begin_v.y > 0)
                {
                    --tileOffsets_.begin_v.y;
                    --tileOffsets_.end_v.y;
                    offScreenRect_.top += TILE_SIZE_Y;
                    wereChangesMade = true;
                }
                break;
            }

            case Direction::Down:
            {
                if (tileOffsets_.end_v.y < mapLayout_.tile_count_v.y)
                {
                    ++tileOffsets_.begin_v.y;
                    ++tileOffsets_.end_v.y;
                    offScreenRect_.top = std::max(0.0f, offScreenRect_.top - TILE_SIZE_Y);
                    wereChangesMade = true;
                }
                break;
            }

            case Direction::Left:
            {
                if (tileOffsets_.begin_v.x > 0)
                {
                    --tileOffsets_.begin_v.x;
                    --tileOffsets_.end_v.x;
                    offScreenRect_.left += TILE_SIZE_Y;
                    wereChangesMade = true;
                }
                break;
            }

            case Direction::Right:
            {
                if (tileOffsets_.end_v.x < mapLayout_.tile_count_v.x)
                {
                    ++tileOffsets_.begin_v.x;
                    ++tileOffsets_.end_v.x;
                    offScreenRect_.left = std::max(0.0f, offScreenRect_.left - TILE_SIZE_X);
                    wereChangesMade = true;
                }
                break;
            }

            case Direction::Count:
            default:
            {
                break;
            }
        }

        if (wereChangesMade)
        {
            ReDraw();
            PositionMapSpriteTextureRect();
        }
    }


    const sf::Vector2f TileMap::CalcOffScreenMapSize() const
    {
        auto const WIDTH{
            (tileOffsets_.end_v.x - tileOffsets_.begin_v.x) * mapLayout_.tile_size_v.x };

        auto const HEIGHT{
            (tileOffsets_.end_v.y - tileOffsets_.begin_v.y) * mapLayout_.tile_size_v.y };

        return sf::Vector2f(static_cast<float>(WIDTH), static_cast<float>(HEIGHT));
    }

}
}
