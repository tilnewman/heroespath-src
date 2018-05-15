// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
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
