// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_MAP_MAP_TILE_DRAW_HPP_INCLUDED
#define HEROESPATH_MAP_MAP_TILE_DRAW_HPP_INCLUDED
//
// map-tile-draw.hpp
//
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>

#include <cstddef> //for std::size_t
#include <vector>

namespace heroespath
{
namespace map
{

    // Represents the draw of on tile to one layer of one map from one texture, often overlapping
    // other tiles.  Note the less-than operator below that is used to sort these objects in a very
    // specific order, mostly just to sort by layers so that overlapping TileDraws work/look
    // correct.
    struct TileDraw
    {
        TileDraw(
            const std::size_t LAYER_INDEX,
            const sf::Vector2i & TILE_INDEX_V,
            const std::size_t TEXTURE_INDEX,
            const sf::IntRect & TEXTURE_RECT)
            : layer_index(LAYER_INDEX)
            , tile_index_v(TILE_INDEX_V)
            , texture_index(TEXTURE_INDEX)
            , texture_rect(TEXTURE_RECT)
        {}

        TileDraw(const TileDraw &) = default;
        TileDraw(TileDraw &&) = default;
        TileDraw & operator=(const TileDraw &) = default;
        TileDraw & operator=(TileDraw &&) = default;

        std::size_t layer_index;
        sf::Vector2i tile_index_v;
        std::size_t texture_index;
        sf::IntRect texture_rect;
    };

    // this is used to sort TileDraws and the order is critical to the operation of
    // map-optimizer.hpp and map-display.hpp
    inline bool operator<(const TileDraw & LHS, const TileDraw & RHS)
    {
        return std::tie(LHS.layer_index, LHS.texture_index, LHS.texture_rect, LHS.tile_index_v)
            < std::tie(RHS.layer_index, RHS.texture_index, RHS.texture_rect, RHS.tile_index_v);
    }

    inline bool operator==(const TileDraw & LHS, const TileDraw & RHS)
    {
        return std::tie(LHS.tile_index_v, LHS.texture_rect, LHS.layer_index, LHS.texture_index)
            == std::tie(RHS.tile_index_v, RHS.texture_rect, RHS.layer_index, RHS.texture_index);
    }

    inline bool operator!=(const TileDraw & LHS, const TileDraw & RHS) { return !(LHS == RHS); }

    using TileDrawVec_t = std::vector<TileDraw>;

} // namespace map
} // namespace heroespath

#endif // HEROESPATH_MAP_MAP_OPTIMIZER_HPP_INCLUDED
