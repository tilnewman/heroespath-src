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
// map-display.cpp
//
#include "map-display.hpp"

#include "map/parser.hpp"
#include "sfml-util/sfml-util.hpp"
#include "misc/assertlogandthrow.hpp"
#include "npc/view-factory.hpp"
#include "npc/lpc-view.hpp"

#include <boost/algorithm/algorithm.hpp>
#include <boost/algorithm/string.hpp>

#include <sstream>
#include <exception>


namespace heroespath
{
namespace map
{

    const int MapDisplay::EXTRA_OFFSCREEN_TILE_COUNT_{ 2 };
    const std::size_t MapDisplay::VERTS_PER_QUAD_{ 4 };


    MapDisplay::MapDisplay(const sf::Vector2f & WIN_POS_V, const sf::Vector2f & WIN_SIZE_V)
        :
        BORDER_PAD_(sfml_util::MapByRes(50.0f, 500.0f)),
        WIN_POS_V_(WIN_POS_V),
        WIN_SIZE_V_(WIN_SIZE_V),
        WIN_CENTER_V_(
            WIN_POS_V_.x + (WIN_SIZE_V_.x * 0.5f),
            WIN_POS_V_.y + (WIN_SIZE_V_.y * 0.5f)),
        layout_(),
        tileOffsets_(),
        playerPosV_(0.0f, 0.0f),
        playerPosOffsetV_(0.0f, 0.0f),
        offScreenRect_(0.0f, 0.0f, 0.0f, 0.0f),
        offScreenSpriteAbove_(),
        offScreenSpriteBelow_(),
        offScreenTextureAbove_(),
        offScreenTextureBelow_(),
        offScreenMapSize_(0.0f, 0.0f),
        player_(npc::ViewFactory::MakeLPCView("media-images-npc-lpc-puck"))
    {}


    void MapDisplay::Load(const sf::Vector2f & STARTING_POS_V)
    {
        tileOffsets_ = map::TileOffsets();
        playerPosOffsetV_ = sf::Vector2f(0.0f, 0.0f);
        playerPosV_ = STARTING_POS_V;
        tileOffsets_ = TileOffsetsFromMapPos(STARTING_POS_V);
        offScreenMapSize_ = CalcOffScreenMapSize();
        SetupOffScreenTexture();
        ReDraw();
    }


    bool MapDisplay::Move(const sfml_util::Direction::Enum DIR, const float ADJUSTMENT)
    {
        switch (DIR)
        {
            case sfml_util::Direction::Up:    { return MoveUp(ADJUSTMENT); }
            case sfml_util::Direction::Down:  { return MoveDown(ADJUSTMENT); }
            case sfml_util::Direction::Left:  { return MoveLeft(ADJUSTMENT); }
            case sfml_util::Direction::Right: { return MoveRight(ADJUSTMENT); }
            case sfml_util::Direction::Count:
            default: { return false; }
        }
    }


    void MapDisplay::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        DrawNormal(target, states);
    }


    void MapDisplay::Update(const float TIME_ELAPSED)
    {
        player_.Update(TIME_ELAPSED);
    }


    bool MapDisplay::MoveUp(const float ADJUSTMENT)
    {
        if (PlayerPosScreen().y > (WIN_POS_V_.y + BORDER_PAD_))
        {
            //move within the view before moving the view
            playerPosOffsetV_.y -= ADJUSTMENT;
        }
        else
        {
            if (offScreenRect_.top > 0.0f)
            {
                auto const OFFSCREEN_TOP_BEFORE{ offScreenRect_.top };
                offScreenRect_.top = std::max(0.0f, offScreenRect_.top - ADJUSTMENT);

                auto const ACTUAL_ADJUSTMENT{ OFFSCREEN_TOP_BEFORE - offScreenRect_.top };

                PositionMapSpriteTextureRect();
                playerPosV_.y = std::max(0.0f, playerPosV_.y - ACTUAL_ADJUSTMENT);

                if (misc::IsRealZero(offScreenRect_.top))
                {
                    IncrementTileOffsetsInDirection(sfml_util::Direction::Up);
                }
            }
            else
            {
                return false;
            }
        }

        return true;
    }


    bool MapDisplay::MoveDown(const float ADJUSTMENT)
    {
        if (PlayerPosScreen().y < ((WIN_POS_V_.y + WIN_SIZE_V_.y) - BORDER_PAD_))
        {
            //move within the view before moving the view
            playerPosOffsetV_.y += ADJUSTMENT;
        }
        else
        {
            auto const VERT_POS{ offScreenRect_.top +
                static_cast<float>(tileOffsets_.begin_v.y * layout_.tile_size_v.y) };

            auto const VERT_LIMIT{
                static_cast<float>(layout_.tile_count_v.y * layout_.tile_size_v.y) -
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
                    IncrementTileOffsetsInDirection(sfml_util::Direction::Down);
                }
            }
        }

        return true;
    }


    bool MapDisplay::MoveLeft(const float ADJUSTMENT)
    {
        if (PlayerPosScreen().x > (WIN_POS_V_.x + BORDER_PAD_))
        {
            //move within the view before moving the view
            playerPosOffsetV_.x -= ADJUSTMENT;
        }
        else
        {
            if (offScreenRect_.left > 0.0f)
            {
                auto const OFFSCREEN_LEFT_BEFORE{ offScreenRect_.left };
                offScreenRect_.left = std::max(0.0f, offScreenRect_.left - ADJUSTMENT);

                auto const ACTUAL_ADJUSTMENT{ OFFSCREEN_LEFT_BEFORE - offScreenRect_.left };
                
                PositionMapSpriteTextureRect();
                playerPosV_.x = std::max(0.0f, playerPosV_.x - ACTUAL_ADJUSTMENT);

                if (misc::IsRealZero(offScreenRect_.left))
                {
                    IncrementTileOffsetsInDirection(sfml_util::Direction::Left);
                }


            }
            else
            {
                return false;
            }
        }

        return true;
    }


    bool MapDisplay::MoveRight(const float ADJUSTMENT)
    {
        if (PlayerPosScreen().x < ((WIN_POS_V_.x + WIN_SIZE_V_.x) - BORDER_PAD_))
        {
            //move within the view before moving the view
            playerPosOffsetV_.x += ADJUSTMENT;
        }
        else
        {
            auto const HORIZ_POS{ (offScreenRect_.left + static_cast<float>(
                tileOffsets_.begin_v.x * layout_.tile_size_v.x)) };

            auto const HORIZ_LIMIT{ static_cast<float>(
                (layout_.tile_count_v.x * layout_.tile_size_v.x) -
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
                    IncrementTileOffsetsInDirection(sfml_util::Direction::Right);
                }
            }
        }

        return true;
    }


    void MapDisplay::DrawNormal(sf::RenderTarget & target, sf::RenderStates states) const
    {
        target.draw(offScreenSpriteBelow_, states);
        DrawPlayerImage(target, PlayerPosScreen());
        target.draw(offScreenSpriteAbove_, states);
    }


    void MapDisplay::DrawDebug(sf::RenderTarget & target, sf::RenderStates states) const
    {
        sf::Sprite offScreenSpriteBelow(offScreenTextureBelow_.getTexture());
        offScreenSpriteBelow.setPosition(WIN_POS_V_);
        target.draw(offScreenSpriteBelow);

        sf::Sprite offScreenSpriteAbove(offScreenTextureAbove_.getTexture());
        offScreenSpriteAbove.setPosition(WIN_POS_V_);
        target.draw(offScreenSpriteAbove);

        sf::FloatRect offScreenInnerWindowRect(
            WIN_POS_V_.x + offScreenRect_.left,
            WIN_POS_V_.y + offScreenRect_.top,
            offScreenRect_.width,
            offScreenRect_.height);

        sfml_util::DrawRectangle(target, states, offScreenInnerWindowRect);
    }


    void MapDisplay::DrawPlayerImage(sf::RenderTarget & target, const sf::Vector2f & POS_V) const
    {
        sf::Sprite sprite;
        player_.GetView().Sprite(sprite);
        sprite.setPosition(POS_V);
        target.draw(sprite);
    }


    void MapDisplay::ReDraw()
    {
        ResetMapSubsections();
        DrawMapSubsectionOffscreen();
    }


    void MapDisplay::ResetMapSubsections()
    {
        for (auto & nextMapLayer : layout_.layer_vec)
        {
            nextMapLayer.ResetForReDraw();
            SetupMapLayerSubsection(nextMapLayer, tileOffsets_);
        }
    }


    void MapDisplay::DrawMapSubsectionOffscreen()
    {
        offScreenTextureBelow_.clear(sf::Color::Transparent);
        offScreenTextureAbove_.clear(sf::Color::Transparent);

        sf::RenderStates renderStates{ sf::RenderStates::Default };
        sf::VertexArray quads(sf::Quads, VERTS_PER_QUAD_);

        //draw all vertex arrays to the off-screen texture
        for (auto const & LAYER : layout_.layer_vec)
        {
            std::size_t tilesPanelIndex{ 0 };
            auto const NUM_VERTS{ LAYER.vert_array.getVertexCount() };
            for (std::size_t vertIndex(0); vertIndex < NUM_VERTS; )
            {
                //skip drawing empty tiles
                if (LAYER.tiles_panel_vec[tilesPanelIndex].is_empty)
                {
                    ++tilesPanelIndex;
                    vertIndex += VERTS_PER_QUAD_;
                }
                else
                {
                    quads[0].position = LAYER.vert_array[vertIndex].position;
                    quads[0].texCoords = LAYER.vert_array[vertIndex++].texCoords;

                    quads[1].position = LAYER.vert_array[vertIndex].position;
                    quads[1].texCoords = LAYER.vert_array[vertIndex++].texCoords;

                    quads[2].position = LAYER.vert_array[vertIndex].position;
                    quads[2].texCoords = LAYER.vert_array[vertIndex++].texCoords;

                    quads[3].position = LAYER.vert_array[vertIndex].position;
                    quads[3].texCoords = LAYER.vert_array[vertIndex++].texCoords;

                    renderStates.texture = & layout_.texture_vec[
                        LAYER.tiles_panel_vec[tilesPanelIndex].texture_index];

                    ++tilesPanelIndex;

                    if (LAYER.type == LayerType::Ground)
                    {
                        offScreenTextureBelow_.draw(quads, renderStates);
                    }
                    else
                    {
                        offScreenTextureAbove_.draw(quads, renderStates);
                    }
                }
            }
        }

        offScreenTextureBelow_.display();
        offScreenTextureAbove_.display();
    }


    void MapDisplay::PositionMapSpriteTextureRect()
    {
        auto const FLOAT_RECT{ sfml_util::ConvertRect<float, int>(offScreenRect_) };
        offScreenSpriteBelow_.setTextureRect(FLOAT_RECT);
        offScreenSpriteAbove_.setTextureRect(FLOAT_RECT);
    }


    void MapDisplay::SetupOffScreenTexture()
    {
        auto const EXTRA_WIDTH{ layout_.tile_size_v.x  * (EXTRA_OFFSCREEN_TILE_COUNT_ * 2) };
        auto const WIDTH{ static_cast<unsigned>(WIN_SIZE_V_.x) + static_cast<unsigned>(EXTRA_WIDTH) };

        auto const EXTRA_HEIGHT{ layout_.tile_size_v.y * (EXTRA_OFFSCREEN_TILE_COUNT_ * 2) };
        auto const HEIGHT{ static_cast<unsigned>(WIN_SIZE_V_.y) + static_cast<unsigned>(EXTRA_HEIGHT) };

        M_ASSERT_OR_LOGANDTHROW_SS(offScreenTextureBelow_.create(WIDTH, HEIGHT),
            "sfml_util::MapDisplay::Load(), failed to sf::RenderTexture::create("
                << WIDTH << "x" << HEIGHT << ") for 'below' texture.");

        M_ASSERT_OR_LOGANDTHROW_SS(offScreenTextureAbove_.create(WIDTH, HEIGHT),
            "sfml_util::MapDisplay::Load(), failed to sf::RenderTexture::create("
            << WIDTH << "x" << HEIGHT << ") for 'above' texture.");

        //set the initial position of what is drawn on-screen from the off-screen texture
        offScreenRect_.width = WIN_SIZE_V_.x;
        offScreenRect_.height = WIN_SIZE_V_.y;

        offScreenRect_.left =
            static_cast<float>(EXTRA_OFFSCREEN_TILE_COUNT_ * layout_.tile_size_v.x);

        offScreenRect_.top =
            static_cast<float>(EXTRA_OFFSCREEN_TILE_COUNT_ * layout_.tile_size_v.y);

        offScreenSpriteBelow_.setTexture(offScreenTextureBelow_.getTexture());
        offScreenSpriteAbove_.setTexture(offScreenTextureAbove_.getTexture());
        
        offScreenSpriteBelow_.setPosition(WIN_POS_V_);
        offScreenSpriteAbove_.setPosition(WIN_POS_V_);

        PositionMapSpriteTextureRect();
    }


    void MapDisplay::SetupMapLayerSubsection(
        map::Layer & mapLayer,
        const map::TileOffsets & TILE_OFFSETS)
    {
        //resize the vertex array to fit the level size
        auto const TILE_WIDTH{  TILE_OFFSETS.end_v.x - TILE_OFFSETS.begin_v.x };
        auto const TILE_HEIGHT{ TILE_OFFSETS.end_v.y - TILE_OFFSETS.begin_v.y };
        mapLayer.vert_array.setPrimitiveType(sf::Quads);

        mapLayer.vert_array.resize(
            static_cast<std::size_t>(TILE_WIDTH * TILE_HEIGHT) * VERTS_PER_QUAD_ );

        mapLayer.tiles_panel_vec.reserve( static_cast<std::size_t>(TILE_WIDTH * TILE_HEIGHT) );

        //populate the vertex array with one quad per tile
        std::size_t vertIndex{ 0 };
        for (int x(TILE_OFFSETS.begin_v.x); x < TILE_OFFSETS.end_v.x; ++x)
        {
            for (int y(TILE_OFFSETS.begin_v.y); y < TILE_OFFSETS.end_v.y; ++y)
            {
                //get the current tile number
                auto const TILE_INDEX{ x + (y * layout_.tile_count_v.x) };

                auto const TILE_NUM_ORIG{
                    mapLayer.mapid_vec[static_cast<std::size_t>(TILE_INDEX)] };

                //get the texture/image this tile can be found in
                const map::TilesPanel & TILES_PANEL{ TilesPanelFromId(TILE_NUM_ORIG) };
                
                mapLayer.tiles_panel_vec.push_back( 
                    map::TilesPanelForLayers(
                        (TILES_PANEL.name == layout_.EmptyTilesPanelName()),
                        TILES_PANEL.texture_index) );

                //get the actual tile number (id)
                auto const TILE_NUM_FINAL{ (TILE_NUM_ORIG - TILES_PANEL.first_id) };

                //get a pointer to the current tile's quad
                auto const VERT_ARRAY_INDEX{ vertIndex++ * VERTS_PER_QUAD_ };
                sf::Vertex * quadPtr{ & mapLayer.vert_array[VERT_ARRAY_INDEX] };

                //define its four corners on screen
                auto const POS_WIDTH{
                    static_cast<float>((x - TILE_OFFSETS.begin_v.x) * layout_.tile_size_v.x) };

                auto const POS_WIDTH_PLUS_ONE_TILE{
                    POS_WIDTH + static_cast<float>(layout_.tile_size_v.x) };

                auto const POS_HEIGHT{
                    static_cast<float>((y - TILE_OFFSETS.begin_v.y) * layout_.tile_size_v.y) };

                auto const POS_HEIGHT_PLUS_ONE_TILE{
                    POS_HEIGHT + static_cast<float>(layout_.tile_size_v.y) };

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
                    layout_.texture_vec[TILES_PANEL.texture_index].getSize().x) / layout_.tile_size_v.x };

                auto const TILE_COUNT_U{
                    static_cast<float>(TILE_NUM_FINAL % TEXTURE_TILE_COUNT_HORIZ) };

                auto const TILE_COUNT_V{
                    static_cast<float>(TILE_NUM_FINAL / TEXTURE_TILE_COUNT_HORIZ) };

                auto const TEXTCOORD_WIDTH{
                    TILE_COUNT_U * static_cast<float>(layout_.tile_size_v.x) };

                auto const TEXTCOORD_WIDTH_PLUS_ONE_TILE{
                    TEXTCOORD_WIDTH + static_cast<float>(layout_.tile_size_v.x) };

                auto const TEXTCOORD_HEIGHT{
                    TILE_COUNT_V * static_cast<float>(layout_.tile_size_v.y) };

                auto const TEXTCOORD_HEIGHT_PLUS_ONE_TILE{
                    TEXTCOORD_HEIGHT + static_cast<float>(layout_.tile_size_v.y) };

                //define its four texture coordinates
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


    const map::TileOffsets MapDisplay::TileOffsetsFromMapPos(const sf::Vector2f & MAP_POS_V) const
    {
        map::TileOffsets offsets;

        {
            auto const PLAYER_POS_IN_TILES_X{ static_cast<int>(MAP_POS_V.x) / layout_.tile_size_v.x };
            auto const WINDOW_TILE_SIZE_X{ (static_cast<int>(WIN_SIZE_V_.x) / layout_.tile_size_v.x) };
            auto const OFFSCREEN_TILE_SIZE_X{ WINDOW_TILE_SIZE_X + (EXTRA_OFFSCREEN_TILE_COUNT_ * 2) };

            offsets.begin_v.x = std::max(0, (PLAYER_POS_IN_TILES_X - (OFFSCREEN_TILE_SIZE_X / 2)));
            offsets.end_v.x = offsets.begin_v.x + OFFSCREEN_TILE_SIZE_X;

            if (offsets.end_v.x > layout_.tile_count_v.x)
            {
                offsets.end_v.x = layout_.tile_count_v.x;
                offsets.begin_v.x = offsets.end_v.x - OFFSCREEN_TILE_SIZE_X;
            }
        }

        {
            auto const PLAYER_POS_IN_TILES_Y{ static_cast<int>(MAP_POS_V.y) / layout_.tile_size_v.y };
            auto const WINDOW_TILE_SIZE_Y{ (static_cast<int>(WIN_SIZE_V_.y) / layout_.tile_size_v.y) };
            auto const OFFSCREEN_TILE_SIZE_Y{ WINDOW_TILE_SIZE_Y + (EXTRA_OFFSCREEN_TILE_COUNT_ * 2) };

            offsets.begin_v.y = std::max(0, (PLAYER_POS_IN_TILES_Y - (OFFSCREEN_TILE_SIZE_Y / 2)));
            offsets.end_v.y = offsets.begin_v.y + OFFSCREEN_TILE_SIZE_Y;

            if (offsets.end_v.y > layout_.tile_count_v.y)
            {
                offsets.end_v.y = layout_.tile_count_v.y;
                offsets.begin_v.y = offsets.end_v.y - OFFSCREEN_TILE_SIZE_Y;
            }
        }

        return offsets;
    }


    const sf::Vector2f MapDisplay::PlayerPosScreen() const
    {
        return ScreenPosFromMapPos(PlayerPosMap());
    }


    const sf::Vector2f MapDisplay::ScreenPosFromMapPos(const sf::Vector2f & MAP_POS_V) const
    {
        auto const SCREEN_LEFT{ (WIN_POS_V_.x - offScreenRect_.left) };
        auto const SCREEN_TOP{ (WIN_POS_V_.y - offScreenRect_.top) };
        const sf::Vector2f SCREEN_POS_V(SCREEN_LEFT, SCREEN_TOP);

        auto const MAP_OFFSET_LEFT{
            MAP_POS_V.x - static_cast<float>(layout_.tile_size_v.x * tileOffsets_.begin_v.x) };

        auto const MAP_OFFSET_TOP {
            MAP_POS_V.y -  static_cast<float>(layout_.tile_size_v.y * tileOffsets_.begin_v.y) };

        const sf::Vector2f MAP_OFFSET_V(MAP_OFFSET_LEFT, MAP_OFFSET_TOP);

        return SCREEN_POS_V + MAP_OFFSET_V;
    }


    const map::TilesPanel & MapDisplay::TilesPanelFromId(const int ID) const
    {
        for (auto const & TILES_PANEL : layout_.tiles_panel_vec)
        {
            if (TILES_PANEL.OwnsId(ID))
            {
                return TILES_PANEL;
            }
        }

        std::ostringstream ss;
        ss << "sfml_util::MapDisplay::TilesPanelFromId(id=" << ID
            << ") failed to find the owning TilesPanel.";

        throw std::runtime_error(ss.str());
    }


    void MapDisplay::IncrementTileOffsetsInDirection(const sfml_util::Direction::Enum DIR)
    {
        auto wereChangesMade{ false };

        auto const TILE_SIZE_X{ static_cast<float>(layout_.tile_size_v.x) };
        auto const TILE_SIZE_Y{ static_cast<float>(layout_.tile_size_v.y) };

        switch (DIR)
        {
            case sfml_util::Direction::Up:
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

            case sfml_util::Direction::Down:
            {
                if (tileOffsets_.end_v.y < layout_.tile_count_v.y)
                {
                    ++tileOffsets_.begin_v.y;
                    ++tileOffsets_.end_v.y;
                    offScreenRect_.top = std::max(0.0f, offScreenRect_.top - TILE_SIZE_Y);
                    wereChangesMade = true;
                }
                break;
            }

            case sfml_util::Direction::Left:
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

            case sfml_util::Direction::Right:
            {
                if (tileOffsets_.end_v.x < layout_.tile_count_v.x)
                {
                    ++tileOffsets_.begin_v.x;
                    ++tileOffsets_.end_v.x;
                    offScreenRect_.left = std::max(0.0f, offScreenRect_.left - TILE_SIZE_X);
                    wereChangesMade = true;
                }
                break;
            }

            case sfml_util::Direction::Count:
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


    const sf::Vector2f MapDisplay::CalcOffScreenMapSize() const
    {
        auto const WIDTH{
            (tileOffsets_.end_v.x - tileOffsets_.begin_v.x) * layout_.tile_size_v.x };

        auto const HEIGHT{
            (tileOffsets_.end_v.y - tileOffsets_.begin_v.y) * layout_.tile_size_v.y };

        return sf::Vector2f(static_cast<float>(WIDTH), static_cast<float>(HEIGHT));
    }

}
}
