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
#ifndef HEROESPATH_MAP_WALK_SFX_HPP_INCLUDED
#define HEROESPATH_MAP_WALK_SFX_HPP_INCLUDED
//
// walk-sfx.hpp
//
#include "sfml-util/music-enum.hpp"
#include "sfml-util/sfml-util.hpp"
#include <algorithm>
#include <vector>

namespace heroespath
{
namespace map
{

    // Responsible for wrapping a region of the map with the walk sfx that goes with it.
    struct WalkSfxRegion
    {
        explicit WalkSfxRegion(
            const sf::FloatRect & REGION = sf::FloatRect(),
            const sfml_util::music::Enum MUSIC = sfml_util::music::Count)
            : region(REGION)
            , music(MUSIC)
        {}

        sf::FloatRect region;
        sfml_util::music::Enum music;
    };

    inline bool operator<(const WalkSfxRegion & L, const WalkSfxRegion & R)
    {
        // Only consider the region when sorting
        return L.region < R.region;
    }

    using WalkSfxRegionVec_t = std::vector<WalkSfxRegion>;

    // Responsible for sorting and wrapping the bottom layer WalkSfxRegions and the top layer.
    struct WalkSfxRegionLayers
    {
        explicit WalkSfxRegionLayers(
            const WalkSfxRegionVec_t & BOTTOM_LAYERS = WalkSfxRegionVec_t(),
            const WalkSfxRegionVec_t & TOP_LAYERS = WalkSfxRegionVec_t())
            : bottom_layers(BOTTOM_LAYERS)
            , top_layers(TOP_LAYERS)
        {}

        inline sfml_util::music::Enum FindSfx(const sf::Vector2f & POSITION) const
        {
            for (auto const & SFX_REGION : top_layers)
            {
                if (SFX_REGION.region.contains(POSITION))
                {
                    return SFX_REGION.music;
                }
            }

            for (auto const & SFX_REGION : bottom_layers)
            {
                if (SFX_REGION.region.contains(POSITION))
                {
                    return SFX_REGION.music;
                }
            }

            return sfml_util::music::Count;
        }

        WalkSfxRegionVec_t bottom_layers;
        WalkSfxRegionVec_t top_layers;
    };
} // namespace map
} // namespace heroespath

#endif // HEROESPATH_MAP_WALK_SFX_HPP_INCLUDED
