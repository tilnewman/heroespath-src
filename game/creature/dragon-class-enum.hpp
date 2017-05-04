#ifndef GAME_CREATURE_DRAGONCLASSENUM_INCLUDED
#define GAME_CREATURE_DRAGONCLASSENUM_INCLUDED
//
// dragon-class-enum.hpp
//  An enumeration of all Dragon classes as determined by Rank.
//
#include <string>
#include "game/stats/types.hpp" //for Rank_t


namespace game
{
namespace creature
{

    struct dragon_class
    {
        enum Enum
        {
            Hatchling = 0,
            Whelp,
            Fledgling,
            Juvenile,
            Adult,
            Wyrm,
            Skycaster,
            Elder,
            Count
        };

        static const std::string Name(const dragon_class::Enum);
        static const std::string Desc(const dragon_class::Enum);
        inline static const std::string ToString(const dragon_class::Enum E) { return Name(E); }
        static dragon_class::Enum ClassFromRank(const stats::Rank_t);
    };

}
}
#endif //GAME_CREATURE_DRAGONCLASSENUM_INCLUDED
