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
#include <string>
#include <vector>

namespace heroespath
{
namespace map
{

    // Responsible for wrapping all the details about a single panel of map tile images.
    struct TilesPanel
    {
        // creates a panel for the "empty" texture
        TilesPanel();

        TilesPanel(
            const std::string & NAME,
            const std::string & FILE_NAME,
            const int & FIRST_ID,
            const int & TILE_COUNT,
            const int & COLUMN,
            const std::size_t & TEXTURE_INDEX);

        TilesPanel(const TilesPanel &) = default;
        TilesPanel(TilesPanel &&) = default;
        TilesPanel & operator=(const TilesPanel &) = default;
        TilesPanel & operator=(TilesPanel &&) = default;

        bool OwnsId(const int ID) const;

        static const std::string NameWithEmptyTexture() { return "empty"; }

        std::string name;
        int first_id;
        int tile_count;
        int column;
        std::size_t texture_index;
        std::string path_str;
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
