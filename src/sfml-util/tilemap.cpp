// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
//
// tilemap.cpp
//
#include "tilemap.hpp"

#include "sfml-util/sfml-util.hpp"
#include "sfml-util/loaders.hpp"

#include "game/log-macros.hpp"
#include "misc/assertlogandthrow.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>

#include <sstream>
#include <exception>


namespace sfml_util
{
namespace map
{

    TileOffsets::TileOffsets()
    :
        begin_x(0),
        end_x  (0),
        begin_y(0),
        end_y  (0)
    {}


    bool operator==(const TileOffsets & L, const TileOffsets & R)
    {
        return ((L.begin_x == R.begin_x) &&
                (L.begin_y == R.begin_y) &&
                (L.end_x == R.end_x) &&
                (L.end_y == R.end_y));
    }


    inline bool operator!=(const TileOffsets & L, const TileOffsets & R)
    {
        return ! (L == R);
    }


    TilesImage::TilesImage(const std::string &   NAME,
                           const std::string &   RELATIVE_PATH,
                           const std::size_t     FIRST_ID,
                           const std::size_t     TILE_COUNT,
                           const std::size_t     COLUMN_COUNT,
                           const TextureSPtr_t & TEXTURE_SPTR)
    :
        name        (NAME),
        path_rel    (RELATIVE_PATH),
        path_obj    (),
        first_id    (FIRST_ID),
        tile_count  (TILE_COUNT),
        column_count(COLUMN_COUNT),
        texture_sptr(TEXTURE_SPTR)
    {}


    bool operator==(const TilesImage & L, const TilesImage & R)
    {
        return ((L.first_id == R.first_id) &&
                (L.texture_sptr == R.texture_sptr) &&
                (L.path_rel == R.path_rel) &&
                (L.path_obj == R.path_obj) &&
                (L.tile_count == R.tile_count) &&
                (L.column_count == R.column_count) &&
                (L.name == R.name));
    }


    bool operator!=(const TilesImage & L, const TilesImage & R)
    {
        return !(L == R);
    }


    const float QuadTree::MIN_QUAD_SIZE_(800.0f);


    QuadTree::QuadTree()
    :
        headQuad_()
    {}


    QuadTree::~QuadTree()
    {}


    void QuadTree::Setup(const float            MAP_WIDTH,
                         const float            MAP_HEIGHT,
                         const FloatRectVec_t & COLL_RECTS_VEC)
    {
        PopulateQuadAndRecurse(headQuad_, sf::FloatRect(0.0f, 0.0f, MAP_WIDTH, MAP_HEIGHT), COLL_RECTS_VEC);
    }


    void QuadTree::PopulateQuadAndRecurse(Quad &                 quad,
                                          const sf::FloatRect &  RECT,
                                          const FloatRectVec_t & COLL_RECTS_VEC)
    {
        for (auto const & NEXT_COLL_RECT : COLL_RECTS_VEC)
        {
            if ((RECT.left < (NEXT_COLL_RECT.left + NEXT_COLL_RECT.width)) &&
                ((RECT.left + RECT.width) > NEXT_COLL_RECT.left) &&
                (RECT.top < (NEXT_COLL_RECT.top + NEXT_COLL_RECT.height)) &&
                ((RECT.height + RECT.top) > NEXT_COLL_RECT.top))
            {
                quad.coll_rects_vec.push_back(NEXT_COLL_RECT);
            }
        }

        if ((quad.coll_rects_vec.empty() == false) && (RECT.width > MIN_QUAD_SIZE_) && (RECT.height > MIN_QUAD_SIZE_))
        {
            const float HALF_WIDTH(RECT.width * 0.5f);
            const float HALF_HEIGHT(RECT.height * 0.5f);
            quad.quad_rects_vec.push_back(sf::FloatRect(RECT.left, RECT.top, HALF_WIDTH, HALF_HEIGHT));
            quad.quad_rects_vec.push_back(sf::FloatRect(RECT.left + HALF_WIDTH, RECT.top, HALF_WIDTH, HALF_HEIGHT));
            quad.quad_rects_vec.push_back(sf::FloatRect(RECT.left, RECT.top + HALF_HEIGHT, HALF_WIDTH, HALF_HEIGHT));
            quad.quad_rects_vec.push_back(sf::FloatRect(RECT.left + HALF_WIDTH, RECT.top + HALF_HEIGHT, HALF_WIDTH, HALF_HEIGHT));

            for (std::size_t i(0); i < 4; ++i)
            {
                quad.child_quads_vec.push_back(Quad());
                PopulateQuadAndRecurse(quad.child_quads_vec[i], quad.quad_rects_vec[i], quad.coll_rects_vec);
            }

            quad.coll_rects_vec.clear();
        }
    }


    bool QuadTree::IsPointWithinCollisionRect(const float POS_LEFT, const float POS_TOP) const
    {
        return IsPointWithinCollisionRect_Impl(headQuad_, POS_LEFT, POS_TOP);
    }


    bool QuadTree::IsPointWithinCollisionRect_Impl(const Quad & QUAD,
                                                   const float  POS_LEFT,
                                                   const float  POS_TOP) const
    {
        if (QUAD.quad_rects_vec.empty())
        {
            for (auto const & NEXT_COLL_RECT : QUAD.coll_rects_vec)
                if (NEXT_COLL_RECT.contains(POS_LEFT, POS_TOP))
                    return true;

            return false;
        }
        else
        {
            if (POS_LEFT < QUAD.quad_rects_vec[1].left)
            {
                if (POS_TOP < QUAD.quad_rects_vec[2].top)
                    return IsPointWithinCollisionRect_Impl(QUAD.child_quads_vec[0], POS_LEFT, POS_TOP);
                else
                    return IsPointWithinCollisionRect_Impl(QUAD.child_quads_vec[2], POS_LEFT, POS_TOP);
            }
            else
            {
                if (POS_TOP < QUAD.quad_rects_vec[2].top)
                    return IsPointWithinCollisionRect_Impl(QUAD.child_quads_vec[1], POS_LEFT, POS_TOP);
                else
                    return IsPointWithinCollisionRect_Impl(QUAD.child_quads_vec[3], POS_LEFT, POS_TOP);
            }
        }
    }


    const float       TileMap::BORDER_PAD_                (75.0f);
    const std::string TileMap::TILES_IMAGE_NAME_EMPTY_    ("Empty");
    const std::string TileMap::XML_NODE_NAME_TILE_LAYER_  ("layer");
    const std::string TileMap::XML_NODE_NAME_OBJECT_LAYER_("objectgroup");
    const std::string TileMap::XML_NODE_NAME_TILESET_     ("tileset");
    const std::string TileMap::XML_ATTRIB_NAME_GROUND_    ("ground");
    const std::string TileMap::XML_ATTRIB_NAME_OBJECTS_   ("object");
    const std::string TileMap::XML_ATTRIB_NAME_SHADOWS_   ("shadow");
    const std::string TileMap::XML_ATTRIB_NAME_COLLISION_ ("collision");
    const unsigned    TileMap::EXTRA_OFFSCREEN_TILE_COUNT_(2);
    const sf::Color   TileMap::DEFAULT_TRANSPARENT_MASK_  (sf::Color(75, 99, 127));
    const sf::Color   TileMap::SHADOW_MASK1_              (sf::Color(0, 0, 0));
    const sf::Color   TileMap::SHADOW_MASK2_              (sf::Color(151, 0, 147));
    const sf::Color   TileMap::SHADOW_MASK2B_             (sf::Color(127, 0, 127));
    const sf::Color   TileMap::SHADOW_MASK3_              (sf::Color(255, 0, 255));
    const sf::Color   TileMap::SHADOW_COLOR1_             (sf::Color(0, 0, 0, 100));
    const sf::Color   TileMap::SHADOW_COLOR2_             (sf::Color(0, 0, 0, 65));
    const sf::Color   TileMap::SHADOW_COLOR3_             (sf::Color(0, 0, 0, 35));


    TileMap::TileMap(const std::string &  MAP_PATH_STR,
                     const sf::Vector2f & WIN_POS_V,
                     const sf::Vector2u & WIN_SIZE_V,
                     const sf::Vector2f & PLAYER_POS_V,
                     const sf::Color &    TRANSPARENT_MASK)
    :
        tileSizeWidth_   (0),
        tileSizeHeight_  (0),
        mapTileCountX_   (0),
        mapTileCountY_   (0),
        prevTileOffsets_ (),
        WIN_POS_V_       (WIN_POS_V),
        WIN_SIZE_V_      (WIN_SIZE_V),
        playerPosV_      (PLAYER_POS_V),
        playerPosOffsetV_(0.0f, 0.0f),
        tileTexture_     (),
        renderStates_    (sf::RenderStates::Default),
        offScreenRect_   (),
        mapSprite_       (),
        offScreenTexture_(),
        emptyRendText_   (),
        collisionsVec_   (),
        mapLayerSVec_    (),
        tilesImageSVec_  (),
        TRANSPARENT_MASK_(TRANSPARENT_MASK),
        collisionTree_   ()
    {
        //parse the .tmx xml file
        ParseMapFile(MAP_PATH_STR);

        //create off-screen texture
        //always ensure these are valid power of 2 sizes
        const unsigned WIDTH(WIN_SIZE_V_.x + (tileSizeWidth_  * (EXTRA_OFFSCREEN_TILE_COUNT_ + 2)));
        const unsigned HEIGHT(WIN_SIZE_V_.y + (tileSizeHeight_ * (EXTRA_OFFSCREEN_TILE_COUNT_ + 2)));
        //M_HP_LOG("Creating TileMap's off-screen sf::RenderTexture at size " << WIDTH << "x" << HEIGHT);
        M_ASSERT_OR_LOGANDTHROW_SS(offScreenTexture_.create(WIDTH, HEIGHT), "TileMap::TileMap(\"" << MAP_PATH_STR << "\"), failed to sf::RenderTexture::create(" << WIDTH << "x" << HEIGHT << ").");

        //set the initial position of what is drawn on-screen from the off-screen texture
        offScreenRect_.width = static_cast<float>(WIN_SIZE_V_.x);
        offScreenRect_.height = static_cast<float>(WIN_SIZE_V_.y);
        offScreenRect_.left = static_cast<float>(EXTRA_OFFSCREEN_TILE_COUNT_ * tileSizeWidth_);
        offScreenRect_.top = static_cast<float>(EXTRA_OFFSCREEN_TILE_COUNT_ * tileSizeHeight_);

        SetupMapSprite();

        ApplyColorMasksToHandleTransparency();

        collisionTree_.Setup(static_cast<float>(tileSizeWidth_ * mapTileCountX_),
                             static_cast<float>(tileSizeHeight_ * mapTileCountY_),
                             collisionsVec_);
    }


    void TileMap::ApplyColorMasksToHandleTransparency()
    {
        for (auto const & NEXT_TI_SPTR: tilesImageSVec_)
        {
            sf::Image srcImage(NEXT_TI_SPTR->texture_sptr->copyToImage());
            sf::Image destImage(srcImage);

            namespace ba = boost::algorithm;
            const bool IS_SHADOW_LAYER( ba::contains(ba::to_lower_copy(NEXT_TI_SPTR->name),
                                                     ba::to_lower_copy(XML_ATTRIB_NAME_SHADOWS_)) );

            const sf::Uint8 * const PIXEL_PTR(srcImage.getPixelsPtr());

            const std::size_t PIXEL_COUNT(static_cast<std::size_t>(srcImage.getSize().x * srcImage.getSize().y * static_cast<unsigned int>(4)));
            for (std::size_t i(0); i < PIXEL_COUNT; i+=4)
            {
                const sf::Uint8 RED  ( * (PIXEL_PTR + i + 0) );
                const sf::Uint8 GREEN( * (PIXEL_PTR + i + 1) );
                const sf::Uint8 BLUE ( * (PIXEL_PTR + i + 2) );
                //const sf::Uint8 ALPHA( * (PIXEL_PTR + i + 3) );

                const unsigned DEST_X((static_cast<unsigned>(i) / static_cast<unsigned>(4)) % destImage.getSize().x);
                const unsigned DEST_Y((static_cast<unsigned>(i) / static_cast<unsigned>(4)) / destImage.getSize().x);

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

            NEXT_TI_SPTR->texture_sptr->update(destImage);
        }
    }


    TileMap::~TileMap()
    {}


    bool TileMap::MoveUp(const float ADJUSTMENT)
    {
        //check for collision
        sf::Vector2f posTest(GetPlayerPosActual());
        posTest.y -= ADJUSTMENT;
        if (IsPointWithinCollision(posTest))
            return false;

        const sf::Vector2f CURR_SCREEN_POS( GetScreenPosPlayer() );

        //move within the view before moving the view
        if (CURR_SCREEN_POS.y > (WIN_POS_V_.y + BORDER_PAD_))
        {
            playerPosOffsetV_.y -= ADJUSTMENT;
            return true;
        }
        else
        {
            if (offScreenRect_.top > 0.0f)
            {
                offScreenRect_.top -= ADJUSTMENT;
                if (offScreenRect_.top < 0.0f)
                    offScreenRect_.top = 0.0f;

                SetupMapSprite();

                playerPosV_.y -= ADJUSTMENT;
                if (playerPosV_.y < 0.0f)
                    playerPosV_.y = 0.0f;

                return true;
            }
        }

        return false;
    }


    bool TileMap::MoveDown(const float ADJUSTMENT)
    {
        //check for collision
        sf::Vector2f posTest(GetPlayerPosActual());
        posTest.y += ADJUSTMENT;
        if (IsPointWithinCollision(posTest))
            return false;

        const sf::Vector2f CURR_SCREEN_POS( GetScreenPosPlayer() );

        //move within the view before moving the view
        if (CURR_SCREEN_POS.y < ((WIN_POS_V_.y + static_cast<float>(WIN_SIZE_V_.y)) - BORDER_PAD_))
        {
            playerPosOffsetV_.y += ADJUSTMENT;
            return true;
        }
        else
        {
            const float VERT_LIMIT(static_cast<float>(mapTileCountY_ * tileSizeHeight_) - static_cast<float>(WIN_SIZE_V_.y));

            if ((offScreenRect_.top + static_cast<float>(prevTileOffsets_.begin_y * tileSizeHeight_)) < VERT_LIMIT)
            {
                offScreenRect_.top += ADJUSTMENT;
                SetupMapSprite();
                playerPosV_.y += ADJUSTMENT;
                return true;
            }
        }

        return false;
    }


    bool TileMap::MoveLeft(const float ADJUSTMENT)
    {
        //check for collision
        sf::Vector2f posTest(GetPlayerPosActual());
        posTest.x -= ADJUSTMENT;
        if (IsPointWithinCollision(posTest))
            return false;

        const sf::Vector2f CURR_SCREEN_POS( GetScreenPosPlayer() );

        //move within the view before moving the view
        if (CURR_SCREEN_POS.x > (WIN_POS_V_.x + BORDER_PAD_))
        {
            playerPosOffsetV_.x -= ADJUSTMENT;
            return true;
        }
        else
        {
            if (offScreenRect_.left > 0.0f)
            {
                offScreenRect_.left -= ADJUSTMENT;
                if (offScreenRect_.left < 0.0f)
                    offScreenRect_.left = 0.0f;

                SetupMapSprite();

                playerPosV_.x -= ADJUSTMENT;
                if (playerPosV_.x < 0.0f)
                    playerPosV_.x = 0.0f;

                return true;
            }
        }

        return false;
    }


    bool TileMap::MoveRight(const float ADJUSTMENT)
    {
        //check for collision
        sf::Vector2f posTest(GetPlayerPosActual());
        posTest.x += ADJUSTMENT;
        if (IsPointWithinCollision(posTest))
            return false;

        const sf::Vector2f CURR_SCREEN_POS( GetScreenPosPlayer() );

        //move within the view before moving the view
        if (CURR_SCREEN_POS.x < ((WIN_POS_V_.x + static_cast<float>(WIN_SIZE_V_.x)) - BORDER_PAD_))
        {
            playerPosOffsetV_.x += ADJUSTMENT;
            return true;
        }
        else
        {
            const float HORIZ_LIMIT(static_cast<float>((mapTileCountX_ * tileSizeWidth_) - WIN_SIZE_V_.x));

            if ((offScreenRect_.left + static_cast<float>(prevTileOffsets_.begin_x * tileSizeWidth_)) < HORIZ_LIMIT)
            {
                offScreenRect_.left += ADJUSTMENT;
                SetupMapSprite();
                playerPosV_.x += ADJUSTMENT;
                return true;
            }
        }

        return false;
    }


    void TileMap::DrawMap(sf::RenderTarget & target)
    {
        //temp draw map outline
        sf::RenderStates states;
        sf::FloatRect mapOutlineRect(WIN_POS_V_.x, WIN_POS_V_.y, static_cast<float>(WIN_SIZE_V_.x), static_cast<float>(WIN_SIZE_V_.y));
        sfml_util::DrawRectangle(target, states, mapOutlineRect, sf::Color::Red);

        //check if player position requires changing the map tiles drawn
        const TileOffsets CURRENT_TILE_OFFSETS( GetTileOffsetsPlayerPos() );
        if (prevTileOffsets_ != CURRENT_TILE_OFFSETS)
        {
            //clear away the existing tiles
            const std::size_t NUM_LAYERS(mapLayerSVec_.size());
            for (std::size_t i(0); i < NUM_LAYERS; ++i)
            {
                mapLayerSVec_[i]->vert_array.clear();
                mapLayerSVec_[i]->tilesimage_svec.clear();
            }

            //establish what new tiles to draw
            for(std::size_t i(0); i<NUM_LAYERS; ++i)
                EstablishMapSubsection(mapLayerSVec_[i], CURRENT_TILE_OFFSETS);

            //adjust offScreenRect to keep the map from jumping every time the offsets change
            if (CURRENT_TILE_OFFSETS.begin_x > prevTileOffsets_.begin_x)
            {
                if (offScreenRect_.left > 0.0f)
                {
                    offScreenRect_.left -= static_cast<float>(tileSizeWidth_);

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
                        offScreenRect_.left += static_cast<float>(tileSizeWidth_);
                    }
                }
            }

            if (CURRENT_TILE_OFFSETS.begin_y > prevTileOffsets_.begin_y)
            {
                if (offScreenRect_.top > 0.0f)
                {
                    offScreenRect_.top -= static_cast<float>(tileSizeHeight_);

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
                    offScreenRect_.top += static_cast<float>(tileSizeHeight_);
                }
            }

            prevTileOffsets_ = CURRENT_TILE_OFFSETS;
            SetupMapSprite();
        }

        //clear the map to black
        offScreenTexture_.clear(sf::Color::Black);

        bool hasObjectTileBeenDrawnYet(false);

        //draw all vertex arrays to the off-screen texture
        sf::VertexArray quads(sf::Quads, 4);
        const std::size_t NUM_MAP_LAYERS(mapLayerSVec_.size());
        for (std::size_t i(0); i < NUM_MAP_LAYERS; ++i)
        {
            std::size_t tilesImageIndex(0);
            const std::size_t NUM_VERTS(mapLayerSVec_[i]->vert_array.getVertexCount());
            for (std::size_t j(0); j < NUM_VERTS; )
            {
                //draw player before objects that might be drawn on top of the player
                if (false == hasObjectTileBeenDrawnYet)
                {
                    namespace ba = boost::algorithm;

                    if (false == ba::contains(ba::to_lower_copy(mapLayerSVec_[i]->tilesimage_svec[tilesImageIndex]->name), XML_ATTRIB_NAME_GROUND_))
                    {
                        offScreenTexture_.display();
                        target.draw(mapSprite_);
                        offScreenTexture_.clear(sf::Color::Transparent);
                        DrawPlayer(target);
                        hasObjectTileBeenDrawnYet = true;
                    }
                }

                //skip drawing empty tiles
                if (mapLayerSVec_[i]->tilesimage_svec[tilesImageIndex]->name != TILES_IMAGE_NAME_EMPTY_)
                {
                    quads[0].position  = mapLayerSVec_[i]->vert_array[j].position;
                    quads[0].texCoords = mapLayerSVec_[i]->vert_array[j++].texCoords;

                    quads[1].position  = mapLayerSVec_[i]->vert_array[j].position;
                    quads[1].texCoords = mapLayerSVec_[i]->vert_array[j++].texCoords;

                    quads[2].position  = mapLayerSVec_[i]->vert_array[j].position;
                    quads[2].texCoords = mapLayerSVec_[i]->vert_array[j++].texCoords;

                    quads[3].position  = mapLayerSVec_[i]->vert_array[j].position;
                    quads[3].texCoords = mapLayerSVec_[i]->vert_array[j++].texCoords;

                    renderStates_.texture = mapLayerSVec_[i]->tilesimage_svec[tilesImageIndex++]->texture_sptr.get();

                    offScreenTexture_.draw(quads, renderStates_);
                }
                else
                {
                    ++tilesImageIndex;
                    j += 4;
                }
            }
        }

        offScreenTexture_.display();
        target.draw(mapSprite_);
    }


    void TileMap::DrawPlayer(sf::RenderTarget & target)
    {
        sf::CircleShape c(5);
        c.setFillColor(sf::Color(255, 0, 0));
        c.setPosition(GetScreenPosPlayer());
        target.draw(c);
    }


    void TileMap::SetupMapSprite()
    {
        mapSprite_.setTexture(offScreenTexture_.getTexture());
        mapSprite_.setPosition(WIN_POS_V_);
        mapSprite_.setTextureRect(sfml_util::ConvertRect<float, int>(offScreenRect_));
    }


    void TileMap::ParseMapFile(const std::string & MAP_PATH_STR)
    {
        try
        {
            boost::property_tree::ptree pt;

            //read in the .tmx file's xml data
            try
            {
                boost::property_tree::read_xml(MAP_PATH_STR, pt);
            }
            catch (const std::exception & E)
            {
                M_HP_LOG("TileMap::ParseMapFile(\"" << MAP_PATH_STR << "\") boost::property_tree::read_xml() threw '" << E.what() << "' exception.");
                throw;
            }
            catch (...)
            {
                std::ostringstream ss;
                ss << "TileMap::ParseMapFile(\"" << MAP_PATH_STR << "\") boost::property_tree::read_xml() threw an unknown exception.";
                M_HP_LOG(ss.str());
                throw std::runtime_error(ss.str());
            }

            //get tile and map info
            const boost::property_tree::ptree IMAGE_PT(pt.get_child("map"));
            tileSizeWidth_  = IMAGE_PT.get<unsigned>("<xmlattr>.tilewidth");
            tileSizeHeight_ = IMAGE_PT.get<unsigned>("<xmlattr>.tileheight");
            mapTileCountX_  = IMAGE_PT.get<unsigned>("<xmlattr>.width");
            mapTileCountY_  = IMAGE_PT.get<unsigned>("<xmlattr>.height");

            SetupEmptyTexture();

            //calculate which tiles need to be drawn around the player position
            const TileOffsets CURRENT_TILE_OFFSETS( GetTileOffsetsPlayerPos() );

            mapLayerSVec_.clear();

            //loop over all layers in the map file
            for(const boost::property_tree::ptree::value_type & CHILD : pt.get_child("map"))
            {
                namespace ba = boost::algorithm;

                const std::string NODE_NAME_LOWER(ba::to_lower_copy(CHILD.first));

                const bool WILL_PARSE( (ba::contains(NODE_NAME_LOWER, ba::to_lower_copy(XML_NODE_NAME_TILE_LAYER_)))   ||
                                       (ba::contains(NODE_NAME_LOWER, ba::to_lower_copy(XML_NODE_NAME_OBJECT_LAYER_))) ||
                                       (ba::contains(NODE_NAME_LOWER, ba::to_lower_copy(XML_NODE_NAME_TILESET_))) );

                const std::string LAYER_NAME_LOWER((WILL_PARSE) ? ba::to_lower_copy(CHILD.second.get<std::string>("<xmlattr>.name")) : "");

                if (WILL_PARSE)
                {
                    M_HP_LOG("TileMap::ParseMapFile(\"" << MAP_PATH_STR << "\") parsing \"" << NODE_NAME_LOWER << "\", named \"" << LAYER_NAME_LOWER << "\"");
                }
                else
                {
                    M_HP_LOG("TileMap::ParseMapFile(\"" << MAP_PATH_STR << "\") skipping \"" << NODE_NAME_LOWER << "\"");
                    continue;
                }

                if (ba::contains(NODE_NAME_LOWER, ba::to_lower_copy(XML_NODE_NAME_OBJECT_LAYER_)))
                {
                    ParseCollisionsLayer(CHILD.second);
                    continue;
                }

                if (ba::contains(NODE_NAME_LOWER, ba::to_lower_copy(XML_NODE_NAME_TILESET_)))
                {
                    ParseTilesetLayer(MAP_PATH_STR, CHILD.second);
                    continue;
                }

                //parse all other layers as generic tile map layers

                //setup vars to parse the map layer
                const std::string RAW_CSV(CHILD.second.get_child("data").data());
                std::stringstream ssAll;
                ssAll << RAW_CSV;

                //create the next layer data structure
                MapLayerSPtr_t mapLayerSPtr(new MapLayer);
                mapLayerSVec_.push_back(mapLayerSPtr);

                //read in the next map layer
                ParseTileLayer(mapLayerSPtr->mapval_vec, ssAll);

                //establish which subsection of the map's tiles will be drawn to screen
                EstablishMapSubsection(mapLayerSPtr, CURRENT_TILE_OFFSETS);
            }
        }
        catch (const std::exception & E)
        {
            M_HP_LOG("TileMap::ParseMapFile(" << MAP_PATH_STR << ") boost::property_tree functions threw '" << E.what() << "' exception.");
            throw;
        }
        catch (...)
        {
            std::ostringstream ss;
            ss << "TileMap::ParseMapFile(" << MAP_PATH_STR << ") boost::property_tree functions threw an unknown exception.";
            M_HP_LOG(ss.str());
            throw std::runtime_error(ss.str());
        }
    }


    void TileMap::ParseTilesetLayer(const std::string &                 MAP_PATH_STR,
                                    const boost::property_tree::ptree & TILESET_PT)
    {
        const boost::property_tree::ptree IMAGE_PT(TILESET_PT.get_child("image"));

        TilesImageSPtr_t tiSPtr( new TilesImage(TILESET_PT.get<std::string>("<xmlattr>.name"),
                                                IMAGE_PT.get<std::string>("<xmlattr>.source"),
                                                TILESET_PT.get<std::size_t>("<xmlattr>.firstgid"),
                                                TILESET_PT.get<std::size_t>("<xmlattr>.tilecount"),
                                                TILESET_PT.get<std::size_t>("<xmlattr>.columns")) );
        namespace bfs = boost::filesystem;
        tiSPtr->path_obj = bfs::system_complete((bfs::current_path() / bfs::path(MAP_PATH_STR)) / (bfs::path("..") / bfs::path(tiSPtr->path_rel)));

        sfml_util::LoadImageOrTextureSPtr(tiSPtr->texture_sptr, tiSPtr->path_obj.string());

        tilesImageSVec_.push_back(tiSPtr);
    }


    void TileMap::ParseCollisionsLayer(const boost::property_tree::ptree & OBJ_GROUP_PT)
    {
        namespace ba = boost::algorithm;

        for (const boost::property_tree::ptree::value_type & CHILD : OBJ_GROUP_PT)
        {
            if (ba::contains(ba::to_lower_copy(CHILD.first), ba::to_lower_copy(XML_ATTRIB_NAME_OBJECTS_)))
            {
                const float LEFT  (CHILD.second.get<float>("<xmlattr>.x") - 20.0f);//Not sure why these magic numbers are needed...zTn 2016-11-1
                const float TOP   (CHILD.second.get<float>("<xmlattr>.y") - 40.0f);
                const float WIDTH (CHILD.second.get<float>("<xmlattr>.width"));
                const float HEIGHT(CHILD.second.get<float>("<xmlattr>.height"));
                const sf::FloatRect RECT(LEFT, TOP, WIDTH, HEIGHT);
                collisionsVec_.push_back(RECT);
            }
        }
    }


    void TileMap::ParseTileLayer(MapValVec_t & map, std::stringstream & ssLayerData)
    {
        //loop over each line of the csv, skipping newlines
        std::string nextLine("");
        while (ssLayerData)
        {
            std::getline(ssLayerData, nextLine, '\n');

            if (nextLine.empty())
                continue;

            std::stringstream ssLine;
            ssLine << nextLine;
            std::string nextVal("");

            //loop through each line reading numbers between the commas
            while (ssLine)
            {
                std::getline(ssLine, nextVal, ',');

                if (nextVal.empty())
                    continue;

                try
                {
                    map.push_back(boost::lexical_cast<MapVal_t>(nextVal));
                }
                catch (const std::exception & E)
                {
                    M_HP_LOG("TileMap::ParseMapLayer(" << nextLine << ") "
                             << "boost::lexical_cast<" << boost::typeindex::type_id<MapVal_t>().pretty_name() << "("
                             << "\"" << nextVal << "\") threw '"
                             << E.what() << "' exception.");

                    throw;
                }
                catch (...)
                {
                    std::ostringstream ss;
                    ss << "TileMap::ParseMapLayer(" << nextLine << ") "
                       << "boost::lexical_cast<" << boost::typeindex::type_id<MapVal_t>().pretty_name() << "("
                       << "\"" << nextVal << "\") threw unknown exception.";

                    M_HP_LOG(ss.str());
                    throw std::runtime_error(ss.str());
                }
            }
        }
    }


    void TileMap::EstablishMapSubsection(MapLayerSPtr_t mapLayerSPtr, const TileOffsets & TILE_OFFSETS)
    {
        // resize the vertex array to fit the level size
        const unsigned TILE_WIDTH (TILE_OFFSETS.end_x - TILE_OFFSETS.begin_x);
        const unsigned TILE_HEIGHT(TILE_OFFSETS.end_y - TILE_OFFSETS.begin_y);
        mapLayerSPtr->vert_array.setPrimitiveType(sf::Quads);
        mapLayerSPtr->vert_array.resize(TILE_WIDTH * TILE_HEIGHT * 4);

        // populate the vertex array with one quad per tile
        unsigned vertIndex(0);
        for (unsigned x(TILE_OFFSETS.begin_x); x < TILE_OFFSETS.end_x; ++x)
        {
            for (unsigned y(TILE_OFFSETS.begin_y); y < TILE_OFFSETS.end_y; ++y)
            {
                // get the current tile number
                const unsigned TILE_INDEX(x + (y * mapTileCountX_));
                const unsigned TILE_NUM_ORIG(mapLayerSPtr->mapval_vec[static_cast<std::size_t>(TILE_INDEX)]);
                const unsigned TILE_NUM_ADJ((TILE_NUM_ORIG == 0) ? 0 : (TILE_NUM_ORIG - 1)); //This -1 comes from the Tiled app that starts tile ids at 1 instead of 0.

                // get the texture/image this tile can be found in
                const TilesImageSPtr_t TILES_IMAGE_PTR(GetTileImageFromId(TILE_NUM_ADJ) );
                mapLayerSPtr->tilesimage_svec.push_back(TILES_IMAGE_PTR);

                // adjust the tile number to start at one
                const unsigned TILE_NUM((TILE_NUM_ADJ - static_cast<unsigned int>(TILES_IMAGE_PTR->first_id)) + static_cast<unsigned int>(1));

                // get a pointer to the current tile's quad
                sf::Vertex * quad( & mapLayerSPtr->vert_array[static_cast<std::size_t>((vertIndex++) * 4)]);

                // define its 4 corners on screen
                const float CURR_HORIZ_SIZE               (static_cast<float>((x - TILE_OFFSETS.begin_x) * tileSizeWidth_));
                const float CURR_HORIZ_SIZE_PLUS_ONE_TILE (CURR_HORIZ_SIZE + static_cast<float>(tileSizeWidth_));

                const float CURR_VERT_SIZE                (static_cast<float>((y - TILE_OFFSETS.begin_y) * tileSizeHeight_));
                const float CURR_VERT_SIZE_PLUS_ONE_TILE  (CURR_VERT_SIZE + static_cast<float>(tileSizeHeight_));

                quad[0].position = sf::Vector2f(CURR_HORIZ_SIZE,                CURR_VERT_SIZE);
                quad[1].position = sf::Vector2f(CURR_HORIZ_SIZE_PLUS_ONE_TILE,  CURR_VERT_SIZE);
                quad[2].position = sf::Vector2f(CURR_HORIZ_SIZE_PLUS_ONE_TILE,  CURR_VERT_SIZE_PLUS_ONE_TILE);
                quad[3].position = sf::Vector2f(CURR_HORIZ_SIZE,                CURR_VERT_SIZE_PLUS_ONE_TILE);

                // find its position in the tileset texture
                const unsigned TEXTURE_TILE_COUNT_HORIZ(TILES_IMAGE_PTR->texture_sptr->getSize().x / tileSizeWidth_);
                const float TU( static_cast<float>(TILE_NUM % TEXTURE_TILE_COUNT_HORIZ) );
                const float TV( static_cast<float>(TILE_NUM / TEXTURE_TILE_COUNT_HORIZ) );

                const float TEXT_COORD_HORIZ_SIZE               (TU * static_cast<float>(tileSizeWidth_) );
                const float TEXT_COORD_HORIZ_SIZE_PLUS_ONE_TILE (TEXT_COORD_HORIZ_SIZE + static_cast<float>(tileSizeWidth_));

                const float TEXT_COORD_VERT_SIZE                (TV * static_cast<float>(tileSizeHeight_));
                const float TEXT_COORD_VERT_SIZE_PLUS_ONE_TILE  (TEXT_COORD_VERT_SIZE + static_cast<float>(tileSizeHeight_));

                // define its 4 texture coordinates
                quad[0].texCoords = sf::Vector2f(TEXT_COORD_HORIZ_SIZE,                 TEXT_COORD_VERT_SIZE);
                quad[1].texCoords = sf::Vector2f(TEXT_COORD_HORIZ_SIZE_PLUS_ONE_TILE,   TEXT_COORD_VERT_SIZE);
                quad[2].texCoords = sf::Vector2f(TEXT_COORD_HORIZ_SIZE_PLUS_ONE_TILE,   TEXT_COORD_VERT_SIZE_PLUS_ONE_TILE);
                quad[3].texCoords = sf::Vector2f(TEXT_COORD_HORIZ_SIZE,                 TEXT_COORD_VERT_SIZE_PLUS_ONE_TILE);
            }
        }
    }


    const TileOffsets TileMap::GetTileOffsetsPlayerPos() const
    {
        return GetTileOffsets(playerPosV_);
    }


    const TileOffsets TileMap::GetTileOffsets(const sf::Vector2f & POS_V) const
    {
        TileOffsets t;

        const unsigned PLAYER_POSX_IN_TILES   (static_cast<unsigned>(POS_V.x) / tileSizeWidth_);
        const unsigned HALF_WIN_TILES_X       ((WIN_SIZE_V_.x / tileSizeWidth_) / 2);

        if (PLAYER_POSX_IN_TILES >= HALF_WIN_TILES_X)
            t.begin_x = (PLAYER_POSX_IN_TILES - HALF_WIN_TILES_X);
        else
            t.begin_x = 0;

        t.end_x = (PLAYER_POSX_IN_TILES + HALF_WIN_TILES_X);

        const unsigned TILE_WITH_OFFSET_COUNT_X((WIN_SIZE_V_.x / tileSizeWidth_) + EXTRA_OFFSCREEN_TILE_COUNT_);
        if (t.end_x < TILE_WITH_OFFSET_COUNT_X)
            t.end_x = TILE_WITH_OFFSET_COUNT_X;

        const unsigned PLAYER_POSY_IN_TILES   (static_cast<unsigned>(POS_V.y) / tileSizeHeight_);
        const unsigned HALF_WIN_TILES_Y       ((WIN_SIZE_V_.y / tileSizeHeight_) / 2);

        if (PLAYER_POSY_IN_TILES >= HALF_WIN_TILES_Y)
            t.begin_y = (PLAYER_POSY_IN_TILES - HALF_WIN_TILES_Y);
        else
            t.begin_y = 0;

        t.end_y = (PLAYER_POSY_IN_TILES + HALF_WIN_TILES_Y);

        const unsigned TILE_WITH_OFFSET_COUNT_Y((WIN_SIZE_V_.y / tileSizeHeight_) + EXTRA_OFFSCREEN_TILE_COUNT_);
        if (t.end_y < TILE_WITH_OFFSET_COUNT_Y)
            t.end_y = TILE_WITH_OFFSET_COUNT_Y;

        if (t.begin_x >= EXTRA_OFFSCREEN_TILE_COUNT_)
            t.begin_x -= EXTRA_OFFSCREEN_TILE_COUNT_;
        else
            t.begin_x = 0;

        if (t.begin_y >= EXTRA_OFFSCREEN_TILE_COUNT_)
            t.begin_y -= EXTRA_OFFSCREEN_TILE_COUNT_;
        else
            t.begin_y = 0;

        if ((t.end_x + EXTRA_OFFSCREEN_TILE_COUNT_) < mapTileCountX_)
            t.end_x += EXTRA_OFFSCREEN_TILE_COUNT_;
        else
            t.end_x = mapTileCountX_;

        if ((t.end_y + EXTRA_OFFSCREEN_TILE_COUNT_) < mapTileCountY_)
            t.end_y += EXTRA_OFFSCREEN_TILE_COUNT_;
        else
            t.end_y = mapTileCountY_;

        const unsigned TILE_WITH_DOUBLE_OFFSET_COUNT_Y(TILE_WITH_OFFSET_COUNT_Y + EXTRA_OFFSCREEN_TILE_COUNT_);
        if ((t.end_y - t.begin_y) != TILE_WITH_DOUBLE_OFFSET_COUNT_Y)
            t.begin_y = (t.end_y - TILE_WITH_DOUBLE_OFFSET_COUNT_Y);

        const unsigned TILE_WITH_DOUBLE_OFFSET_COUNT_X(TILE_WITH_OFFSET_COUNT_X + EXTRA_OFFSCREEN_TILE_COUNT_);
        if ((t.end_x - t.begin_x) != TILE_WITH_DOUBLE_OFFSET_COUNT_X)
            t.begin_x = (t.end_x - TILE_WITH_DOUBLE_OFFSET_COUNT_X);

        return t;
    }


    const sf::Vector2f TileMap::GetPlayerPosActual() const
    {
        return (playerPosV_ + playerPosOffsetV_);
    }


    const sf::Vector2f TileMap::GetScreenPos(const TileOffsets & TILE_OFFSETS) const
    {
        //Convert from off-screen coords to on-screen coords.
        const float OFFSET_DIFFX(static_cast<float>(TILE_OFFSETS.end_x - TILE_OFFSETS.begin_x));
        const float OFFSET_DIFFY(static_cast<float>(TILE_OFFSETS.end_y - TILE_OFFSETS.begin_y));

        sf::Vector2f posMappedV(static_cast<float>(WIN_POS_V_.x + ((OFFSET_DIFFX * static_cast<float>(tileSizeWidth_)) * 0.5f)),
                                static_cast<float>(WIN_POS_V_.y + ((OFFSET_DIFFY * static_cast<float>(tileSizeHeight_)) * 0.5f)));

        return posMappedV;
    }


    const sf::Vector2f TileMap::GetScreenPosPlayer() const
    {
        return GetScreenPos( GetTileOffsetsPlayerPos() ) + playerPosOffsetV_;
    }


    bool TileMap::IsPointWithinCollision(const sf::Vector2f & POS_V) const
    {
        /*
        for (auto const & NEXT_RECT: collisionsVec_)
            if (NEXT_RECT.contains(POS_V))
                return true;

        return false;
        */
        return collisionTree_.IsPointWithinCollisionRect(POS_V.x, POS_V.y);
    }


    const TilesImageSPtr_t TileMap::GetTileImageFromId(const MapVal_t ID) const
    {
        TilesImageSPtr_t tiSPtr;

        for (auto const & NEXT_TI_SPTR : tilesImageSVec_)
        {
            if (NEXT_TI_SPTR->OwnsId(ID))
            {
                tiSPtr = NEXT_TI_SPTR;
                break;
            }
        }

        M_ASSERT_OR_LOGANDTHROW_SS((tiSPtr->texture_sptr.get() != nullptr), "TileMap::GetTextureFromId(" << ID << ") failed to find an owning texture.");
        return tiSPtr;
    }


    void TileMap::SetupEmptyTexture()
    {
        M_ASSERT_OR_LOGANDTHROW_SS(emptyRendText_.create(tileSizeWidth_, tileSizeHeight_), "TileMap::SetupEmptyTexture() sf::RenderTexture::create(" << tileSizeWidth_ << "x" << tileSizeHeight_ << ") failed.");

        emptyRendText_.clear(sf::Color::Transparent);
        emptyRendText_.display();

        TextureSPtr_t textureSPtr( new sf::Texture );
        * textureSPtr = emptyRendText_.getTexture();

        TilesImageSPtr_t tiSPtr( new TilesImage(TILES_IMAGE_NAME_EMPTY_, "", 0, 1, 1, textureSPtr) );

        tilesImageSVec_.push_back(tiSPtr);
    }

}
}
