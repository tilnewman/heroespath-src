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
#ifndef HEROESPATH_STATS_STATSET_HPP_INCLUDED
#define HEROESPATH_STATS_STATSET_HPP_INCLUDED
//
// stat-set.hpp
//
#include "misc/types.hpp"
#include "stats/trait-enum.hpp"

#include <string>
#include <tuple>

namespace heroespath
{
namespace stats
{

    // Convenience warpper for creature stat values.
    // Because sometimes the TraitSet class is just cumbersome overkill.
    class StatSet
    {
    public:
        explicit StatSet(
            const Strength_t & STR = 0_str,
            const Accuracy_t & ACC = 0_acc,
            const Charm_t & CHA = 0_cha,
            const Luck_t & LCK = 0_lck,
            const Speed_t & SPD = 0_spd,
            const Intell_t & INT = 0_int);

        Strength_t Str() const { return str_; }
        Accuracy_t Acc() const { return acc_; }
        Charm_t Cha() const { return cha_; }
        Luck_t Lck() const { return lck_; }
        Speed_t Spd() const { return spd_; }
        Intell_t Int() const { return int_; }

        stats::Trait_t Get(const Traits::Enum) const;
        void Set(const Traits::Enum, const stats::Trait_t);

        const std::string ToString(const bool WILL_WRAP) const;

        friend bool operator==(const StatSet & L, const StatSet & R);
        friend bool operator<(const StatSet & L, const StatSet & R);

    private:
        Strength_t str_;
        Accuracy_t acc_;
        Charm_t cha_;
        Luck_t lck_;
        Speed_t spd_;
        Intell_t int_;
    };

    inline bool operator==(const StatSet & L, const StatSet & R)
    {
        return std::tie(L.str_, L.acc_, L.cha_, L.lck_, L.spd_, L.int_)
            == std::tie(R.str_, R.acc_, R.cha_, R.lck_, R.spd_, R.int_);
    }

    inline bool operator!=(const StatSet & L, const StatSet & R) { return !(L == R); }

    inline bool operator<(const StatSet & L, const StatSet & R)
    {
        return std::tie(L.str_, L.acc_, L.cha_, L.lck_, L.spd_, L.int_)
            < std::tie(R.str_, R.acc_, R.cha_, R.lck_, R.spd_, R.int_);
    }
} // namespace stats
} // namespace heroespath

#endif // HEROESPATH_STATS_STATSET_HPP_INCLUDED
