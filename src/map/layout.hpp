// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_LAYOUT_HPP_INCLUDED
#define HEROESPATH_LAYOUT_HPP_INCLUDED
//
// layout.hpp
//
#include "gui/cached-texture.hpp"
#include "map/layer.hpp"
#include "map/tiles-panel.hpp"

#include <string>
#include <tuple>
#include <vector>

namespace heroespath
{
namespace map
{

    // Responsible for wrapping all image and state of a single loaded map.
    struct Layout
    {
        Layout()
            : tile_size_v(0, 0)
            , tile_count_v(0, 0)
            , layer_vec()
            , tiles_panel_vec()
            , texture_vec()
        {}

        void Reset()
        {
            tile_size_v.x = 0;
            tile_size_v.y = 0;
            tile_count_v.x = 0;
            tile_count_v.y = 0;
            layer_vec.clear();
            tiles_panel_vec.clear();
            texture_vec.clear();
        }

        int LayerTileNumber(const Layer & LAYER, const sf::Vector2i & TILE_INDEXES) const
        {
            const auto TILE_INDEX { (TILE_INDEXES.x + (TILE_INDEXES.y * tile_count_v.x)) };
            return LAYER.mapid_vec.at(static_cast<std::size_t>(TILE_INDEX));
        }

        int LayerTileNumber(const std::size_t LAYER_INDEX, const sf::Vector2i & TILE_INDEXES) const
        {
            return LayerTileNumber(layer_vec.at(LAYER_INDEX), TILE_INDEXES);
        }

        // returns the first layer tile number (int) and texture index (std::size_t), returns an
        // out-of-range texture index if not found
        const std::tuple<int, std::size_t> FindTileTextureInfo(const int LAYER_TILE_NUMBER) const
        {
            for (const auto & TILES_PANEL : tiles_panel_vec)
            {
                if (TILES_PANEL.OwnsId(LAYER_TILE_NUMBER))
                {
                    return std::make_tuple(TILES_PANEL.first_id, TILES_PANEL.texture_index);
                }
            }

            return std::make_tuple(-1, texture_vec.size());
        }

        sf::Vector2i tile_size_v;
        sf::Vector2i tile_count_v;
        LayerVec_t layer_vec;
        TilesPanelVec_t tiles_panel_vec;
        std::vector<gui::CachedTexture> texture_vec;
    };

} // namespace map
} // namespace heroespath

#endif // HEROESPATH_LAYOUT_HPP_INCLUDED
