///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef GAME_STATS_STATSET_HPP_INCLUDED
#define GAME_STATS_STATSET_HPP_INCLUDED
//
// stat-set.hpp
//
#include "game/stats/types.hpp"
#include "game/stats/trait-enum.hpp"

#include <tuple>
#include <string>


namespace game
{
namespace stats
{

    //Convenience warpper for creature stat values.
    //Because sometimes the TraitSet class is just cumbersome overkill.
    class StatSet
    {
    public:
        StatSet(const stats::Trait_t STR = 0,
                const stats::Trait_t ACC = 0,
                const stats::Trait_t CHA = 0,
                const stats::Trait_t LCK = 0,
                const stats::Trait_t SPD = 0,
                const stats::Trait_t INT = 0);

        inline stats::Trait_t Str() const { return str_; }
        inline stats::Trait_t Acc() const { return acc_; }
        inline stats::Trait_t Cha() const { return cha_; }
        inline stats::Trait_t Lck() const { return lck_; }
        inline stats::Trait_t Spd() const { return spd_; }
        inline stats::Trait_t Int() const { return int_; }

        stats::Trait_t Get(const Traits::Enum) const;
        void Set(const Traits::Enum, const stats::Trait_t);

        const std::string ToString(const bool WILL_WRAP) const;

        friend bool operator==(const StatSet & L, const StatSet & R);
        friend bool operator<(const StatSet & L, const StatSet & R);

    private:
        stats::Trait_t str_;
        stats::Trait_t acc_;
        stats::Trait_t cha_;
        stats::Trait_t lck_;
        stats::Trait_t spd_;
        stats::Trait_t int_;
    };


    inline bool operator==(const StatSet & L, const StatSet & R)
    {
        return std::tie(L.str_, L.acc_, L.cha_, L.lck_, L.spd_, L.int_)
                ==
               std::tie(R.str_, R.acc_, R.cha_, R.lck_, R.spd_, R.int_);
    }


    inline bool operator!=(const StatSet & L, const StatSet & R)
    {
        return ! (L == R);
    }


    inline bool operator<(const StatSet & L, const StatSet & R)
    {
        return std::tie(L.str_, L.acc_, L.cha_, L.lck_, L.spd_, L.int_)
                <
               std::tie(R.str_, R.acc_, R.cha_, R.lck_, R.spd_, R.int_);
    }

}
}


#endif //GAME_STATS_STATSET_HPP_INCLUDED
