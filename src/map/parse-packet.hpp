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
#ifndef HEROESPATH_MAP_PARSE_PACKET_HPP_INCLUDED
#define HEROESPATH_MAP_PARSE_PACKET_HPP_INCLUDED
//
// parse-packet.hpp
//
#include "map/layer-type-enum.hpp"
#include "map/layout.hpp"
#include "map/map-anim.hpp"
#include "map/transition.hpp"
#include "map/walk-bounds.hpp"
#include "map/walk-sfx.hpp"
#include "misc/types.hpp"

#include <string>
#include <vector>

namespace heroespath
{
namespace map
{

    // Responsible for wrapping all the data needed to parse a map file.
    struct ParsePacket
    {
        ParsePacket(
            const std::string & FILE_PATH_PARAM,
            Layout & layoutParam,
            std::vector<sf::FloatRect> & collisionVecParam,
            TransitionVec_t & transitionVecParam,
            WalkRectMap_t & walkVecMapParam,
            MapAnimVec_t & mapAnimVecParam,
            WalkSfxRegionLayers & walkSfxLayersParam)
            : file_path(FILE_PATH_PARAM)
            , layout(layoutParam)
            , collision_vec(collisionVecParam)
            , transition_vec(transitionVecParam)
            , walk_region_vecmap(walkVecMapParam)
            , animation_vec(mapAnimVecParam)
            , walkSfxLayers(walkSfxLayersParam)
        {}

        std::string file_path;
        Layout & layout;
        std::vector<sf::FloatRect> & collision_vec;
        TransitionVec_t & transition_vec;
        WalkRectMap_t & walk_region_vecmap;
        MapAnimVec_t & animation_vec;
        WalkSfxRegionLayers & walkSfxLayers;
    };
}
}

#endif // HEROESPATH_MAP_PARSE_PACKET_HPP_INCLUDED
