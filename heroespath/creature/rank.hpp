#ifndef APPBASE_HEROESPATH_CREATURE_RANK_INCLUDED
#define APPBASE_HEROESPATH_CREATURE_RANK_INCLUDED
//
// rank.hpp
//  Code that gives meaning to all possible rank values.
//
#include "heroespath/stats/types.hpp"

#include <string>
#include <utility>


namespace heroespath
{
namespace creature
{

    struct rank_class
    {
        enum Enum
        {
            Novice = 0,
            Trainee,
            Skilled,
            Expert,
            Master,
            GrandMaster,
            Count
        };

        static rank_class::Enum FromRank(const stats::Rank_t RANK);

        //returns a second/max of zero to represent limitless
        static std::pair<stats::Rank_t, stats::Rank_t> RankRangeByClass(const rank_class::Enum E);

        static const std::string ToString(const rank_class::Enum E);
    };

}
}
#endif //APPBASE_HEROESPATH_CREATURE_RANK_INCLUDED
