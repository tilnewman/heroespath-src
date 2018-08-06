// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
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

} // namespace map
} // namespace heroespath

#endif // HEROESPATH_MAP_PARSE_PACKET_HPP_INCLUDED
