// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_MAP_MAP_TILE_OPTIMIZER_HPP_INCLUDED
#define HEROESPATH_MAP_MAP_TILE_OPTIMIZER_HPP_INCLUDED
//
// map-tile-optimizer.hpp
//
#include "gui/cached-texture.hpp"
#include "map/map-tile-draw.hpp"
#include "misc/vector-map.hpp"
#include "sfutil/vector-and-rect.hpp"

#include <string>
#include <vector>

namespace heroespath
{
namespace map
{

    struct Layout;
    struct Layer;

    // Rearranges the layers and textures of a map made with the "Tiled" app so that it takes up
    // less memory on the video card and so that it draws much faster.  The map is drawn in two sets
    // called "below" and "above", which explains why many of the functions below are for "All" or
    // one "Set".
    struct MapTileOptimizer
    {
        // wraps all the info that MapTileOptimizer needs
        struct Packet
        {
            Packet(
                const Layout & LAYOUT,
                const sf::Vector2i & TILE_COUNT_V,
                const sf::Vector2i & TILE_SIZE_V,
                std::vector<sf::Texture> & textureVec,
                TileDrawVec_t & tileDrawsBelow,
                TileDrawVec_t & tileDrawsAbove)
                : layout(LAYOUT)
                , tile_count_v(TILE_COUNT_V)
                , tile_size_v(TILE_SIZE_V)
                , textures(textureVec)
                , tile_draws_below(tileDrawsBelow)
                , tile_draws_above(tileDrawsAbove)
            {}

            Packet(const Packet &) = default;
            Packet(Packet &&) = default;
            Packet & operator=(const Packet &) = default;
            Packet & operator=(Packet &&) = default;

            const sf::Texture & MapTileTexture(const std::size_t INDEX) const;
            std::size_t MapTileTextureCount() const;

            const Layout & layout;
            const sf::Vector2i tile_count_v;
            const sf::Vector2i tile_size_v;
            std::vector<sf::Texture> & textures;
            TileDrawVec_t & tile_draws_below;
            TileDrawVec_t & tile_draws_above;
        };

        // these are just two helper types used by OptimizeLayers()
        struct LayerInfo
        {
            std::size_t layer_index;
            misc::VectorMap<std::size_t, int> texture_count_map;
            misc::VectorMap<sf::Vector2i, std::vector<std::size_t>> pos_indexes_map;
        };
        using LayerInfoMap_t = misc::VectorMap<std::size_t, LayerInfo>;

        static void Optimize(Packet & packet);

    private:
        static const std::size_t TEXTURE_DIMMENSION_MAX_;

        static void PopulateTileDraws(Packet & packet);

        static void PopulateTileDrawsForLayer(Packet & packet, const Layer & LAYER);

        static const TileDraw MakeTileDraw(
            const Packet & PACKET,
            const Layer & LAYER,
            const std::size_t TEXTURE_INDEX,
            const sf::Vector2i & TILE_INDEXES,
            const int TEXTURE_TILE_NUMBER);

        static void OptimizeTextures(Packet & packet);

        static void OptimizeTexturesSet(
            const std::size_t MAX_TILES_PER_NEW_TEXTURE,
            const sf::Vector2i & TILE_SIZE_V,
            const std::vector<gui::CachedTexture> & TEXTURES_ORIG,
            std::vector<sf::Texture> & texturesNew,
            TileDrawVec_t & tileDraws);

        // returns the number of tile-index-vecs from tileDrawIndexVecsToPutInNewImage were actually
        // put in the new image
        static std::size_t MakeOptimizedTexture(
            const std::size_t MAX_TILES_PER_NEW_TEXTURE,
            const sf::Vector2i & TILE_SIZE_V,
            const std::vector<gui::CachedTexture> & TEXTURES_ORIG,
            TileDrawVec_t & tileDraws,
            std::vector<sf::Texture> & texturesNew,
            std::vector<std::vector<std::size_t>> & tileDrawIndexVecsToPutInNewImage);

        static void OptimizeLayers(Packet & packet);

        static void OptimizeLayersSet(TileDrawVec_t & tileDraws);

        static void MinimizeLayerOverlap(LayerInfoMap_t & layerMap, TileDrawVec_t & tileDraws);

        static void
            EliminateSkippedLayerIndexes(LayerInfoMap_t & layerMap, TileDrawVec_t & tileDraws);

        // returns true if any tile positions were descended
        static bool DescendNonOverlappingTiles(
            LayerInfo & layerUnder, LayerInfo & layerOver, TileDrawVec_t & tileDraws);

        static void LogSummary(const Packet & PACKET);

        static void LogSummaryAll(const Packet & PACKET);

        static void LogSummarySet(
            const Packet & PACKET, const std::string & NAME, const TileDrawVec_t & DRAWS);

        static std::size_t PixelsInTexture(const sf::Texture & TEXTURE);

        static std::size_t TilesInTexture(
            const sf::Vector2i & TILE_SIZE_V, const std::size_t WIDTH, const std::size_t HEIGHT);

        static std::size_t TilesInTexture(
            const sf::Vector2i & TILE_SIZE_V, const std::size_t SQUARE_PIXEL_DIMMENSION);

        static std::size_t
            TilesInTexture(const sf::Vector2i & TILE_SIZE_V, const sf::Texture & TEXTURE);
    };

} // namespace map
} // namespace heroespath

#endif // HEROESPATH_MAP_MAP_OPTIMIZER_HPP_INCLUDED
