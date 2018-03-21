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
#ifndef HEROESPATH_TILESPANEL_HPP_INCLUDED
#define HEROESPATH_TILESPANEL_HPP_INCLUDED
//
// tiles-panel.hpp
//
#include "misc/types.hpp"

#include <boost/filesystem.hpp>

#include <string>
#include <vector>

namespace heroespath
{
namespace map
{

    // Responsible for wrapping all the details about a single panel of map tile images.
    struct TilesPanel
    {
        explicit TilesPanel(
            const std::string & NAME = "",
            const std::string & RELATIVE_PATH = "",
            const int & FIRST_ID = 0,
            const int & TILE_COUNT = 1,
            const int & COLUMN = 1,
            const std::size_t & TEXTURE_INDEX = 0)
            : name(NAME)
            , path_rel(RELATIVE_PATH)
            , first_id(FIRST_ID)
            , tile_count(TILE_COUNT)
            , column(COLUMN)
            , texture_index(TEXTURE_INDEX)
            , path_obj()
        {}

        bool OwnsId(const int ID) const
        {
            return ((ID >= first_id) && (ID < (first_id + tile_count)));
        }

        std::string name;
        std::string path_rel; // path string relative to the .tmx map path
        int first_id;
        int tile_count;
        int column;
        std::size_t texture_index;
        boost::filesystem::path path_obj;
    };

    using TilesPanelVec_t = std::vector<TilesPanel>;

    // Responsible for wrapping only the details layers need about a single panel
    // of map tiles images.
    struct TilesPanelForLayers
    {
        TilesPanelForLayers(const bool IS_EMPTY, const std::size_t TEXTURE_INDEX)
            : is_empty(IS_EMPTY)
            , texture_index(TEXTURE_INDEX)
        {}

        bool is_empty;
        std::size_t texture_index;
    };

    using TilesPanelForLayersVec_t = std::vector<TilesPanelForLayers>;
} // namespace map
} // namespace heroespath

#endif // HEROESPATH_TILESIMAGE_HPP_INCLUDED
