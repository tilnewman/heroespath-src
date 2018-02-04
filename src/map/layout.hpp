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
#ifndef HEROESPATH_LAYOUT_HPP_INCLUDED
#define HEROESPATH_LAYOUT_HPP_INCLUDED
//
// layout.hpp
//
#include "sfml-util/sfml-graphics.hpp"
#include "map/layer.hpp"
#include "map/tiles-panel.hpp"

#include <string>


namespace heroespath
{
namespace map
{

    //Responsible for wrapping all image and state of a single loaded map.
    class Layout
    {
        Layout(const Layout &) = delete;
        Layout & operator=(const Layout &) = delete;
    
    public:
        Layout()
        :
            tile_size_horiz(0),
            tile_size_vert(0),
            tile_count_horiz(0),
            tile_count_vert(0),
            layer_vec(),
            tiles_panel_vec(),
            texture_vec(),
            collision_rect_vec(),
            empty_texture()
        {}

        void ResetBeforeLoading()
        {
            tile_size_horiz = 0;
            tile_size_vert = 0;
            tile_count_horiz = 0;
            tile_count_vert = 0;
            layer_vec.clear();
            tiles_panel_vec.clear();
            texture_vec.clear();
            collision_rect_vec.clear();
        }

        unsigned tile_size_horiz;
        unsigned tile_size_vert;
        unsigned tile_count_horiz;
        unsigned tile_count_vert;

        LayerVec_t layer_vec;

        TilesPanelVec_t tiles_panel_vec;
        
        sfml_util::TextureVec_t texture_vec;

        sfml_util::FloatRectVec_t collision_rect_vec;

        sf::RenderTexture empty_texture;

        inline const std::string EmptyTilesPanelName() const { return "empty"; }
    };

}
}


#endif //HEROESPATH_LAYOUT_HPP_INCLUDED
