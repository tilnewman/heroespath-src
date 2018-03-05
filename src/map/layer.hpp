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
