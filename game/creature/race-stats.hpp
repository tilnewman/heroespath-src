#ifndef GAME_CREATURE_RACESTATS_INCLUDED
#define GAME_CREATURE_RACESTATS_INCLUDED
//
// race-stats.hpp
//  Race specific attribute stat modifiers.
//
#include "game/stats/stat-set.hpp"
#include "game/creature/race-enum.hpp"
#include "game/creature/role-enum.hpp"
#include <string>


namespace game
{
namespace creature
{

    const stats::StatSet StatModifierByRace(const creature::race::Enum);

}
}
#endif //GAME_CREATURE_RACESTATS_INCLUDED
