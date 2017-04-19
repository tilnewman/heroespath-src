#ifndef APPBASE_HEROESPATH_STATS_TYPES_INCLUDED
#define APPBASE_HEROESPATH_STATS_TYPES_INCLUDED
//
// types.hpp
//  Various typedefs required by the heroespath project
//
#include <cstddef> //for std::size_t


namespace heroespath
{
namespace stats
{

    //The base numerical type for all Stat objects.  Should be signed.
    using Stat_t = int;

    static const Stat_t INITIAL_STAT_MIN(0);
    static const Stat_t INITIAL_STAT_MAX(20);
    static const Stat_t INVALID_STAT_VAL(-1);

    //The numerical type for experience points.  Should be unsigned.
    using Exp_t = unsigned int;

    //The numerical type for Rank.  Should be unsigned.
    using Rank_t = std::size_t;

    //The numerical type for Creature Health.  Should be signed.
    using Health_t = int;

    //The numerical type of the armor rating.  Shoulde be signed.
    using Armor_t = int;

}
}
#endif //APPBASE_HEROESPATH_STATS_TYPES_INCLUDED
