#ifndef GAME_CREATURE_ROLESTATS_INCLUDED
#define GAME_CREATURE_ROLESTATS_INCLUDED
//
// role-stats.hpp
//  Specific creature role stat modifiers.
//
#include "game/stats/stat-set.hpp"
#include "game/creature/role-enum.hpp"

#include <map>
#include <string>


namespace game
{
namespace creature
{

    using RoleStatSetMap_t = std::map<role::Enum, stats::StatSet>;


    //Responsible for role related stat modifiers, for loading them from the
    //game data file and for providing access to them.
    struct RoleStatModifier
    {
        static const stats::StatSet Get(const role::Enum);

    private:
        static RoleStatSetMap_t roleStatSetMap_;
    };

}
}

#endif //GAME_CREATURE_ROLESTATS_INCLUDED
