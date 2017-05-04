#ifndef GAME_CREATURE_ROLESTATS_INCLUDED
#define GAME_CREATURE_ROLESTATS_INCLUDED
//
// role-stats.hpp
//  Specific creature role stat modifiers.
//
#include "game/creature/role-enum.hpp"
#include "game/stats/stat-set.hpp"


namespace game
{
namespace creature
{

    const stats::StatSet StatModifierByRole(const role::Enum);

}
}
#endif //GAME_CREATURE_ROLESTATS_INCLUDED
