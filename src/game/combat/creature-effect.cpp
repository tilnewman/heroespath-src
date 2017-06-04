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
// creature-effect.cpp
//
#include "creature-effect.hpp"

#include "game/creature/condition.hpp"
#include "game/creature/condition-algorithms.hpp"
#include "game/creature/creature.hpp"

#include "misc/vectors.hpp"

#include <algorithm>

#include <boost/algorithm/string.hpp>

namespace game
{
namespace combat
{

    CreatureEffect::CreatureEffect(const creature::CreaturePtr_t        CREATURE_PTR,
                                   const HitInfoVec_t &                 HIT_INFO_VEC,
                                   const spell::SpellPtr_t              SPELL_PTR,
                                   const creature::ConditionEnumVec_t & CONDITIONS_VEC,
                                   const bool                           WAS_POUNCED)
    :
        hitInfoVec_   (HIT_INFO_VEC),
        spellPtr_     (SPELL_PTR),
        creaturePtr_  (CREATURE_PTR),
        conditionsVec_(CONDITIONS_VEC),
        wasPounced_   (WAS_POUNCED)
    {}


    CreatureEffect::CreatureEffect(const CreatureEffect & CE)
    :
        hitInfoVec_   (CE.hitInfoVec_),

        //The lifetime of these objects is not managed by this class.
        //Usage is short-term observation only, so ptr copying is safe.
        spellPtr_     (CE.spellPtr_),
        creaturePtr_  (CE.creaturePtr_),

        conditionsVec_(CE.conditionsVec_),
        wasPounced_   (CE.wasPounced_)
    {}


    CreatureEffect & CreatureEffect::operator=(const CreatureEffect & CE)
    {
        if (& CE != this)
        {
            hitInfoVec_ = CE.hitInfoVec_;
            conditionsVec_ = CE.conditionsVec_;
            wasPounced_ = CE.wasPounced_;

            //see copy constructor comment regarding these pointers
            spellPtr_ = CE.spellPtr_;
            creaturePtr_ = CE.creaturePtr_;
        }

        return * this;
    }


    bool CreatureEffect::GetWasHit() const
    {
        for (auto const & NEXT_HIT_INFO : hitInfoVec_)
        {
            if (NEXT_HIT_INFO.WasHit())
            {
                return true;
            }
        }

        return false;
    }


    stats::Health_t CreatureEffect::GetDamageTotal() const
    {
        stats::Health_t damageTotal{ 0 };

        for (auto const & NEXT_HIT_INFO : hitInfoVec_)
        {
            damageTotal += NEXT_HIT_INFO.Damage();
        }

        return damageTotal;
    }


    const creature::ConditionEnumVec_t CreatureEffect::GetHitConditions() const
    {
        creature::ConditionEnumVec_t conditionsVec;

        for (auto const & NEXT_HIT_INFO : hitInfoVec_)
        {
            const auto NEXT_CONDITION_VEC{ NEXT_HIT_INFO.Conditions() };
            for (auto const NEXT_CONDITION_ENUM : NEXT_CONDITION_VEC)
            {
                conditionsVec.push_back(NEXT_CONDITION_ENUM);
            }
        }

        return conditionsVec;
    }


    const creature::ConditionEnumVec_t CreatureEffect::GetAllConditions() const
    {
        creature::ConditionEnumVec_t allConditionsVec;

        auto const HIT_CONDITIONS_VEC{ GetHitConditions() };

        if (HIT_CONDITIONS_VEC.empty() == false)
        {
            std::copy(HIT_CONDITIONS_VEC.begin(),
                      HIT_CONDITIONS_VEC.end(),
                      back_inserter(allConditionsVec));
        }

        auto const NON_HIT_CONDITIONS_VEC{ GetNonHitConditions() };

        if (NON_HIT_CONDITIONS_VEC.empty() == false)
        {
            std::copy(NON_HIT_CONDITIONS_VEC.begin(),
                      NON_HIT_CONDITIONS_VEC.end(),
                      back_inserter(allConditionsVec));
        }

        return allConditionsVec;
    }


    bool CreatureEffect::ContainsCondition(const creature::Conditions::Enum E) const
    {
        const auto CONDITIONS_VEC{ GetAllConditions() };

        for (auto const NEXT_CONDITION_ENUM : CONDITIONS_VEC)
        {
            if (NEXT_CONDITION_ENUM == E)
            {
                return true;
            }
        }

        return false;
    }


    bool CreatureEffect::RemoveCondition(const creature::Conditions::Enum E)
    {
        auto wasCondRemoved{ false };

        for (auto & nextHitInfo : hitInfoVec_)
        {
            if (nextHitInfo.RemoveCondition(E))
            {
                wasCondRemoved = true;
            }
        }

        creature::ConditionEnumVec_t condsToRemoveVec;

        for (auto const NEXT_CONDITION_ENUM : conditionsVec_)
        {
            if (NEXT_CONDITION_ENUM == E)
            {
                condsToRemoveVec.push_back(NEXT_CONDITION_ENUM);
            }
        }

        if (condsToRemoveVec.empty() == false)
        {
            wasCondRemoved = true;
        }

        for (auto const NEXT_CONDITION_TO_REMOVE_ENUM : condsToRemoveVec)
        {
            conditionsVec_.erase(std::remove(conditionsVec_.begin(),
                                             conditionsVec_.end(),
                                             NEXT_CONDITION_TO_REMOVE_ENUM), conditionsVec_.end());
        }

        return wasCondRemoved;
    }


    bool operator<(const CreatureEffect & L, const CreatureEffect & R)
    {
        if (misc::Vector::OrderlessCompareLess(L.conditionsVec_, R.conditionsVec_) == true)
        {
            return true;
        }

        return std::tie(L.hitInfoVec_,
                        L.spellPtr_,
                        L.creaturePtr_,
                        L.wasPounced_)
               <
               std::tie(R.hitInfoVec_,
                        R.spellPtr_,
                        R.creaturePtr_,
                        R.wasPounced_);
    }


    bool operator==(const CreatureEffect & L, const CreatureEffect & R)
    {
        if (misc::Vector::OrderlessCompareEqual(L.conditionsVec_, R.conditionsVec_) == false)
        {
            return false;
        }

        return std::tie(L.hitInfoVec_,
                        L.spellPtr_,
                        L.creaturePtr_,
                        L.wasPounced_)
               ==
               std::tie(R.hitInfoVec_,
                        R.spellPtr_,
                        R.creaturePtr_,
                        R.wasPounced_);
    }

}
}
