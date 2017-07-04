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
// conditions.cpp
//
#include "conditions.hpp"

#include "game/creature/condition-warehouse.hpp"
#include "game/creature/stats.hpp"
#include "game/combat/fight.hpp"

#include "misc/assertlogandthrow.hpp"
#include "misc/random.hpp"

#include <sstream>
#include <exception>


namespace game
{
namespace creature
{
namespace condition
{

    void AsleepMagical::PerTurnEffect(CreaturePtr_t          creaturePtr,
                                      combat::HitInfoVec_t & hitInfoVec,
                                      bool &) const
    {
        //if previous condition slept the creature, then don't wake
        for (auto const & NEXT_HIT_INFO : hitInfoVec)
        {
            if (NEXT_HIT_INFO.CondsAddedContains(creature::Conditions::AsleepNatural) ||
                NEXT_HIT_INFO.CondsAddedContains(creature::Conditions::AsleepMagical))
            {
                return;
            }
        }

        if (misc::random::Int(100) < (8 + static_cast<int>(creaturePtr->Rank())))
        {
            creaturePtr->ConditionRemove(Conditions::AsleepMagical);

            const ContentAndNamePos CNP(" wakes from a magical sleep.",
                                        NamePosition::TargetBefore);

            hitInfoVec.push_back( combat::HitInfo(false,
                                                  Which(),
                                                  CNP,
                                                  0,
                                                  {},
                                                  { Conditions::AsleepMagical }) );
        }
    }


    void AsleepNatural::PerTurnEffect(CreaturePtr_t          creaturePtr,
                                      combat::HitInfoVec_t & hitInfoVec,
                                      bool &) const
    {
        //if previous condition slept the creature, then don't wake
        for (auto const & NEXT_HIT_INFO : hitInfoVec)
        {
            if (NEXT_HIT_INFO.CondsAddedContains(creature::Conditions::AsleepNatural) ||
                NEXT_HIT_INFO.CondsAddedContains(creature::Conditions::AsleepMagical))
            {
                return;
            }
        }

        if (misc::random::Int(100) < (15 + static_cast<int>(creaturePtr->Rank())))
        {
            creaturePtr->ConditionRemove(Conditions::AsleepNatural);

            const ContentAndNamePos CNP(" wakes from a natural sleep.",
                                        NamePosition::TargetBefore);

            hitInfoVec.push_back( combat::HitInfo(false,
                                                  Which(),
                                                  CNP,
                                                  0,
                                                  {},
                                                  { Conditions::AsleepNatural }) );
        }
    }


    void Dazed::PerTurnEffect(CreaturePtr_t          creaturePtr,
                              combat::HitInfoVec_t & hitInfoVec,
                              bool &) const
    {
        //if previous condition Dazed the creature, then don't clear
        for (auto const & NEXT_HIT_INFO : hitInfoVec)
        {
            if (NEXT_HIT_INFO.CondsAddedContains(creature::Conditions::Dazed))
            {
                return;
            }
        }

        if (misc::random::Int(100) < (10 + static_cast<int>(creaturePtr->Rank())))
        {
            creaturePtr->ConditionRemove(Conditions::Dazed);

            const ContentAndNamePos CNP("'s mind clears an is not Dazed anymore.",
                                        NamePosition::TargetBefore);

            hitInfoVec.push_back( combat::HitInfo(false,
                                                  Which(),
                                                  CNP,
                                                  0,
                                                  {},
                                                  { Conditions::Dazed }) );
        }
    }


    void Pounced::PerTurnEffect(CreaturePtr_t          creaturePtr,
                                combat::HitInfoVec_t & hitInfoVec,
                                bool &                 hasTurnBeenConsumed) const
    {
        //can't spend turn getting up if turn already consumed/spent
        if (hasTurnBeenConsumed)
        {
            return;
        }

        //if previous condition tripped the creature, then don't get up
        for (auto const & NEXT_HIT_INFO : hitInfoVec)
        {
            if (NEXT_HIT_INFO.CondsAddedContains(creature::Conditions::Tripped))
            {
                return;
            }
        }

        if (creature::Stats::Test(creaturePtr, stats::stat::Strength, 0.0f, true, true))
        {
            creaturePtr->ConditionRemove(Conditions::Pounced);

            const ContentAndNamePos CNP(" gets up after being pounced on.",
                                        NamePosition::TargetBefore);

            hitInfoVec.push_back( combat::HitInfo(false,
                                                  Which(),
                                                  CNP,
                                                  0,
                                                  {},
                                                  { Conditions::Pounced }) );

            hasTurnBeenConsumed = true;
        }
    }


    void Tripped::PerTurnEffect(CreaturePtr_t          creaturePtr,
                                combat::HitInfoVec_t & hitInfoVec,
                                bool &                 hasTurnBeenConsumed) const
    {
        //can't spend turn getting up if turn already consumed/spent
        if (hasTurnBeenConsumed)
        {
            return;
        }

        //if previous condition tripped the creature, then don't get up
        for (auto const & NEXT_HIT_INFO : hitInfoVec)
        {
            if (NEXT_HIT_INFO.CondsAddedContains(creature::Conditions::Tripped))
            {
                return;
            }
        }

        creaturePtr->ConditionRemove(Conditions::Tripped);

        const ContentAndNamePos CNP(" gets up after being tripped.", NamePosition::TargetBefore);

        hitInfoVec.push_back( combat::HitInfo(false,
                                              Which(),
                                              CNP,
                                              0,
                                              {},
                                              { Conditions::Tripped }) );

        hasTurnBeenConsumed = true;
    }


    void Poisoned::PerTurnEffect(CreaturePtr_t          creaturePtr,
                                 combat::HitInfoVec_t & hitInfoVec,
                                 bool &) const
    {
        if (creature::Stats::Test(creaturePtr, stats::stat::Strength, 0.0f, true, true) &&
            (misc::random::Int(9) == 0))
        {
            creature::CondEnumVec_t condsRemoved;

            combat::FightClub::RemoveAddedCondition(creature::Conditions::Poisoned,
                                                    creaturePtr,
                                                    hitInfoVec,
                                                    condsRemoved);

            std::ostringstream ss;
            ss << "The poison has left ";

            if (creaturePtr->IsPlayerCharacter() == false)
            {
                ss << "the ";
            }

            auto const CONTENT_NAME_POS{ ContentAndNamePos(ss.str(), "'s body.", "",
                NamePosition::TargetBefore) };

            hitInfoVec.push_back(combat::HitInfo(false,
                                                 Which(),
                                                 CONTENT_NAME_POS,
                                                 0,
                                                 {},
                                                 condsRemoved) );
        }
        else
        {
            const stats::Health_t DAMAGE_BASE{ ((creaturePtr->IsPixie()) ?
                0 : misc::random::Int(1, 5)) };

            auto const DAMAGE_RAND_MAX{ std::max(1, static_cast<int>(
                static_cast<float>(creaturePtr->HealthNormal()) * 0.1f)) };

            auto const DAMAGE_FROM_HEALTH_NORMAL{ misc::random::Int(1, DAMAGE_RAND_MAX) };

            const stats::Health_t DAMAGE_FINAL{ -1 * (DAMAGE_BASE + DAMAGE_FROM_HEALTH_NORMAL) };

            CondEnumVec_t condsAddedVec;
            CondEnumVec_t condsRemovedVec;

            combat::FightClub::HandleDamage(creaturePtr,
                                            hitInfoVec,
                                            DAMAGE_FINAL,
                                            condsAddedVec,
                                            condsRemovedVec,
                                            false);

            std::ostringstream ss;
            ss << " hurts " << creature::sex::HimHerIt(creaturePtr->Sex(), false)
                << " for " << std::abs(DAMAGE_FINAL) << " damage.";

            const ContentAndNamePos CNP("The poison in ", ss.str(), "", NamePosition::TargetBefore);

            hitInfoVec.push_back(combat::HitInfo(true, Which(), CNP, DAMAGE_FINAL,
                condsAddedVec, condsRemovedVec));
        }
    }

}
}
}
