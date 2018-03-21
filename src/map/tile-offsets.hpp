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
#ifndef HEROESPATH_TILEOFFSETS_HPP_INCLUDED
#define HEROESPATH_TILEOFFSETS_HPP_INCLUDED
//
// tile-offsets.hpp
//
#include "misc/types.hpp"
#include "sfml-util/sfml-graphics.hpp"

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
