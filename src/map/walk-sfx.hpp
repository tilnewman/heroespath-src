// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
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
            const sfml_util::sound_effect::Enum SFX = sfml_util::sound_effect::Count)
            : region(REGION)
            , sfx(SFX)
        {}

        sf::FloatRect region;
        sfml_util::sound_effect::Enum sfx;
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

        sfml_util::sound_effect::Enum FindSfx(const sf::Vector2f & POSITION) const
        {
            for (auto const & SFX_REGION : top_layers)
            {
                if (SFX_REGION.region.contains(POSITION))
                {
                    return SFX_REGION.sfx;
                }
            }

            for (auto const & SFX_REGION : bottom_layers)
            {
                if (SFX_REGION.region.contains(POSITION))
                {
                    return SFX_REGION.sfx;
                }
            }

            return sfml_util::sound_effect::Count;
        }

        WalkSfxRegionVec_t bottom_layers;
        WalkSfxRegionVec_t top_layers;
    };

} // namespace map
} // namespace heroespath

#endif // HEROESPATH_MAP_WALK_SFX_HPP_INCLUDED
