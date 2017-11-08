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
#ifndef SFMLUTIL_TILEMAP_HPP_INCLUDED
#define SFMLUTIL_TILEMAP_HPP_INCLUDED
//
// tilemap.hpp
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/collision-quad-tree.hpp"

#include "misc/types.hpp"

//suppress warnings that are safe to ignore in boost
#if !defined(WIN32) && !defined(_WIN32) && !defined(__WIN32__) && !defined(__WINDOWS__)
#pragma GCC diagnostic ignored "-Wundef"
#pragma GCC diagnostic ignored "-Wswitch-enum"
#endif

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/filesystem/path.hpp>

#if !defined(WIN32) && !defined(_WIN32) && !defined(__WIN32__) && !defined(__WINDOWS__)
#pragma GCC diagnostic warning "-Wundef"
#pragma GCC diagnostic warning "-Wswitch-enum"
#endif

#include <string>
#include <vector>
#include <list>
#include <tuple>


using namespace heroespath;


namespace sfml_util
{

    struct TileOffsets
    {
        Count_t begin_x = 0_count;
        Count_t end_x = 0_count;
        Count_t begin_y = 0_count;
        Count_t end_y = 0_count;
    };

    inline bool operator==(const TileOffsets & L, const TileOffsets & R)
    {
        return std::tie(L.begin_x, L.begin_y, L.end_x, L.end_y)
                ==
               std::tie(R.begin_x, R.begin_y, R.end_x, R.end_y);
    }

    inline bool operator!=(const TileOffsets & L, const TileOffsets & R)
    {
        return ! (L == R);
    }



    //Responsible for wrapping all the details about a single tile image.
    struct TileImage
    {
        explicit TileImage(
            const std::string & NAME          = "",
            const std::string & RELATIVE_PATH = "",
            const ID_t FIRST_ID               = 0_id,
            const Count_t TILE_COUNT          = 1_count,
            const Column_t COLUMN             = 1_column,
            const Index_t TEXTURE_INDEX       = 0_index);

        inline bool OwnsId(const ID_t ID) const
        {
            return (ID >= first_id) &&
                   (ID.AsUInt() < (first_id.AsUInt() + tile_count.AsUInt()));
        }

        std::string name;
        std::string path_rel; //path string relative to the .tmx map path
        ID_t first_id;
        Count_t tile_count;
        Column_t column;
        Index_t texture_index;
        boost::filesystem::path path_obj;
    };

    using TileImageVec_t = std::vector<TileImage>;



    //Responsible for wrapping all the details about a layer of the map.
    struct MapLayer
    {
        sf::VertexArray vert_array;
        TileImageVec_t tilesimage_vec;
        IDVec_t mapid_vec;
    };

    using MapLayerVec_t = std::vector<MapLayer>;



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
            const sf::Vector2f & PLAYER_POS_V,
            const sf::Color & TRANSPARENT_MASK_COLOR = DEFAULT_TRANSPARENT_MASK_);

        void ApplyColorMasksToHandleTransparency();

        bool MoveUp(const float ADJUSTMENT);
        bool MoveDown(const float ADJUSTMENT);
        bool MoveLeft(const float ADJUSTMENT);
        bool MoveRight(const float ADJUSTMENT);

        void Draw(sf::RenderTarget &, const sf::RenderStates &);

    private:
        void SetupMapSprite();

        void ParseMapFile(const std::string & MAP_FILE_PATH_STR);
        void ParseMapFile_TryCatchAndLog(const std::string & MAP_FILE_PATH_STR);
        void ParseMapFile_Implementation(const std::string & MAP_FILE_PATH_STR);

        const boost::property_tree::ptree ParseMapFile_ReadXML(
            const std::string & MAP_FILE_PATH_STR) const;

        void ParseMapFile_ParseMapSizes(const boost::property_tree::ptree &);

        void ParseMapFile_ParseLayerTileset(const boost::property_tree::ptree &);

        bool ParseMapFile_WillParseLayer(const std::string & NODENAME_LOWERCASE) const;

        void ParseMapFile_ParseLayerCollisions(const boost::property_tree::ptree &);

        void ParseMapFile_PraseLayerGeneric(
            const boost::property_tree::ptree::value_type &,
            const TileOffsets & PLAYER_POS_TILE_OFFSETS);

        void ParseMapFile_ParseGenericTileLayer(IDVec_t &, std::stringstream &);

        void EstablishMapSubsection(
            MapLayer & mapLayerSPtr,
            const TileOffsets & TILE_OFFSETS);

        const TileOffsets GetTileOffsets(const sf::Vector2f &) const;
        const TileOffsets GetTileOffsetsPlayerPos() const;
        const sf::Vector2f GetPlayerPosActual() const;
        const sf::Vector2f GetScreenPos(const TileOffsets &) const;
        const sf::Vector2f GetScreenPosPlayer() const;

        bool IsPointWithinCollision(const sf::Vector2f &) const;

        const TileImage & GetTileImageFromId(const ID_t) const;

        void SetupEmptyTexture();

        void DrawPlayer(sf::RenderTarget & target);

        void AdjustOffscreenRectsToPreventDrift(const TileOffsets &);

    public:
        //This is how close the player position can get to
        //the edge of the map before being forced to stop.
        static const float BORDER_PAD_;

        //the name of the fully transparent texture used to draw 'nothing' tiles
        static const std::string TILE_IMAGE_NAME_EMPTY_;

        //these are the names of XML nodes used in parsing the .tmx map files
        static const std::string XML_NODE_NAME_TILE_LAYER_;
        static const std::string XML_NODE_NAME_OBJECT_LAYER_;
        static const std::string XML_NODE_NAME_TILESET_;

        //these are the sub-names of XML nodes used in parsing the .tmx map files
        static const std::string XML_ATTRIB_NAME_GROUND_;
        static const std::string XML_ATTRIB_NAME_OBJECTS_;
        static const std::string XML_ATTRIB_NAME_SHADOWS_;
        static const std::string XML_ATTRIB_NAME_COLLISION_;

        //how many tiles to draw offscreen that are outside the visible map area
        static const Count_t EXTRA_OFFSCREEN_TILE_COUNT_;

        //The tileset I found online uses this color as a background,
        //so it needs to be changed to transparen.t
        static const sf::Color DEFAULT_TRANSPARENT_MASK_;

        //the tileset I found online uses these magenta colors as shades for shadows
        static const sf::Color SHADOW_MASK1_;
        static const sf::Color SHADOW_MASK2_;
        static const sf::Color SHADOW_MASK2B_;
        static const sf::Color SHADOW_MASK3_;

        //these are the shades of semi-transparent black I chose for shadows
        static const sf::Color SHADOW_COLOR1_;
        static const sf::Color SHADOW_COLOR2_;
        static const sf::Color SHADOW_COLOR3_;

        static const Index_t TRANSPARENT_TEXTURE_INDEX_;

    private:
        unsigned           tileSizeWidth_;
        unsigned           tileSizeHeight_;
        unsigned           mapTileCountX_;
        unsigned           mapTileCountY_;
        TileOffsets        prevTileOffsets_;
        const sf::Vector2f WIN_POS_V_;
        const sf::Vector2u WIN_SIZE_V_;
        sf::Vector2f       playerPosV_; //in offscreen coordinates
        sf::Vector2f       playerPosOffsetV_;//in screeen coordinates
        sf::RenderStates   renderStates_;
        sf::FloatRect      offScreenRect_;
        sf::Sprite         mapSprite_;
        sf::RenderTexture  offScreenTexture_;
        sf::RenderTexture  emptyRendText_;
        FloatRectVec_t     collisionsVec_;
        MapLayerVec_t      mapLayers_;
        TileImageVec_t     tilesImageVec_;
        const sf::Color    TRANSPARENT_MASK_;
        QuadTree           collisionTree_;
        sfml_util::TextureVec_t textures_;
    };


    using TileMapUPtr_t = std::unique_ptr<TileMap>;

}

#endif //SFMLUTIL_TILEMAP_HPP_INCLUDED
