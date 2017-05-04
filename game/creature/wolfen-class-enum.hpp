#ifndef GAME_CREATURE_WOLFENCLASSENUM_INCLUDED
#define GAME_CREATURE_WOLFENCLASSENUM_INCLUDED
//
// wolfen-class-enum.hpp
//  An enumeration of all Wolfen classes as determined by Rank.
//
#include <string>
#include "game/stats/types.hpp" //for Rank_t


namespace game
{
namespace creature
{

    struct wolfen_class
    {
        enum Enum
        {
            Pup = 0,
            Juvenile,
            Adult,
            Noble,
            Highborn,
            Elder,
            Count
        };

        static const std::string Name(const wolfen_class::Enum);
        static const std::string Desc(const wolfen_class::Enum);
        inline static const std::string ToString(const wolfen_class::Enum E) { return Name(E); }
        static wolfen_class::Enum ClassFromRank(const stats::Rank_t);
    };

}
}
#endif //GAME_CREATURE_WOLFENCLASSENUM_INCLUDED
