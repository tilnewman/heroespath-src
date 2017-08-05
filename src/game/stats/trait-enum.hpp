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
#ifndef GAME_STATS_TRAITENUM_HPP_INCLUDED
#define GAME_STATS_TRAITENUM_HPP_INCLUDED
//
// trait-enum.hpp
//
#include <string>
#include <vector>
#include <utility>


namespace game
{
namespace stats
{

    //Identifies a RatioStat
    struct Traits
    {
        enum Enum
        {
            //Keep stats starting at zero and in order.
            Strength = 0,
            Accuracy,
            Charm,
            Luck,
            Speed,
            Intelligence,
            StatCount,
            Mana = StatCount,
            ArmorRating,
            MagicCast,
            MagicEffect,
            MagicResist,
            Encounter,
            Surprise,
            FindReligious,
            FindMagic,
            FindCoinsAmount,
            Backstab,
            HealthGainMelee,
            HealthGainAll,
            AnimalResist,
            DamageBonusFist,
            DamageBonusMelee,
            DamageBonusProj,
            DamageBonusAll,
            PoisonOnMelee,
            PoisonOnAll,
            PanicOnSpell,
            CurseCast,
            CurseEffect,
            CurseResist,
            CurseOnDamage,
            BlessCast,
            BlessEffect,
            BlessResist,
            HitPower,
            HitCritical,
            FireDamage,
            FireResist,
            FrostDamage,
            FrostResist,
            HonorDamage,
            HonorResist,
            ShadowDamage,
            ShadowResist,
            Count
        };

        static const std::string ToString(const Enum);
        static const std::string Abbr(const Enum);
        static const std::string Name(const Enum);
        static const std::string Desc(const Enum);
    };


    using TraitsVec_t = std::vector<Traits::Enum>;
    using TraitValuePair_t = std::pair<Traits::Enum, int>;
    using TraitValueVec_t = std::vector<TraitValuePair_t>;

}
}

#endif //GAME_STATS_TRAITENUM_HPP_INCLUDED
