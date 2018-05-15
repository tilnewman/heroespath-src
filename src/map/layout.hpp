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
#include "map/layer.hpp"
#include "map/tiles-panel.hpp"
#include "sfml-util/sfml-graphics.hpp"

#include <string>

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
            , empty_texture()
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

        const std::string EmptyTilesPanelName() const { return "empty"; }

        sf::Vector2i tile_size_v;
        sf::Vector2i tile_count_v;
        LayerVec_t layer_vec;
        TilesPanelVec_t tiles_panel_vec;
        sfml_util::TextureVec_t texture_vec;
        sf::RenderTexture empty_texture;
    };
} // namespace map
} // namespace heroespath

#endif // HEROESPATH_LAYOUT_HPP_INCLUDED
