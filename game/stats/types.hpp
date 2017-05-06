#ifndef GAME_STATS_TYPES_INCLUDED
#define GAME_STATS_TYPES_INCLUDED
//
// types.hpp
//  Various typedefs required by the heroespath project
//
#include <cstddef> //for std::size_t


namespace game
{
namespace stats
{

    //The base numerical type for all Stat objects.  Must be signed.
    using Stat_t = int;

    //for experience points, must be unsigned
    using Exp_t = unsigned int;

    //for Rank, must be unsigned
    using Rank_t = std::size_t;

    //for Creature Health, must be signed
    using Health_t = int;

    //for Armor Rating, must be signed
    using Armor_t = int;

    //for mana (spell points), must be signed
    using Mana_t = int;

}
}
#endif //GAME_STATS_TYPES_INCLUDED
