// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_TILEOFFSETS_HPP_INCLUDED
#define HEROESPATH_TILEOFFSETS_HPP_INCLUDED
//
// tile-offsets.hpp
//
#include "misc/types.hpp"

#include <SFML/System/Vector2.hpp>

#include <tuple>

namespace heroespath
{
namespace map
{

    // Responsible for wrapping the range of tile positions that are drawn on the map.
    struct TileOffsets
    {
        explicit TileOffsets(
            const sf::Vector2i & BEGIN_V = sf::Vector2i(0, 0),
            const sf::Vector2i & END_V = sf::Vector2i(0, 0))
            : begin_v(BEGIN_V)
            , end_v(END_V)
        {}

        sf::Vector2i begin_v;
        sf::Vector2i end_v;
    };

    inline bool operator==(const TileOffsets & L, const TileOffsets & R)
    {
        return std::tie(L.begin_v, L.end_v) == std::tie(R.begin_v, R.end_v);
    }

    inline bool operator!=(const TileOffsets & L, const TileOffsets & R) { return !(L == R); }

} // namespace map
} // namespace heroespath

#endif // HEROESPATH_TILEOFFSETS_HPP_INCLUDED
