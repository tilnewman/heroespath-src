// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_CREATURE_RACESTATS_HPP_INCLUDED
#define HEROESPATH_CREATURE_RACESTATS_HPP_INCLUDED
//
// race-stats.hpp
//  Race specific attribute stat modifiers.
//
#include "creature/race-enum.hpp"
#include "creature/stat-set.hpp"
#include "misc/vector-map.hpp"
#include <string>

namespace heroespath
{
namespace creature
{

    using RaceStatSetMap_t = misc::VectorMap<race::Enum, StatSet>;

    // Responsible for reading race related stat modifiers from the game data
    // file and then providing access to them.
    struct RaceStatModifier
    {
        static const StatSet Get(const creature::race::Enum);

    private:
        static RaceStatSetMap_t raceStatSetMap_;
    };

} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_RACESTATS_HPP_INCLUDED
