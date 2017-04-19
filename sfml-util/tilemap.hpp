#ifndef APPBASE_SFMLUTIL_TILEMAP_INCLUDED
#define APPBASE_SFMLUTIL_TILEMAP_INCLUDED
//
// tilemap.hpp
//
#include "sfml-util/sfml-graphics.hpp"

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
#include <map>
#include <tuple>


namespace sfml_util
{
namespace map
{

    //helpful struct used by TileMap
    struct TileOffsets
    {
        TileOffsets();

        unsigned begin_x;
        unsigned end_x;
        unsigned begin_y;
        unsigned end_y;
    };

    bool operator==(const TileOffsets & L, const TileOffsets & R);

    inline bool operator!=(const TileOffsets & L, const TileOffsets & R);



    //holds all the info regarding an image file used by TileMap
    struct TilesImage
    {
        TilesImage(const std::string & NAME          = "",
                   const std::string & RELATIVE_PATH = "",
                   const std::size_t   FIRST_ID      = 0,
                   const std::size_t   TILE_COUNT    = 1,
                   const std::size_t   COLUMN_COUNT  = 1,
                   const TextureSPtr_t TEXTURE_SPTR  = TextureSPtr_t());

        inline bool OwnsId(const std::size_t ID) { return ((ID >= first_id) && (ID < (first_id + tile_count))); }

        std::string   name;
        std::string   path_rel; //path string relative to the .tmx map path
        boost::filesystem::path     path_obj;
        std::size_t   first_id;
        std::size_t   tile_count;
        std::size_t   column_count;
        TextureSPtr_t texture_sptr;
    };

    bool operator==(const TilesImage & L, const TilesImage & R);

    bool operator!=(const TilesImage & L, const TilesImage & R);

    using TilesImageSPtr_t = std::shared_ptr<TilesImage>;
    using TilesImageSVec_t = std::vector<TilesImageSPtr_t>;


    //custom types
    using MapVal_t = unsigned;
    using MapValVec_t = std::vector<MapVal_t>;


    //encapsulates all the info about a layer of the tilemap
    struct MapLayer
    {
        MapLayer()
        :
            vert_array     (),
            tilesimage_svec(),
            mapval_vec     ()
        {}

        sf::VertexArray   vert_array;
        TilesImageSVec_t  tilesimage_svec;
        MapValVec_t       mapval_vec;
    };

    using MapLayerSPtr_t = std::shared_ptr<MapLayer>;
    using MapLayerSVec_t = std::vector<MapLayerSPtr_t>;


    //collision detection quad
    struct Quad
    {
        FloatRectVec_t coll_rects_vec;
        FloatRectVec_t quad_rects_vec;
        std::vector<Quad> child_quads_vec;
    };


    //collision detection quad-tree
    class QuadTree
    {
        //prevent copy construction
        QuadTree(const QuadTree &);

        //prevent copy assignment
        QuadTree & operator=(const QuadTree &);

    public:
        QuadTree();
        virtual ~QuadTree();

        void Setup(const float            MAP_WIDTH,
                   const float            MAP_HEIGHT,
                   const FloatRectVec_t & COLL_RECTS_VEC);

        bool IsPointWithinCollisionRect(const float POS_LEFT, const float POS_TOP) const;

    private:
        void PopulateQuadAndRecurse(Quad &                 quad,
                                    const sf::FloatRect &  RECT,
                                    const FloatRectVec_t & COLL_RECTS_VEC);

        bool IsPointWithinCollisionRect_Impl(const Quad & QUAD,
                                             const float  POS_LEFT,
                                             const float  POS_TOP) const;

        static const float MIN_QUAD_SIZE_;
        Quad headQuad_;
    };


    //Encapsulates a tiled map, along with the player's position.
    class TileMap
    {
        //prevent copy construction
        TileMap(const TileMap &);

        //prevent copy assignment
        TileMap & operator=(const TileMap &);

    public:
        TileMap(const std::string &  MAP_PATH_STR,
                const sf::Vector2f & WIN_POS_V,
                const sf::Vector2u & WIN_SIZE_V,
                const sf::Vector2f & PLAYER_POS_V,
                const sf::Color &    TRANSPARENT_MASK_COLOR = DEFAULT_TRANSPARENT_MASK_);

        virtual ~TileMap();

        void ApplyColorMasksToHandleTransparency();

        bool MoveUp(const float ADJUSTMENT);
        bool MoveDown(const float ADJUSTMENT);
        bool MoveLeft(const float ADJUSTMENT);
        bool MoveRight(const float ADJUSTMENT);

        void DrawMap(sf::RenderTarget & target);

    private:
        void SetupMapSprite();

        void ParseMapFile(const std::string & MAP_PATH_STR);

        void ParseTilesetLayer(const std::string &                 MAP_PATH_STR,
                               const boost::property_tree::ptree & TILESET_PT);

        void ParseTileLayer(MapValVec_t & map, std::stringstream & ss);

        void ParseCollisionsLayer(const boost::property_tree::ptree &);

        void EstablishMapSubsection(MapLayerSPtr_t mapLayerSPtr, const TileOffsets & TILE_OFFSETS);

        const TileOffsets GetTileOffsets(const sf::Vector2f &) const;
        const TileOffsets GetTileOffsetsPlayerPos() const;
        const sf::Vector2f GetPlayerPosActual() const;
        const sf::Vector2f GetScreenPos(const TileOffsets &) const;
        const sf::Vector2f GetScreenPosPlayer() const;

        bool IsPointWithinCollision(const sf::Vector2f &) const;

        const TilesImageSPtr_t GetTileImageFromId(const MapVal_t ID) const;

        void SetupEmptyTexture();

        void DrawPlayer(sf::RenderTarget & target);

    public:
        //this is how close the player position can get to the edge of the map before being forced to stop.
        static const float BORDER_PAD_;

        //the name of the fully transparent texture used to draw 'nothing' tiles
        static const std::string TILES_IMAGE_NAME_EMPTY_;

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
        static const unsigned EXTRA_OFFSCREEN_TILE_COUNT_;

        //the tileset I found online uses this color as a background, so it needs to be changed to transparent
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
        sf::Texture        tileTexture_;
        sf::RenderStates   renderStates_;
        sf::FloatRect      offScreenRect_;
        sf::Sprite         mapSprite_;
        sf::RenderTexture  offScreenTexture_;
        sf::RenderTexture  emptyRendText_;
        FloatRectVec_t     collisionsVec_;
        MapLayerSVec_t     mapLayerSVec_;
        TilesImageSVec_t   tilesImageSVec_;
        const sf::Color    TRANSPARENT_MASK_;
        QuadTree           collisionTree_;
    };

}
}
#endif //APPBASE_SFMLUTIL_TILEMAP_INCLUDED
