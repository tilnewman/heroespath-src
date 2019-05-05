// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// tiles-panel.cpp
//
#include "tiles-panel.hpp"

namespace heroespath
{
namespace map
{

    TilesPanel::TilesPanel(
        const std::string & NAME,
        const std::string & FILE_PATH,
        const int & FIRST_ID,
        const int & TILE_COUNT,
        const int & COLUMN,
        const std::size_t & TEXTURE_INDEX)
        : name(NAME)
        , first_id(FIRST_ID)
        , tile_count(TILE_COUNT)
        , column(COLUMN)
        , texture_index(TEXTURE_INDEX)
        , path_str(FILE_PATH)
    {}

    bool TilesPanel::OwnsId(const int ID) const
    {
        return ((ID >= first_id) && (ID < (first_id + tile_count)));
    }

} // namespace map
} // namespace heroespath
