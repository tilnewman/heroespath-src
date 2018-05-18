// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_CREATURE_RANK_HPP_INCLUDED
#define HEROESPATH_CREATURE_RANK_HPP_INCLUDED
//
// rank.hpp
//  Code that gives meaning to all possible rank values.
//
#include "creature/trait.hpp"
#include "misc/types.hpp"

#include "misc/range.hpp"

#include <string>
#include <utility>

namespace heroespath
{
namespace creature
{

    using RankRange_t = misc::Range<Rank_t>;

    class rank_class
    {
    public:
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

        static rank_class::Enum FromRank(const Rank_t & RANK);

        // returns a second/max of zero to represent limitless
        static const RankRange_t RankRangeByClass(const rank_class::Enum E);

        static const std::string ToString(const rank_class::Enum E);
    };
} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_RANK_HPP_INCLUDED
