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
#include "sfml-util/loaders.hpp"
#include "log/log-macros.hpp"
#include "misc/assertlogandthrow.hpp"

#include <sstream>
#include <exception>

#include <boost/algorithm/algorithm.hpp>
#include <boost/algorithm/string.hpp>


namespace heroespath
{
namespace sfml_util
{

    const float       TileMap::BORDER_PAD_                { 75.0f };
    const Count_t     TileMap::EXTRA_OFFSCREEN_TILE_COUNT_{ 2_count };
    const sf::Color   TileMap::DEFAULT_TRANSPARENT_MASK_  { sf::Color(75, 99, 127) };
    const sf::Color   TileMap::SHADOW_MASK1_              { sf::Color(0, 0, 0) };
    const sf::Color   TileMap::SHADOW_MASK2_              { sf::Color(151, 0, 147) };
    const sf::Color   TileMap::SHADOW_MASK2B_             { sf::Color(127, 0, 127) };
    const sf::Color   TileMap::SHADOW_MASK3_              { sf::Color(255, 0, 255) };
    const sf::Color   TileMap::SHADOW_COLOR1_             { sf::Color(0, 0, 0, 100) };
    const sf::Color   TileMap::SHADOW_COLOR2_             { sf::Color(0, 0, 0, 65) };
    const sf::Color   TileMap::SHADOW_COLOR3_             { sf::Color(0, 0, 0, 35) };
    const Index_t     TileMap::TRANSPARENT_TEXTURE_INDEX_ { 0_index };


    TileMap::TileMap(
        const std::string & MAP_PATH_STR,
        const sf::Vector2f & WIN_POS_V,
        const sf::Vector2u & WIN_SIZE_V,
        const sf::Vector2f & PLAYER_POS_V,
        const sf::Color & TRANSPARENT_MASK)
    :
        mapParser_       (),
        mapLayout_       (),
        prevTileOffsets_ (),
        WIN_POS_V_       (WIN_POS_V),
        WIN_SIZE_V_      (WIN_SIZE_V),
        playerPosMapV_   (PLAYER_POS_V),
        playerPosOffsetScreenV_(0.0f, 0.0f),
        renderStates_    (sf::RenderStates::Default),
        offScreenRect_   (),
        mapSprite_       (),
        offScreenTexture_(),
        TRANSPARENT_MASK_(TRANSPARENT_MASK),
        collisionTree_   ()
    {
        mapParser_.Parse(MAP_PATH_STR, mapLayout_);
        
        //create off-screen texture
        //always ensure these are valid power of 2 sizes
        auto const WIDTH{
            WIN_SIZE_V_.x + (mapLayout_.tile_size_horiz  * (EXTRA_OFFSCREEN_TILE_COUNT_.As<unsigned>() + 2)) };

        auto const HEIGHT{
            WIN_SIZE_V_.y + (mapLayout_.tile_size_vert * (EXTRA_OFFSCREEN_TILE_COUNT_.As<unsigned>() + 2)) };

        M_ASSERT_OR_LOGANDTHROW_SS(offScreenTexture_.create(WIDTH, HEIGHT),
            "sfml_util::TileMap::TileMap(\"" << MAP_PATH_STR
            << "\"), failed to sf::RenderTexture::create("<< WIDTH << "x" << HEIGHT << ").");

        //set the initial position of what is drawn on-screen from the off-screen texture
        offScreenRect_.width = static_cast<float>(WIN_SIZE_V_.x);
        offScreenRect_.height = static_cast<float>(WIN_SIZE_V_.y);

        offScreenRect_.left =
            static_cast<float>(EXTRA_OFFSCREEN_TILE_COUNT_.As<unsigned>() * mapLayout_.tile_size_horiz);

        offScreenRect_.top =
            static_cast<float>(EXTRA_OFFSCREEN_TILE_COUNT_.As<unsigned>() * mapLayout_.tile_size_vert);

        SetupMapSprite();

        ApplyColorMasksToHandleTransparency();

        collisionTree_.Setup(
            static_cast<float>(mapLayout_.tile_size_horiz * mapLayout_.tile_count_horiz),
            static_cast<float>(mapLayout_.tile_size_vert * mapLayout_.tile_count_vert),
            mapLayout_.collision_rect_vec);
    }


    void TileMap::ApplyColorMasksToHandleTransparency()
    {
        for (auto & nextTilesPanel: mapLayout_.tiles_panel_vec)
        {
            sf::Image srcImage(mapLayout_.texture_vec[nextTilesPanel.texture_index.Get()].copyToImage());
            sf::Image destImage(srcImage);

            namespace ba = boost::algorithm;
            auto const IS_SHADOW_LAYER{ ba::contains(
                ba::to_lower_copy(nextTilesPanel.name),
                ba::to_lower_copy(mapParser_.XML_ATTRIB_NAME_SHADOWS_)) };

            const sf::Uint8 * const PIXEL_PTR{ srcImage.getPixelsPtr() };

            auto const PIXEL_COUNT{ static_cast<std::size_t>(
                srcImage.getSize().x * srcImage.getSize().y * static_cast<unsigned int>(4)) };

            for (std::size_t i(0); i < PIXEL_COUNT; i+=4)
            {
                const sf::Uint8 RED  { * (PIXEL_PTR + i + 0) };
                const sf::Uint8 GREEN{ * (PIXEL_PTR + i + 1) };
                const sf::Uint8 BLUE { * (PIXEL_PTR + i + 2) };

                const unsigned NUM_COLOR_COMPONENTS{ 4 };//red, green, blue, alpha

                auto const DEST_X{
                    (static_cast<unsigned>(i) / NUM_COLOR_COMPONENTS) % destImage.getSize().x};

                auto const DEST_Y{
                    (static_cast<unsigned>(i) / NUM_COLOR_COMPONENTS) / destImage.getSize().x};

                //check for faded blue background color that should be made fully transparent
                if ((RED   == TRANSPARENT_MASK_.r) &&
                    (GREEN == TRANSPARENT_MASK_.g) &&
                    (BLUE  == TRANSPARENT_MASK_.b))
                {
                    destImage.setPixel(DEST_X, DEST_Y, sf::Color::Transparent);
                }
                else
                {
                    //all remaining shadow colors has a green value of zero
                    if (IS_SHADOW_LAYER && (GREEN == 0))
                    {
                        if ((RED  == SHADOW_MASK1_.r) && (BLUE == SHADOW_MASK1_.b))
                        {
                            destImage.setPixel(DEST_X, DEST_Y, SHADOW_COLOR1_);
                        }
                        else if (((RED  == SHADOW_MASK2_.r) &&  (BLUE == SHADOW_MASK2_.b)) ||
                                 ((RED  == SHADOW_MASK2B_.r) && (BLUE == SHADOW_MASK2B_.b)))
                        {
                            destImage.setPixel(DEST_X, DEST_Y, SHADOW_COLOR2_);
                        }
                        else if ((RED  == SHADOW_MASK3_.r) && (BLUE == SHADOW_MASK3_.b))
                        {
                            destImage.setPixel(DEST_X, DEST_Y, SHADOW_COLOR3_);
                        }
                    }
                }
            }

            mapLayout_.texture_vec[nextTilesPanel.texture_index.Get()].update(destImage);
        }
    }


    bool TileMap::MoveUp(const float ADJUSTMENT)
    {
        //check for collision
        auto posTest{ GetPlayerPosCollision() };
        posTest.y -= ADJUSTMENT;
        if (IsPointWithinCollision(posTest))
        {
            return false;
        }

        auto const CURR_SCREEN_POS{ GetPlayerPosScreen() };

        //move within the view before moving the view
        if (CURR_SCREEN_POS.y > (WIN_POS_V_.y + BORDER_PAD_))
        {
            playerPosOffsetScreenV_.y -= ADJUSTMENT;
            return true;
        }
        else
        {
            if (offScreenRect_.top > 0.0f)
            {
                offScreenRect_.top -= ADJUSTMENT;
                if (offScreenRect_.top < 0.0f)
                {
                    offScreenRect_.top = 0.0f;
                }

                SetupMapSprite();

                playerPosMapV_.y -= ADJUSTMENT;
                if (playerPosMapV_.y < 0.0f)
                {
                    playerPosMapV_.y = 0.0f;
                }

                return true;
            }
        }

        return false;
    }


    bool TileMap::MoveDown(const float ADJUSTMENT)
    {
        //check for collision
        auto posTest{ GetPlayerPosCollision() };
        posTest.y += ADJUSTMENT;
        if (IsPointWithinCollision(posTest))
        {
            return false;
        }

        auto const CURR_SCREEN_POS{ GetPlayerPosScreen() };

        //move within the view before moving the view
        if (CURR_SCREEN_POS.y < ((WIN_POS_V_.y + static_cast<float>(WIN_SIZE_V_.y)) - BORDER_PAD_))
        {
            playerPosOffsetScreenV_.y += ADJUSTMENT;
            return true;
        }
        else
        {
            auto const VERT_POS{
                offScreenRect_.top +
                    static_cast<float>(prevTileOffsets_.begin_y.As<unsigned>() * mapLayout_.tile_size_vert) };

            auto const VERT_LIMIT{
                static_cast<float>(mapLayout_.tile_count_vert * mapLayout_.tile_size_vert) -
                    static_cast<float>(WIN_SIZE_V_.y) };

            if (VERT_POS < VERT_LIMIT)
            {
                offScreenRect_.top += ADJUSTMENT;
                SetupMapSprite();
                playerPosMapV_.y += ADJUSTMENT;
                return true;
            }
        }

        return false;
    }


    bool TileMap::MoveLeft(const float ADJUSTMENT)
    {
        //check for collision
        auto posTest{ GetPlayerPosCollision() };
        posTest.x -= ADJUSTMENT;
        if (IsPointWithinCollision(posTest))
        {
            return false;
        }

        auto const CURR_SCREEN_POS{ GetPlayerPosScreen() };

        //move within the view before moving the view
        if (CURR_SCREEN_POS.x > (WIN_POS_V_.x + BORDER_PAD_))
        {
            playerPosOffsetScreenV_.x -= ADJUSTMENT;
            return true;
        }
        else
        {
            if (offScreenRect_.left > 0.0f)
            {
                offScreenRect_.left -= ADJUSTMENT;
                if (offScreenRect_.left < 0.0f)
                {
                    offScreenRect_.left = 0.0f;
                }

                SetupMapSprite();

                playerPosMapV_.x -= ADJUSTMENT;
                if (playerPosMapV_.x < 0.0f)
                {
                    playerPosMapV_.x = 0.0f;
                }

                return true;
            }
        }

        return false;
    }


    bool TileMap::MoveRight(const float ADJUSTMENT)
    {
        //check for collision
        auto posTest{ GetPlayerPosCollision() };
        posTest.x += ADJUSTMENT;
        if (IsPointWithinCollision(posTest))
        {
            return false;
        }

        auto const CURR_SCREEN_POS{ GetPlayerPosScreen() };

        //move within the view before moving the view
        if (CURR_SCREEN_POS.x < ((WIN_POS_V_.x + static_cast<float>(WIN_SIZE_V_.x)) - BORDER_PAD_))
        {
            playerPosOffsetScreenV_.x += ADJUSTMENT;
            return true;
        }
        else
        {
            auto const HORIZ_POS{
                (offScreenRect_.left +
                    static_cast<float>(prevTileOffsets_.begin_x.As<unsigned>() * mapLayout_.tile_size_horiz)) };

            auto const HORIZ_LIMIT{
                static_cast<float>((mapLayout_.tile_count_horiz * mapLayout_.tile_size_horiz) - WIN_SIZE_V_.x) };

            if (HORIZ_POS < HORIZ_LIMIT)
            {
                offScreenRect_.left += ADJUSTMENT;
                SetupMapSprite();
                playerPosMapV_.x += ADJUSTMENT;
                return true;
            }
        }

        return false;
    }


    void TileMap::Draw(sf::RenderTarget & target, const sf::RenderStates &)
    {
        //check if player position requires changing the map tiles drawn
        auto const CURRENT_TILE_OFFSETS{ GetTileOffsetsOfPlayerPos() };
        if (prevTileOffsets_ != CURRENT_TILE_OFFSETS)
        {
            //clear away the existing tiles
            for(auto & nextMapLayer : mapLayout_.layer_vec)
            {
                nextMapLayer.ResetForReDraw();
            }

            //establish what new tiles to draw
            for (auto & nextMapLayer : mapLayout_.layer_vec)
            {
                EstablishMapSubsection(nextMapLayer, CURRENT_TILE_OFFSETS);
            }

            //adjust offScreenRect to keep the map from jumping every time the offsets change
            AdjustOffscreenRectsToPreventDrift(CURRENT_TILE_OFFSETS);

            prevTileOffsets_ = CURRENT_TILE_OFFSETS;
            SetupMapSprite();
        }

        //clear the map to black
        offScreenTexture_.clear(sf::Color::Black);

        auto hasObjectTileBeenDrawnYet{ false };

        //draw all vertex arrays to the off-screen texture
        sf::VertexArray quads(sf::Quads, 4);
        for (auto const & LAYER : mapLayout_.layer_vec)
        {
            std::size_t tilesPanelIndex{ 0 };
            auto const NUM_VERTS{ LAYER.vert_array.getVertexCount() };
            for (std::size_t vertIndex(0); vertIndex < NUM_VERTS; )
            {
                //draw player before objects that might be drawn on top of the player
                if (false == hasObjectTileBeenDrawnYet)
                {
                    namespace ba = boost::algorithm;

                    auto const IS_GROUND_LAYER{ ba::contains(
                            ba::to_lower_copy(LAYER.tiles_panel_vec[tilesPanelIndex].name),
                            mapParser_.XML_ATTRIB_NAME_GROUND_) };

                    if (IS_GROUND_LAYER == false)
                    {
                        offScreenTexture_.display();
                        target.draw(mapSprite_);
                        offScreenTexture_.clear(sf::Color::Transparent);
                        DrawPlayer(target);
                        hasObjectTileBeenDrawnYet = true;
                    }
                }

                //skip drawing empty tiles
                if (LAYER.tiles_panel_vec[tilesPanelIndex].name != mapLayout_.EmptyTilesPanelName())
                {
                    quads[0].position  = LAYER.vert_array[vertIndex].position;
                    quads[0].texCoords = LAYER.vert_array[vertIndex++].texCoords;

                    quads[1].position  = LAYER.vert_array[vertIndex].position;
                    quads[1].texCoords = LAYER.vert_array[vertIndex++].texCoords;

                    quads[2].position  = LAYER.vert_array[vertIndex].position;
                    quads[2].texCoords = LAYER.vert_array[vertIndex++].texCoords;

                    quads[3].position  = LAYER.vert_array[vertIndex].position;
                    quads[3].texCoords = LAYER.vert_array[vertIndex++].texCoords;

                    renderStates_.texture = & mapLayout_.texture_vec[
                        LAYER.tiles_panel_vec[tilesPanelIndex++].texture_index.As<unsigned>()];

                    offScreenTexture_.draw(quads, renderStates_);
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
        target.draw(mapSprite_);
        DrawPlayer(target);
    }


    void TileMap::DrawPlayer(sf::RenderTarget & target)
    {
        sf::CircleShape c(5);
        c.setFillColor(sf::Color(255, 0, 0));
        c.setPosition(GetPlayerPosScreen());
        target.draw(c);
    }


    void TileMap::SetupMapSprite()
    {
        mapSprite_.setTexture(offScreenTexture_.getTexture());
        mapSprite_.setPosition(WIN_POS_V_);
        mapSprite_.setTextureRect(sfml_util::ConvertRect<float, int>(offScreenRect_));
    }


    void TileMap::EstablishMapSubsection(
        map::Layer & mapLayer,
        const map::TileOffsets & TILE_OFFSETS)
    {
        //resize the vertex array to fit the level size
        auto const TILE_WIDTH{  TILE_OFFSETS.end_x - TILE_OFFSETS.begin_x };
        auto const TILE_HEIGHT{ TILE_OFFSETS.end_y - TILE_OFFSETS.begin_y };
        mapLayer.vert_array.setPrimitiveType(sf::Quads);
        const std::size_t VERTEXES_PER_QUAD{ 4 };

        mapLayer.vert_array.resize(
            (TILE_WIDTH.As<std::size_t>() * TILE_HEIGHT.As<std::size_t>()) * VERTEXES_PER_QUAD);

        mapLayer.tiles_panel_vec.reserve(TILE_WIDTH.As<std::size_t>() * TILE_HEIGHT.As<std::size_t>());

        //populate the vertex array with one quad per tile
        unsigned vertIndex(0);
        for (Count_t::type x(TILE_OFFSETS.begin_x.Get()); x < TILE_OFFSETS.end_x.Get(); ++x)
        {
            for (Count_t::type y(TILE_OFFSETS.begin_y.Get()); y < TILE_OFFSETS.end_y.Get(); ++y)
            {
                //get the current tile number
                auto const TILE_INDEX{ x + (y * mapLayout_.tile_count_horiz) };

                auto const TILE_NUM_ORIG{
                    mapLayer.mapid_vec[static_cast<std::size_t>(TILE_INDEX)] };

                //This -1_id comes from the Tiled app that starts tile ids at 1 instead of 0.
                auto const TILE_NUM_ID_ADJ{
                    (TILE_NUM_ORIG == 0_id) ? 0_id : (TILE_NUM_ORIG - 1_id) };

                //get the texture/image this tile can be found in
                const map::TilesPanel & TILES_PANEL{ GetTilesPanelFromId(TILE_NUM_ID_ADJ) };
                mapLayer.tiles_panel_vec.push_back(TILES_PANEL);

                //adjust the tile number to start at one
                auto const TILE_ID{ (TILE_NUM_ID_ADJ - TILES_PANEL.first_id) + 1_id };

                //get a pointer to the current tile's quad
                sf::Vertex * quadPtr{
                    & mapLayer.vert_array[static_cast<std::size_t>((vertIndex++) * 4)] };

                //define its 4 corners on screen
                auto const POS_WIDTH{
                    static_cast<float>((x - TILE_OFFSETS.begin_x.Get()) * mapLayout_.tile_size_horiz) };

                auto const POS_WIDTH_PLUS_ONE_TILE{
                    POS_WIDTH + static_cast<float>(mapLayout_.tile_size_horiz) };

                auto const POS_HEIGHT{
                    static_cast<float>((y - TILE_OFFSETS.begin_y.Get()) * mapLayout_.tile_size_vert) };

                auto const POS_HEIGHT_PLUS_ONE_TILE{
                    POS_HEIGHT + static_cast<float>(mapLayout_.tile_size_vert) };

                quadPtr[0].position =
                    sf::Vector2f(POS_WIDTH, POS_HEIGHT);

                quadPtr[1].position =
                    sf::Vector2f(POS_WIDTH_PLUS_ONE_TILE, POS_HEIGHT);

                quadPtr[2].position =
                    sf::Vector2f(POS_WIDTH_PLUS_ONE_TILE, POS_HEIGHT_PLUS_ONE_TILE);

                quadPtr[3].position =
                    sf::Vector2f(POS_WIDTH, POS_HEIGHT_PLUS_ONE_TILE);

                //find its position in the tileset texture
                auto const TEXTURE_TILE_COUNT_HORIZ{
                    mapLayout_.texture_vec[TILES_PANEL.texture_index.Get()].getSize().x / mapLayout_.tile_size_horiz };

                auto const TILE_COUNT_U{
                    static_cast<float>(TILE_ID.As<unsigned>() % TEXTURE_TILE_COUNT_HORIZ) };

                auto const TILE_COUNT_V{
                    static_cast<float>(TILE_ID.As<unsigned>() / TEXTURE_TILE_COUNT_HORIZ) };

                auto const TEXTCOORD_WIDTH{
                    TILE_COUNT_U * static_cast<float>(mapLayout_.tile_size_horiz) };

                auto const TEXTCOORD_WIDTH_PLUS_ONE_TILE{
                    TEXTCOORD_WIDTH + static_cast<float>(mapLayout_.tile_size_horiz) };

                auto const TEXTCOORD_HEIGHT{
                    TILE_COUNT_V * static_cast<float>(mapLayout_.tile_size_vert) };

                auto const TEXTCOORD_HEIGHT_PLUS_ONE_TILE{
                    TEXTCOORD_HEIGHT + static_cast<float>(mapLayout_.tile_size_vert) };

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


    const map::TileOffsets TileMap::GetTileOffsetsOfPlayerPos() const
    {
        return GetTileOffsetsFromMapPos(playerPosMapV_);
    }


    const map::TileOffsets TileMap::GetTileOffsetsFromMapPos(const sf::Vector2f & MAP_POS_V) const
    {
        map::TileOffsets tileOffsets;

        auto const PLAYER_POSX_IN_TILES{ static_cast<unsigned>(MAP_POS_V.x) / mapLayout_.tile_size_horiz };
        auto const HALF_WIN_TILES_X{ (WIN_SIZE_V_.x / mapLayout_.tile_size_horiz) / 2 };

        if (PLAYER_POSX_IN_TILES >= HALF_WIN_TILES_X)
        {
            tileOffsets.begin_x = Count_t(PLAYER_POSX_IN_TILES - HALF_WIN_TILES_X);
        }
        else
        {
            tileOffsets.begin_x = 0_count;
        }

        tileOffsets.end_x = Count_t(PLAYER_POSX_IN_TILES + HALF_WIN_TILES_X);

        auto const TILE_WITH_OFFSET_COUNT_X{
            Count_t((WIN_SIZE_V_.x / mapLayout_.tile_size_horiz) + EXTRA_OFFSCREEN_TILE_COUNT_.As<unsigned>()) };

        if (tileOffsets.end_x < TILE_WITH_OFFSET_COUNT_X)
        {
            tileOffsets.end_x = TILE_WITH_OFFSET_COUNT_X;
        }

        auto const PLAYER_POSY_IN_TILES{ static_cast<unsigned>(MAP_POS_V.y) / mapLayout_.tile_size_vert };
        auto const HALF_WIN_TILES_Y{ (WIN_SIZE_V_.y / mapLayout_.tile_size_vert) / 2 };

        if (PLAYER_POSY_IN_TILES >= HALF_WIN_TILES_Y)
        {
            tileOffsets.begin_y = Count_t(PLAYER_POSY_IN_TILES - HALF_WIN_TILES_Y);
        }
        else
        {
            tileOffsets.begin_y = 0_count;
        }

        tileOffsets.end_y = Count_t(PLAYER_POSY_IN_TILES + HALF_WIN_TILES_Y);

        auto const TILE_WITH_OFFSET_COUNT_Y{
            Count_t((WIN_SIZE_V_.y / mapLayout_.tile_size_vert) + EXTRA_OFFSCREEN_TILE_COUNT_.As<unsigned>()) };

        if (tileOffsets.end_y < TILE_WITH_OFFSET_COUNT_Y)
        {
            tileOffsets.end_y = TILE_WITH_OFFSET_COUNT_Y;
        }

        if (tileOffsets.begin_x >= EXTRA_OFFSCREEN_TILE_COUNT_)
        {
            tileOffsets.begin_x -= EXTRA_OFFSCREEN_TILE_COUNT_;
        }
        else
        {
            tileOffsets.begin_x = 0_count;
        }

        if (tileOffsets.begin_y >= EXTRA_OFFSCREEN_TILE_COUNT_)
        {
            tileOffsets.begin_y -= EXTRA_OFFSCREEN_TILE_COUNT_;
        }
        else
        {
            tileOffsets.begin_y = 0_count;
        }

        if ((tileOffsets.end_x.As<unsigned>() + EXTRA_OFFSCREEN_TILE_COUNT_.As<unsigned>()) < mapLayout_.tile_count_horiz)
        {
            tileOffsets.end_x += EXTRA_OFFSCREEN_TILE_COUNT_;
        }
        else
        {
            tileOffsets.end_x = Count_t(mapLayout_.tile_count_horiz);
        }

        if ((tileOffsets.end_y.As<unsigned>() + EXTRA_OFFSCREEN_TILE_COUNT_.As<unsigned>()) < mapLayout_.tile_count_vert)
        {
            tileOffsets.end_y += EXTRA_OFFSCREEN_TILE_COUNT_;
        }
        else
        {
            tileOffsets.end_y = Count_t(mapLayout_.tile_count_vert);
        }

        auto const TILE_WITH_DOUBLE_OFFSET_COUNT_Y{
            TILE_WITH_OFFSET_COUNT_Y + EXTRA_OFFSCREEN_TILE_COUNT_ };

        if ((tileOffsets.end_y - tileOffsets.begin_y) != TILE_WITH_DOUBLE_OFFSET_COUNT_Y)
        {
            tileOffsets.begin_y = (tileOffsets.end_y - TILE_WITH_DOUBLE_OFFSET_COUNT_Y);
        }

        auto const TILE_WITH_DOUBLE_OFFSET_COUNT_X{
            TILE_WITH_OFFSET_COUNT_X + EXTRA_OFFSCREEN_TILE_COUNT_ };

        if ((tileOffsets.end_x - tileOffsets.begin_x) != TILE_WITH_DOUBLE_OFFSET_COUNT_X)
        {
            tileOffsets.begin_x = (tileOffsets.end_x - TILE_WITH_DOUBLE_OFFSET_COUNT_X);
        }

        return tileOffsets;
    }


    const sf::Vector2f TileMap::GetPlayerPosCollision() const
    {
        return (playerPosMapV_ + playerPosOffsetScreenV_);
    }


    const sf::Vector2f TileMap::GetPosScreen(const map::TileOffsets & TILE_OFFSETS) const
    {
        //Convert from off-screen coords to on-screen coords.
        auto const OFFSET_DIFFX{
            static_cast<float>(TILE_OFFSETS.end_x.As<int>() - TILE_OFFSETS.begin_x.As<int>()) };

        auto const OFFSET_DIFFY{
            static_cast<float>(TILE_OFFSETS.end_y.As<int>() - TILE_OFFSETS.begin_y.As<int>()) };

        auto const LEFT{ static_cast<float>(
            WIN_POS_V_.x + ((OFFSET_DIFFX * static_cast<float>(mapLayout_.tile_size_horiz)) * 0.5f)) };

        auto const TOP{ static_cast<float>(
            WIN_POS_V_.y + ((OFFSET_DIFFY * static_cast<float>(mapLayout_.tile_size_vert)) * 0.5f)) };

        return sf::Vector2f(LEFT, TOP);
    }


    const sf::Vector2f TileMap::GetPosScreen(const sf::Vector2f & MAP_POS_V) const
    {
        return GetPosScreen( GetTileOffsetsFromMapPos(MAP_POS_V) );
    }


    const sf::Vector2f TileMap::GetPlayerPosScreen() const
    {
        return GetPosScreen( GetTileOffsetsOfPlayerPos() ) + playerPosOffsetScreenV_;
    }


    bool TileMap::IsPointWithinCollision(const sf::Vector2f & POS_V) const
    {
        return collisionTree_.IsPointWithinCollisionRect(POS_V);
    }


    const map::TilesPanel & TileMap::GetTilesPanelFromId(const ID_t & ID) const
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


    void TileMap::AdjustOffscreenRectsToPreventDrift(const map::TileOffsets & CURRENT_TILE_OFFSETS)
    {
        if (CURRENT_TILE_OFFSETS.begin_x > prevTileOffsets_.begin_x)
        {
            if (offScreenRect_.left > 0.0f)
            {
                offScreenRect_.left -= static_cast<float>(mapLayout_.tile_size_horiz);

                if (offScreenRect_.left < 0.0f)
                {
                    offScreenRect_.left = 0.0f;
                }
            }
        }
        else
        {
            if (CURRENT_TILE_OFFSETS.begin_x < prevTileOffsets_.begin_x)
            {
                if (offScreenRect_.left > 0.0f)
                {
                    offScreenRect_.left += static_cast<float>(mapLayout_.tile_size_horiz);
                }
            }
        }

        if (CURRENT_TILE_OFFSETS.begin_y > prevTileOffsets_.begin_y)
        {
            if (offScreenRect_.top > 0.0f)
            {
                offScreenRect_.top -= static_cast<float>(mapLayout_.tile_size_vert);

                if (offScreenRect_.top < 0.0f)
                {
                    offScreenRect_.top = 0.0f;
                }
            }
        }
        else
        {
            if (CURRENT_TILE_OFFSETS.begin_y < prevTileOffsets_.begin_y)
            {
                offScreenRect_.top += static_cast<float>(mapLayout_.tile_size_vert);
            }
        }
    }

}
}
