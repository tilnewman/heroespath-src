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
#include "game/game-data-file.hpp"

#include "misc/assertlogandthrow.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/algorithm.hpp>
#include <boost/algorithm/string.hpp>

#include <sstream>
#include <exception>


namespace heroespath
{
namespace sfml_util
{

    TileImage::TileImage(
        const std::string & NAME,
        const std::string & RELATIVE_PATH,
        const ID_t & FIRST_ID,
        const Count_t & TILE_COUNT,
        const Column_t & COLUMN,
        const Index_t & TEXTURE_INDEX)
    :
        name         (NAME),
        path_rel     (RELATIVE_PATH),
        first_id     (FIRST_ID),
        tile_count   (TILE_COUNT),
        column       (COLUMN),
        texture_index(TEXTURE_INDEX),
        path_obj     ()
    {}


    const float       TileMap::BORDER_PAD_                { 75.0f };
    const std::string TileMap::TILE_IMAGE_NAME_EMPTY_     { "Empty" };
    const std::string TileMap::XML_NODE_NAME_TILE_LAYER_  { "layer" };
    const std::string TileMap::XML_NODE_NAME_OBJECT_LAYER_{ "objectgroup" };
    const std::string TileMap::XML_NODE_NAME_TILESET_     { "tileset" };
    const std::string TileMap::XML_ATTRIB_NAME_GROUND_    { "ground" };
    const std::string TileMap::XML_ATTRIB_NAME_OBJECTS_   { "object" };
    const std::string TileMap::XML_ATTRIB_NAME_SHADOWS_   { "shadow" };
    const std::string TileMap::XML_ATTRIB_NAME_COLLISION_ { "collision" };
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
        tileSizeWidth_   (0),
        tileSizeHeight_  (0),
        mapTileCountX_   (0),
        mapTileCountY_   (0),
        prevTileOffsets_ (),
        WIN_POS_V_       (WIN_POS_V),
        WIN_SIZE_V_      (WIN_SIZE_V),
        playerPosMapV_   (PLAYER_POS_V),
        playerPosOffsetScreenV_(0.0f, 0.0f),
        renderStates_    (sf::RenderStates::Default),
        offScreenRect_   (),
        mapSprite_       (),
        offScreenTexture_(),
        emptyRendText_   (),
        collisionRectsVec_(),
        mapLayers_       (),
        tilesImageVec_   (),
        TRANSPARENT_MASK_(TRANSPARENT_MASK),
        collisionTree_   (),
        textures_        ()
    {
        //parse the .tmx xml file
        ParseMapFile(MAP_PATH_STR);

        //create off-screen texture
        //always ensure these are valid power of 2 sizes
        auto const WIDTH{
            WIN_SIZE_V_.x + (tileSizeWidth_  * (EXTRA_OFFSCREEN_TILE_COUNT_.As<unsigned>() + 2)) };

        auto const HEIGHT{
            WIN_SIZE_V_.y + (tileSizeHeight_ * (EXTRA_OFFSCREEN_TILE_COUNT_.As<unsigned>() + 2)) };

        M_ASSERT_OR_LOGANDTHROW_SS(offScreenTexture_.create(WIDTH, HEIGHT),
            "sfml_util::TileMap::TileMap(\"" << MAP_PATH_STR
            << "\"), failed to sf::RenderTexture::create("<< WIDTH << "x" << HEIGHT << ").");

        //set the initial position of what is drawn on-screen from the off-screen texture
        offScreenRect_.width = static_cast<float>(WIN_SIZE_V_.x);
        offScreenRect_.height = static_cast<float>(WIN_SIZE_V_.y);

        offScreenRect_.left =
            static_cast<float>(EXTRA_OFFSCREEN_TILE_COUNT_.As<unsigned>() * tileSizeWidth_);

        offScreenRect_.top =
            static_cast<float>(EXTRA_OFFSCREEN_TILE_COUNT_.As<unsigned>() * tileSizeHeight_);

        SetupMapSprite();

        ApplyColorMasksToHandleTransparency();

        collisionTree_.Setup(
            static_cast<float>(tileSizeWidth_ * mapTileCountX_),
            static_cast<float>(tileSizeHeight_ * mapTileCountY_),
            collisionRectsVec_);
    }


    void TileMap::ApplyColorMasksToHandleTransparency()
    {
        for (auto & nextTileImage: tilesImageVec_)
        {
            sf::Image srcImage(textures_[nextTileImage.texture_index.Get()].copyToImage());
            sf::Image destImage(srcImage);

            namespace ba = boost::algorithm;
            auto const IS_SHADOW_LAYER{ ba::contains(
                ba::to_lower_copy(nextTileImage.name),
                ba::to_lower_copy(XML_ATTRIB_NAME_SHADOWS_)) };

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

            textures_[nextTileImage.texture_index.Get()].update(destImage);
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
                    static_cast<float>(prevTileOffsets_.begin_y.As<unsigned>() * tileSizeHeight_) };

            auto const VERT_LIMIT{
                static_cast<float>(mapTileCountY_ * tileSizeHeight_) -
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
                    static_cast<float>(prevTileOffsets_.begin_x.As<unsigned>() * tileSizeWidth_)) };

            auto const HORIZ_LIMIT{
                static_cast<float>((mapTileCountX_ * tileSizeWidth_) - WIN_SIZE_V_.x) };

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


    void TileMap::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        //check if player position requires changing the map tiles drawn
        auto const CURRENT_TILE_OFFSETS{ GetTileOffsetsOfPlayerPos() };
        if (prevTileOffsets_ != CURRENT_TILE_OFFSETS)
        {
            //clear away the existing tiles
            for(auto & nextMapLayer : mapLayers_)
            {
                nextMapLayer.vert_array.clear();
                nextMapLayer.tilesimage_vec.clear();
            }

            //establish what new tiles to draw
            for (auto & nextMapLayer : mapLayers_)
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
        for (auto const & NEXT_MAP_LAYER : mapLayers_)
        {
            std::size_t tilesImageIndex{ 0 };
            auto const NUM_VERTS{ NEXT_MAP_LAYER.vert_array.getVertexCount() };
            for (std::size_t vertIndex(0); vertIndex < NUM_VERTS; )
            {
                //draw player before objects that might be drawn on top of the player
                if (false == hasObjectTileBeenDrawnYet)
                {
                    namespace ba = boost::algorithm;

                    auto const IS_GROUND_LAYER{ ba::contains(
                            ba::to_lower_copy(NEXT_MAP_LAYER.tilesimage_vec[tilesImageIndex].name),
                            XML_ATTRIB_NAME_GROUND_) };

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
                if (NEXT_MAP_LAYER.tilesimage_vec[tilesImageIndex].name != TILE_IMAGE_NAME_EMPTY_)
                {
                    quads[0].position  = NEXT_MAP_LAYER.vert_array[vertIndex].position;
                    quads[0].texCoords = NEXT_MAP_LAYER.vert_array[vertIndex++].texCoords;

                    quads[1].position  = NEXT_MAP_LAYER.vert_array[vertIndex].position;
                    quads[1].texCoords = NEXT_MAP_LAYER.vert_array[vertIndex++].texCoords;

                    quads[2].position  = NEXT_MAP_LAYER.vert_array[vertIndex].position;
                    quads[2].texCoords = NEXT_MAP_LAYER.vert_array[vertIndex++].texCoords;

                    quads[3].position  = NEXT_MAP_LAYER.vert_array[vertIndex].position;
                    quads[3].texCoords = NEXT_MAP_LAYER.vert_array[vertIndex++].texCoords;

                    renderStates_.texture = & textures_[
                        NEXT_MAP_LAYER.tilesimage_vec[tilesImageIndex++].texture_index.As<unsigned>()];

                    offScreenTexture_.draw(quads, renderStates_);
                }
                else
                {
                    ++tilesImageIndex;
                    auto const VERTEXES_PER_QUAD{ 4 };
                    vertIndex += VERTEXES_PER_QUAD;
                }
            }
        }

        offScreenTexture_.display();
        target.draw(mapSprite_);
        DrawPlayer(target);

        //TEMP TODO REMOVE - draw collision rects on screen
        for (auto const & COL_RECT : collisionRectsVec_)
        {
            auto const SCREEN_POS_TOP_LEFT{
                GetPosScreen(sf::Vector2f(COL_RECT.left, COL_RECT.top)) };

            auto const SCREEN_POS_BOT_RIGHT{
                GetPosScreen(sf::Vector2f(
                    COL_RECT.left + COL_RECT.width,
                    COL_RECT.top + COL_RECT.height)) };

            const sf::FloatRect SCREEN_RECT(
                SCREEN_POS_TOP_LEFT.x,
                SCREEN_POS_TOP_LEFT.y,
                SCREEN_POS_BOT_RIGHT.x - SCREEN_POS_TOP_LEFT.x,
                SCREEN_POS_BOT_RIGHT.y - SCREEN_POS_TOP_LEFT.y);

            M_HP_LOG_DBG("\t********** " << sfml_util::RectToString(COL_RECT) << ",  " << sfml_util::RectToString(SCREEN_RECT))
            sfml_util::DrawRectangle(target, STATES, SCREEN_RECT);
        }
        M_HP_LOG_DBG("\t**********");
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


    void TileMap::ParseMapFile(const std::string & MAP_FILE_PATH_STR)
    {
        ParseMapFile_TryCatchAndLog(MAP_FILE_PATH_STR);
    }


    void TileMap::ParseMapFile_TryCatchAndLog(const std::string & MAP_FILE_PATH_STR)
    {
        try
        {
            ParseMapFile_Implementation(MAP_FILE_PATH_STR);
        }
        catch (const std::exception & E)
        {
            M_HP_LOG_ERR("sfml_util::TileMap::ParseMapFile_TryCatchAndLog(" << MAP_FILE_PATH_STR
                << ") boost::property_tree functions threw '" << E.what() << "' exception.");

            throw;
        }
        catch (...)
        {
            std::ostringstream ss;
            ss << "sfml_util::TileMap::ParseMapFile_TryCatchAndLog(" << MAP_FILE_PATH_STR
                << ") boost::property_tree functions threw an unknown exception.";

            M_HP_LOG_ERR(ss.str());
            throw std::runtime_error(ss.str());
        }
    }


    void TileMap::ParseMapFile_Implementation(const std::string & MAP_FILE_PATH_STR)
    {
        auto const XML_PROPERTY_TREE{ ParseMapFile_ReadXML(MAP_FILE_PATH_STR) };

        ParseMapFile_ParseMapSizes(XML_PROPERTY_TREE);

        SetupEmptyTexture();

        //calculate which tiles need to be drawn around the player position
        auto const PLAYER_POS_TILE_OFFSETS{ GetTileOffsetsOfPlayerPos() };

        //loop over all layers in the map file and parse them separately
        mapLayers_.clear();
        for(const boost::property_tree::ptree::value_type & PROPTREE_CHILD_PAIR :
            XML_PROPERTY_TREE.get_child("map"))
        {
            namespace ba = boost::algorithm;
            auto const NODENAME_LOWER{ ba::to_lower_copy(PROPTREE_CHILD_PAIR.first) };

            if (ParseMapFile_WillParseLayer(NODENAME_LOWER) == false)
            {
                continue;
            }
            else if (ba::contains(NODENAME_LOWER, ba::to_lower_copy(XML_NODE_NAME_OBJECT_LAYER_)))
            {
                ParseMapFile_ParseLayerCollisions(PROPTREE_CHILD_PAIR.second);
                continue;
            }
            else if (ba::contains(NODENAME_LOWER, ba::to_lower_copy(XML_NODE_NAME_TILESET_)))
            {
                ParseMapFile_ParseLayerTileset(PROPTREE_CHILD_PAIR.second);
                continue;
            }
            else
            {
                ParseMapFile_PraseLayerGeneric(PROPTREE_CHILD_PAIR, PLAYER_POS_TILE_OFFSETS);
            }
        }
    }


    const boost::property_tree::ptree TileMap::ParseMapFile_ReadXML(
        const std::string & MAP_FILE_PATH_STR) const
    {
        boost::property_tree::ptree xmlPropertyTree;

        try
        {
            boost::property_tree::read_xml(MAP_FILE_PATH_STR, xmlPropertyTree);
        }
        catch (const std::exception & E)
        {
            M_HP_LOG_ERR("sfml_util::TileMap::ParseMapFile_Implementation(\"" << MAP_FILE_PATH_STR
                << "\") boost::property_tree::read_xml() threw '" << E.what() << "' exception.");

            throw;
        }
        catch (...)
        {
            std::ostringstream ss;
            ss << "sfml_util::TileMap::ParseMapFile_Implementation(\"" << MAP_FILE_PATH_STR
                << "\") boost::property_tree::read_xml() threw an unknown exception.";

            M_HP_LOG_ERR(ss.str());
            throw std::runtime_error(ss.str());
        }

        return xmlPropertyTree;
    }


    void TileMap::ParseMapFile_ParseMapSizes(const boost::property_tree::ptree & XML_PROPERTY_TREE)
    {
        auto const XML_PROPERTY_TREE_MAPIMAGE{ XML_PROPERTY_TREE.get_child("map") };

        try
        {
            tileSizeWidth_ = XML_PROPERTY_TREE_MAPIMAGE.get<unsigned>("<xmlattr>.tilewidth");
            tileSizeHeight_ = XML_PROPERTY_TREE_MAPIMAGE.get<unsigned>("<xmlattr>.tileheight");
            mapTileCountX_ = XML_PROPERTY_TREE_MAPIMAGE.get<unsigned>("<xmlattr>.width");
            mapTileCountY_ = XML_PROPERTY_TREE_MAPIMAGE.get<unsigned>("<xmlattr>.height");
        }
        catch (const std::exception & E)
        {
            std::string name("");
            try
            {
                name = XML_PROPERTY_TREE_MAPIMAGE.get<std::string>("<xmlattr>.name");
            }
            catch (...)
            {
                name = "(no name error)";
            }

            M_HP_LOG_FAT("TileMap::ParseMapFile_ParseMapSizes() threw exception when parsing \""
                << name << "\".  what=\"" << E.what() << "\".");

            throw E;
        }
    }


    void TileMap::ParseMapFile_ParseLayerTileset(
        const boost::property_tree::ptree & TILESET_PROPTREE)
    {
        auto const IMAGE_PROPTREE{ TILESET_PROPTREE.get_child("image") };

        textures_.push_back( sf::Texture() );
        auto const TEXTURE_INDEX{ textures_.size() - 1 };

        tilesImageVec_.push_back( TileImage(
            TILESET_PROPTREE.get<std::string>("<xmlattr>.name"),
            IMAGE_PROPTREE.get<std::string>("<xmlattr>.source"),
            ID_t(TILESET_PROPTREE.get<ID_t::type>("<xmlattr>.firstgid")),
            Count_t(TILESET_PROPTREE.get<Count_t::type>("<xmlattr>.tilecount")),
            Column_t(TILESET_PROPTREE.get<Column_t::type>("<xmlattr>.columns")),
            Index_t(TEXTURE_INDEX)) );

        TileImage & tileImage{ tilesImageVec_[tilesImageVec_.size() - 1] };

        namespace bfs = boost::filesystem;
        tileImage.path_obj = bfs::system_complete(
            bfs::path(game::GameDataFile::Instance()->GetMediaPath("media-maps-tile-dir")) /
            bfs::path(tileImage.path_rel).leaf());

        sfml_util::LoadTexture(textures_[TEXTURE_INDEX], tileImage.path_obj.string());
    }


    bool TileMap::ParseMapFile_WillParseLayer(const std::string & NODENAME_LOWERCASE) const
    {
        namespace ba = boost::algorithm;

        auto const DOES_NODENAME_CONTAIN_TILELAYER{
            ba::contains(NODENAME_LOWERCASE, ba::to_lower_copy(XML_NODE_NAME_TILE_LAYER_)) };

        auto const DOES_NODENAME_CONTAIN_OBJECTLAYER{
            ba::contains(NODENAME_LOWERCASE, ba::to_lower_copy(XML_NODE_NAME_OBJECT_LAYER_)) };

        auto const DOES_NODENAME_CONTAIN_TILESET{
            ba::contains(NODENAME_LOWERCASE, ba::to_lower_copy(XML_NODE_NAME_TILESET_)) };

        return (
            DOES_NODENAME_CONTAIN_TILELAYER ||
            DOES_NODENAME_CONTAIN_OBJECTLAYER ||
            DOES_NODENAME_CONTAIN_TILESET );
    }


    void TileMap::ParseMapFile_ParseLayerCollisions(const boost::property_tree::ptree & OBJ_GROUP_PT)
    {
        namespace ba = boost::algorithm;

        for (const boost::property_tree::ptree::value_type & CHILD : OBJ_GROUP_PT)
        {
            if (ba::contains(ba::to_lower_copy(CHILD.first), ba::to_lower_copy(XML_ATTRIB_NAME_OBJECTS_)))
            {
                try
                {
                    //Not sure why these magic numbers 20.0f and 40.0f are here...zTn 2016-11-1
                    const float LEFT(CHILD.second.get<float>("<xmlattr>.x") - 20.0f);
                    const float TOP(CHILD.second.get<float>("<xmlattr>.y") - 40.0f);

                    const float WIDTH(CHILD.second.get<float>("<xmlattr>.width"));
                    const float HEIGHT(CHILD.second.get<float>("<xmlattr>.height"));

                    const sf::FloatRect RECT(LEFT, TOP, WIDTH, HEIGHT);

                    collisionRectsVec_.push_back(RECT);
                }
                catch (const std::exception & E)
                {
                    std::string name("(no name)");
                    try
                    {
                        name = CHILD.second.get<std::string>("<xmlattr>.name");
                    }
                    catch (...)
                    {
                        name = CHILD.first;
                    }

                    M_HP_LOG_FAT("TileMap::ParseMapFile_ParseLayerCollisions() threw "
                        << "std::exception when parsing \"" << name << "\".  what=\""
                        << E.what() << "\".");

                    throw E;
                }
            }
        }
    }


    void TileMap::ParseMapFile_PraseLayerGeneric(
        const boost::property_tree::ptree::value_type & PROPTREE_CHILD_PAIR,
        const TileOffsets & PLAYER_POS_TILE_OFFSETS)
    {
        std::stringstream ssAllData;
        ssAllData << PROPTREE_CHILD_PAIR.second.get_child("data").data();

        mapLayers_.push_back( MapLayer() );
        MapLayer & mapLayer{ mapLayers_[mapLayers_.size() - 1] };
        ParseMapFile_ParseGenericTileLayer(mapLayer.mapid_vec, ssAllData);
        EstablishMapSubsection(mapLayer, PLAYER_POS_TILE_OFFSETS);
    }


    void TileMap::ParseMapFile_ParseGenericTileLayer(
        IDVec_t & mapIDs,
        std::stringstream & tileLayerDataSS)
    {
        //found by experiment to be a reliable upper bound for typical maps
        mapIDs.reserve(20'000);

        std::string nextLine("");
        while (tileLayerDataSS)
        {
            std::getline(tileLayerDataSS, nextLine, '\n');

            if (nextLine.empty())
            {
                continue;
            }

            std::stringstream ssLine;
            ssLine << nextLine;
            std::string nextValStr("");

            //loop through each line reading numbers between the commas
            while (ssLine)
            {
                std::getline(ssLine, nextValStr, ',');

                if (nextValStr.empty())
                {
                    continue;
                }

                try
                {
                    //using boost::lexical_cast instead of std::stoi because it tested faster
                    mapIDs.push_back( ID_t(boost::lexical_cast<ID_t::type>(nextValStr)) );
                }
                catch (const std::exception & E)
                {
                    M_HP_LOG("sfml_util::TileMap::ParseMapFile_ParseGenericTileLayer("
                        << nextLine << ") " << "boost::lexical_cast<"
                        << boost::typeindex::type_id<ID_t::type>().pretty_name() << "("
                        << "\"" << nextValStr << "\") threw '" << E.what() << "' exception.");

                    throw;
                }
                catch (...)
                {
                    std::ostringstream ss;
                    ss << "sfml_util::TileMap::ParseMapFile_ParseGenericTileLayer("
                       << nextLine << ") " << "boost::lexical_cast<"
                       << boost::typeindex::type_id<ID_t::type>().pretty_name() << "("
                       << "\"" << nextValStr << "\") threw unknown exception.";

                    M_HP_LOG(ss.str());
                    throw std::runtime_error(ss.str());
                }
            }
        }
    }


    void TileMap::EstablishMapSubsection(MapLayer & mapLayer, const TileOffsets & TILE_OFFSETS)
    {
        //resize the vertex array to fit the level size
        auto const TILE_WIDTH{  TILE_OFFSETS.end_x - TILE_OFFSETS.begin_x };
        auto const TILE_HEIGHT{ TILE_OFFSETS.end_y - TILE_OFFSETS.begin_y };
        mapLayer.vert_array.setPrimitiveType(sf::Quads);
        const std::size_t VERTEXES_PER_QUAD{ 4 };

        mapLayer.vert_array.resize(
            (TILE_WIDTH.As<std::size_t>() * TILE_HEIGHT.As<std::size_t>()) * VERTEXES_PER_QUAD);

        mapLayer.tilesimage_vec.reserve(TILE_WIDTH.As<std::size_t>() * TILE_HEIGHT.As<std::size_t>());

        //populate the vertex array with one quad per tile
        unsigned vertIndex(0);
        for (Count_t::type x(TILE_OFFSETS.begin_x.Get()); x < TILE_OFFSETS.end_x.Get(); ++x)
        {
            for (Count_t::type y(TILE_OFFSETS.begin_y.Get()); y < TILE_OFFSETS.end_y.Get(); ++y)
            {
                //get the current tile number
                auto const TILE_INDEX{ x + (y * mapTileCountX_) };

                auto const TILE_NUM_ORIG{
                    mapLayer.mapid_vec[static_cast<std::size_t>(TILE_INDEX)] };

                //This -1 comes from the Tiled app that starts tile ids at 1 instead of 0.
                auto const TILE_NUM_ID_ADJ{
                    (TILE_NUM_ORIG == 0_id) ? 0_id : (TILE_NUM_ORIG - 1_id) };

                //get the texture/image this tile can be found in
                const TileImage & TILE_IMAGE{ GetTileImageFromId(TILE_NUM_ID_ADJ) };
                mapLayer.tilesimage_vec.push_back(TILE_IMAGE);

                //adjust the tile number to start at one
                auto const TILE_ID{ (TILE_NUM_ID_ADJ - TILE_IMAGE.first_id) + 1_id };

                //get a pointer to the current tile's quad
                sf::Vertex * quadPtr{
                    & mapLayer.vert_array[static_cast<std::size_t>((vertIndex++) * 4)] };

                //define its 4 corners on screen
                auto const POS_WIDTH{
                    static_cast<float>((x - TILE_OFFSETS.begin_x.Get()) * tileSizeWidth_) };

                auto const POS_WIDTH_PLUS_ONE_TILE{
                    POS_WIDTH + static_cast<float>(tileSizeWidth_) };

                auto const POS_HEIGHT{
                    static_cast<float>((y - TILE_OFFSETS.begin_y.Get()) * tileSizeHeight_) };

                auto const POS_HEIGHT_PLUS_ONE_TILE{
                    POS_HEIGHT + static_cast<float>(tileSizeHeight_) };

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
                    textures_[TILE_IMAGE.texture_index.Get()].getSize().x / tileSizeWidth_ };

                auto const TILE_COUNT_U{
                    static_cast<float>(TILE_ID.As<unsigned>() % TEXTURE_TILE_COUNT_HORIZ) };

                auto const TILE_COUNT_V{
                    static_cast<float>(TILE_ID.As<unsigned>() / TEXTURE_TILE_COUNT_HORIZ) };

                auto const TEXTCOORD_WIDTH{
                    TILE_COUNT_U * static_cast<float>(tileSizeWidth_) };

                auto const TEXTCOORD_WIDTH_PLUS_ONE_TILE{
                    TEXTCOORD_WIDTH + static_cast<float>(tileSizeWidth_) };

                auto const TEXTCOORD_HEIGHT{
                    TILE_COUNT_V * static_cast<float>(tileSizeHeight_) };

                auto const TEXTCOORD_HEIGHT_PLUS_ONE_TILE{
                    TEXTCOORD_HEIGHT + static_cast<float>(tileSizeHeight_) };

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


    const TileOffsets TileMap::GetTileOffsetsOfPlayerPos() const
    {
        return GetTileOffsetsFromMapPos(playerPosMapV_);
    }


    const TileOffsets TileMap::GetTileOffsetsFromMapPos(const sf::Vector2f & MAP_POS_V) const
    {
        TileOffsets tileOffsets;

        auto const PLAYER_POSX_IN_TILES{ static_cast<unsigned>(MAP_POS_V.x) / tileSizeWidth_ };
        auto const HALF_WIN_TILES_X{ (WIN_SIZE_V_.x / tileSizeWidth_) / 2 };

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
            Count_t((WIN_SIZE_V_.x / tileSizeWidth_) + EXTRA_OFFSCREEN_TILE_COUNT_.As<unsigned>()) };

        if (tileOffsets.end_x < TILE_WITH_OFFSET_COUNT_X)
        {
            tileOffsets.end_x = TILE_WITH_OFFSET_COUNT_X;
        }

        auto const PLAYER_POSY_IN_TILES{ static_cast<unsigned>(MAP_POS_V.y) / tileSizeHeight_ };
        auto const HALF_WIN_TILES_Y{ (WIN_SIZE_V_.y / tileSizeHeight_) / 2 };

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
            Count_t((WIN_SIZE_V_.y / tileSizeHeight_) + EXTRA_OFFSCREEN_TILE_COUNT_.As<unsigned>()) };

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

        if ((tileOffsets.end_x.As<unsigned>() + EXTRA_OFFSCREEN_TILE_COUNT_.As<unsigned>()) < mapTileCountX_)
        {
            tileOffsets.end_x += EXTRA_OFFSCREEN_TILE_COUNT_;
        }
        else
        {
            tileOffsets.end_x = Count_t(mapTileCountX_);
        }

        if ((tileOffsets.end_y.As<unsigned>() + EXTRA_OFFSCREEN_TILE_COUNT_.As<unsigned>()) < mapTileCountY_)
        {
            tileOffsets.end_y += EXTRA_OFFSCREEN_TILE_COUNT_;
        }
        else
        {
            tileOffsets.end_y = Count_t(mapTileCountY_);
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


    const sf::Vector2f TileMap::GetPosScreen(const TileOffsets & TILE_OFFSETS) const
    {
        //Convert from off-screen coords to on-screen coords.
        auto const OFFSET_DIFFX{
            static_cast<float>(TILE_OFFSETS.end_x.As<int>() - TILE_OFFSETS.begin_x.As<int>()) };

        auto const OFFSET_DIFFY{
            static_cast<float>(TILE_OFFSETS.end_y.As<int>() - TILE_OFFSETS.begin_y.As<int>()) };

        auto const LEFT{ static_cast<float>(
            WIN_POS_V_.x + ((OFFSET_DIFFX * static_cast<float>(tileSizeWidth_)) * 0.5f)) };

        auto const TOP{ static_cast<float>(
            WIN_POS_V_.y + ((OFFSET_DIFFY * static_cast<float>(tileSizeHeight_)) * 0.5f)) };

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


    const TileImage & TileMap::GetTileImageFromId(const ID_t & ID) const
    {
        for (auto const & TILE_IMAGE : tilesImageVec_)
        {
            if (TILE_IMAGE.OwnsId(ID))
            {
                return TILE_IMAGE;
            }
        }

        std::ostringstream ss;
        ss << "sfml_util::TileMap::GetTileImageFromId(id=" << ID
            << ") failed to find the owning TileImage.";

        throw std::runtime_error(ss.str());
    }


    void TileMap::SetupEmptyTexture()
    {
        M_ASSERT_OR_LOGANDTHROW_SS(emptyRendText_.create(tileSizeWidth_, tileSizeHeight_),
            "TileMap::SetupEmptyTexture() sf::RenderTexture::create(" << tileSizeWidth_ << "x"
            << tileSizeHeight_ << ") failed.");

        emptyRendText_.clear(sf::Color::Transparent);
        emptyRendText_.display();

        textures_.resize(TRANSPARENT_TEXTURE_INDEX_.Get() + 1);
        textures_[TRANSPARENT_TEXTURE_INDEX_.Get()] = emptyRendText_.getTexture();

        tilesImageVec_.push_back( TileImage(
            TILE_IMAGE_NAME_EMPTY_,
            "",//the empty/transparent texture has no file
            0_id,
            1_count,
            1_column,
            TRANSPARENT_TEXTURE_INDEX_) );
    }


    void TileMap::AdjustOffscreenRectsToPreventDrift(const TileOffsets & CURRENT_TILE_OFFSETS)
    {
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
    }

}
}
