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

#include <tuple>



namespace heroespath
{
namespace map
{

    //Responsible for wrapping the range of tile positions that are drawn on the map.
    struct TileOffsets
    {
        TileOffsets(
            const int BEGIN_X = 0,
            const int END_X = 0,
            const int BEGIN_Y = 0,
            const int END_Y = 0)
        :
            begin_x(BEGIN_X),
            end_x(END_X),
            begin_y(BEGIN_Y),
            end_y(END_Y)
        {}

        int begin_x;
        int end_x;
        int begin_y;
        int end_y;
    };


    inline bool operator==(const TileOffsets & L, const TileOffsets & R)
    {
        return std::tie(L.begin_x, L.begin_y, L.end_x, L.end_y)
                ==
               std::tie(R.begin_x, R.begin_y, R.end_x, R.end_y);
    }


    inline bool operator!=(const TileOffsets & L, const TileOffsets & R)
    {
        return ! (L == R);
    }

}
}


#endif //HEROESPATH_TILEOFFSETS_HPP_INCLUDED
