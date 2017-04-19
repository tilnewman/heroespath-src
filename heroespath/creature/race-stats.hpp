#ifndef APPBASE_HEROESPATH_CREATURE_RACESTATS_INCLUDED
#define APPBASE_HEROESPATH_CREATURE_RACESTATS_INCLUDED
//
// race-stats.hpp
//  Race specific attribute stat modifiers.
//
#include "heroespath/stats/stat-set.hpp"
#include "heroespath/creature/race-enum.hpp"
#include "heroespath/creature/role-enum.hpp"
#include <string>


namespace heroespath
{
namespace creature
{

    const stats::StatSet StatModifierByRace(const creature::race::Enum);

}
}
#endif //APPBASE_HEROESPATH_CREATURE_RACESTATS_INCLUDED
