#ifndef GAME_CREATURE_RACESTATS_INCLUDED
#define GAME_CREATURE_RACESTATS_INCLUDED
//
// race-stats.hpp
//  Race specific attribute stat modifiers.
//
#include "game/stats/stat-set.hpp"
#include "game/creature/race-enum.hpp"

#include <map>
#include <string>


namespace game
{
namespace creature
{

    using RaceStatSetMap_t = std::map<race::Enum, stats::StatSet>;


    //Responsible for reading race related stat modifiers from the game data
    //file and then providing access to them.
    struct RaceStatModifier
    {
        static const stats::StatSet Get(const creature::race::Enum);

    private:
        static RaceStatSetMap_t raceStatSetMap_;
    };

}
}

#endif //GAME_CREATURE_RACESTATS_INCLUDED
