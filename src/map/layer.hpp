// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_LAYER_HPP_INCLUDED
#define HEROESPATH_LAYER_HPP_INCLUDED
//
// layer.hpp
//
#include "map/layer-type-enum.hpp"
#include "map/tiles-panel.hpp"
#include "misc/types.hpp"
#include "sfml-util/sfml-graphics.hpp"

#include <vector>

namespace heroespath
{
namespace map
{

    // Responsible for wrapping all data about a single map layer.
    struct Layer
    {
        LayerType::Enum type;

        // only stores verts for drawn map tiles
        sf::VertexArray vert_array;

        // copies of TileImages that are used by vert_array
        TilesPanelForLayersVec_t tiles_panel_vec;

        // the map IDs that make up this panel
        std::vector<int> mapid_vec;

        void ResetForReDraw()
        {
            vert_array.clear();
            tiles_panel_vec.clear();
        }
    };

    using LayerVec_t = std::vector<Layer>;
} // namespace map
} // namespace heroespath

#endif // HEROESPATH_LAYER_HPP_INCLUDED
