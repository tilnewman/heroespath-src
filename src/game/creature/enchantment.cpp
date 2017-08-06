// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
//
// enchantment.cpp
//
#include "enchantment.hpp"

#include "game/creature/creature.hpp"
#include "game/creature/condition-algorithms.hpp"

#include <sstream>


namespace game
{
namespace creature
{

    Enchantment::Enchantment(const EnchantmentType::Enum TYPE,
                             const stats::TraitSet &     TRAIT_SET,
                             const UseInfo &             USE_INFO,
                             const SummonInfo &          SUMMON_INFO)
    :
        type_       (TYPE),
        traitSet_   (TRAIT_SET),
        useInfo_    (USE_INFO),
        summonInfo_ (SUMMON_INFO)
    {}


    Enchantment::~Enchantment()
    {}


    const std::string Enchantment::EffectStr(const CreaturePtr_t) const
    {
        std::ostringstream ss;

        ss << EnchantmentType::ToString(type_);

        if (IsUseableEver())
        {
            auto const COUNT_REMAIN{ useInfo_.CountRemaining() };
            if (COUNT_REMAIN > 0)
            {
                ss << SepIfNotEmpty(ss.str()) << COUNT_REMAIN << " uses left";
            }
            else if (COUNT_REMAIN == 0)
            {
                ss << SepIfNotEmpty(ss.str()) << "cannot be used again";
            }
            else
            {
                ss << SepIfNotEmpty(ss.str()) << "";
            }

            auto const SPELL{ useInfo_.Spell() };
            if (SPELL != spell::Spells::Count)
            {
                ss << SepIfNotEmpty(ss.str()) << "casts the "
                    << spell::Spells::Name(SPELL) << " spell";
            }

            auto const CONDS_REMOVED_VEC{ useInfo_.ConditionsRemoved() };
            if (CONDS_REMOVED_VEC.empty())
            {
                ss << SepIfNotEmpty(ss.str()) << "removes the conditions: "
                    << creature::condition::Algorithms::Names(CONDS_REMOVED_VEC, false, true);
            }
        }

        if (useInfo_.RestrictedToPhase() != Phase::NotAPhase)
        {
            ss << SepIfNotEmpty(ss.str()) << ", use only during "
                << Phase::ToString(useInfo_.RestrictedToPhase(), false);
        }

        auto const TRAITS_STR{ traitSet_.ToString(false, false, false, true) };
        if (TRAITS_STR.empty() == false)
        {
            ss << SepIfNotEmpty(ss.str()) << "Traits: " << TRAITS_STR;
        }

        if (summonInfo_.Rank() > 0)
        {
            ss << SepIfNotEmpty(ss.str()) << "summons a " << summonInfo_.Rank() << " rank "
                << creature::race::Name(summonInfo_.Race())
                << " " << creature::role::Name(summonInfo_.Role())
                << " to fight for the party";
        }
        
        return ss.str();
    }


    int Enchantment::TreasureScore() const
    {
        using namespace stats;

        int score{ 0 };

        //TODO move this part of the calculation to treasure factory
        for (int i(1); i < Traits::Count; ++i)
        {
            auto const NEXT_TRAIT_ENUM{ static_cast<Traits::Enum>(i) };
            auto const NEXT_TRAIT_VALUE{ traitSet_.GetCopy(NEXT_TRAIT_ENUM).Current() };
            auto traitScore{ [NEXT_TRAIT_VALUE]()
                {
                    if (NEXT_TRAIT_VALUE >= 0)
                    {
                        return 10 * NEXT_TRAIT_VALUE;
                    }
                    else
                    {
                        return 5 * std::abs(NEXT_TRAIT_VALUE);
                    }
                }() };

            if ((NEXT_TRAIT_ENUM == Traits::HealthGainAll) ||
                (NEXT_TRAIT_ENUM == Traits::HealthGainMelee))
            {
                traitScore *= 10;
            }
            else if ((NEXT_TRAIT_ENUM == Traits::AnimalResist) ||
                     (NEXT_TRAIT_ENUM == Traits::ArmorRating) ||
                     (NEXT_TRAIT_ENUM == Traits::Backstab) ||
                     (NEXT_TRAIT_ENUM == Traits::CurseOnDamage) ||
                     (NEXT_TRAIT_ENUM == Traits::DamageBonusAll) ||
                     (NEXT_TRAIT_ENUM == Traits::DamageBonusMelee) ||
                     (NEXT_TRAIT_ENUM == Traits::DamageBonusProj) ||
                     (NEXT_TRAIT_ENUM == Traits::FindCoinsAmount) ||
                     (NEXT_TRAIT_ENUM == Traits::PoisonOnAll) ||
                     (NEXT_TRAIT_ENUM == Traits::PoisonOnMelee))
            {
                traitScore *= 2;
            }
            else if (NEXT_TRAIT_ENUM == Traits::DamageBonusFist)
            {
                traitScore /= 4;
            }

            score += traitScore;
        }

        if (type_ & EnchantmentType::WhenHeld)
        {
            score += 350;
        }

        if (type_ & EnchantmentType::AllowsFlight)
        {
            score += 200;
        }

        if ((type_ & EnchantmentType::BlessWithoutItem) ||
            (type_ & EnchantmentType::CurseWithoutItem))
        {
            score += 300;
        }

        return score;
    }

}
}
