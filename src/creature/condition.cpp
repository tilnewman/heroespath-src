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
// condition.hpp
//
#include "condition.hpp"

#include "combat/fight.hpp"
#include "creature/creature.hpp"
#include "creature/stats.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/random.hpp"

#include <algorithm>
#include <sstream>
#include <tuple>

namespace heroespath
{
namespace creature
{

    Condition::Condition(
        const Conditions::Enum TYPE, const bool IS_MAGICAL, const stats::TraitSet & TRAIT_SET)
        : type_(TYPE)
        , isMagical_(IS_MAGICAL)
        , traitSet_(TRAIT_SET)
    {}

    const std::string Condition::ToString() const
    {
        std::ostringstream ss;
        ss << Conditions::Name(type_);

        if (isMagical_)
        {
            ss << " (magical)";
        }

        ss << ".  " << Conditions::Desc(type_);

        auto const TRAIT_STR{ traitSet_.ToString(false, false, false, true) };
        if (TRAIT_STR.empty() == false)
        {
            ss << "  Traits: " << TRAIT_STR;
        }

        return ss.str();
    }

    const std::string Condition::LongDesc() const
    {
        std::ostringstream ss;
        ss << Conditions::Desc(type_);

        if (isMagical_)
        {
            ss << " (a magical condition)";
        }

        auto const TRAIT_STR{ traitSet_.ToString(false, false, false, true) };
        if (TRAIT_STR.empty() == false)
        {
            ss << "  Traits: " << TRAIT_STR;
        }

        ss << ".";
        return ss.str();
    }

    bool operator<(const Condition & L, const Condition & R)
    {
        return std::tie(L.type_, L.isMagical_, L.traitSet_)
            < std::tie(R.type_, R.isMagical_, R.traitSet_);
    }

    bool operator==(const Condition & L, const Condition & R)
    {
        return std::tie(L.type_, L.isMagical_, L.traitSet_)
            == std::tie(R.type_, R.isMagical_, R.traitSet_);
    }

    // InitialChange() does not alter stats, see creature.cpp::ConditionAdd() for that
    void Condition::InitialChange(CreaturePtrC_t) const {}

    void Condition::FinalChange(CreaturePtrC_t) const {}

    void Condition::PerTurnEffect(
        CreaturePtr_t creaturePtr,
        combat::HitInfoVec_t & hitInfoVec,
        bool & hasTurnBeenConsumed) const
    {
        switch (Which())
        {
            case Conditions::Dazed:
            {
                // if previous condition Dazed the creature, then don't clear
                for (auto const & NEXT_HIT_INFO : hitInfoVec)
                {
                    if (NEXT_HIT_INFO.CondsAddedContains(creature::Conditions::Dazed))
                    {
                        return;
                    }
                }

                if (misc::random::Int(100) < (10 + creaturePtr->Rank().As<int>()))
                {
                    creaturePtr->ConditionRemove(Conditions::Dazed);

                    const combat::ContentAndNamePos CNP(
                        "'s mind clears an is not Dazed anymore.",
                        combat::NamePosition::TargetBefore);

                    hitInfoVec.emplace_back(
                        combat::HitInfo(false, Which(), CNP, 0_health, {}, { Conditions::Dazed }));
                }

                break;
            }
            case Conditions::Tripped:
            {
                // can't spend turn getting up if turn already consumed/spent
                if (hasTurnBeenConsumed)
                {
                    return;
                }

                // if previous condition tripped the creature, then don't get up
                for (auto const & NEXT_HIT_INFO : hitInfoVec)
                {
                    if (NEXT_HIT_INFO.CondsAddedContains(creature::Conditions::Tripped))
                    {
                        return;
                    }
                }

                // don't get up if can't take action
                if (creaturePtr->CanTakeAction() == false)
                {
                    return;
                }

                creaturePtr->ConditionRemove(Conditions::Tripped);

                const combat::ContentAndNamePos CNP(
                    " gets up after being tripped.", combat::NamePosition::TargetBefore);

                hitInfoVec.emplace_back(
                    combat::HitInfo(false, Which(), CNP, 0_health, {}, { Conditions::Tripped }));

                hasTurnBeenConsumed = true;

                break;
            }
            case Conditions::AsleepNatural:
            {
                // if previous condition slept the creature, then don't wake
                for (auto const & NEXT_HIT_INFO : hitInfoVec)
                {
                    if (NEXT_HIT_INFO.CondsAddedContains(creature::Conditions::AsleepNatural)
                        || NEXT_HIT_INFO.CondsAddedContains(creature::Conditions::AsleepMagical))
                    {
                        return;
                    }
                }

                if (misc::random::Int(100) < (15 + creaturePtr->Rank().As<int>()))
                {
                    creaturePtr->ConditionRemove(Conditions::AsleepNatural);

                    const combat::ContentAndNamePos CNP(
                        " wakes from a natural sleep.", combat::NamePosition::TargetBefore);

                    hitInfoVec.emplace_back(combat::HitInfo(
                        false, Which(), CNP, 0_health, {}, { Conditions::AsleepNatural }));
                }

                break;
            }
            case Conditions::Poisoned:
            {
                if (creature::Stats::Test(
                        creaturePtr,
                        stats::Traits::Strength,
                        static_cast<creature::Stats::With>(
                            creature::Stats::With::Luck | creature::Stats::With::RaceRoleBonus))
                    && (misc::random::Int(9) == 0))
                {
                    creature::CondEnumVec_t condsRemoved;

                    combat::FightClub::RemoveAddedCondition(
                        creature::Conditions::Poisoned, creaturePtr, hitInfoVec, condsRemoved);

                    std::ostringstream ss;
                    ss << "The poison has left ";

                    if (creaturePtr->IsPlayerCharacter() == false)
                    {
                        ss << "the ";
                    }

                    auto const CONTENT_NAME_POS{ combat::ContentAndNamePos(
                        ss.str(), "'s body.", "", combat::NamePosition::TargetBefore) };

                    hitInfoVec.emplace_back(combat::HitInfo(
                        false, Which(), CONTENT_NAME_POS, 0_health, {}, condsRemoved));
                }
                else
                {
                    auto const DAMAGE_BASE{ (
                        (creaturePtr->IsPixie()) ? 0 : misc::random::Int(1, 5)) };

                    auto const DAMAGE_RAND_MAX{ std::max(
                        1, static_cast<int>(creaturePtr->HealthNormal().As<float>() * 0.1f)) };

                    auto const DAMAGE_FROM_HEALTH_NORMAL{ misc::random::Int(1, DAMAGE_RAND_MAX) };

                    const Health_t DAMAGE_FINAL{ Health_t(
                        -1 * (DAMAGE_BASE + DAMAGE_FROM_HEALTH_NORMAL)) };

                    CondEnumVec_t condsAddedVec;
                    CondEnumVec_t condsRemovedVec;

                    combat::FightClub::HandleDamage(
                        creaturePtr,
                        hitInfoVec,
                        DAMAGE_FINAL,
                        condsAddedVec,
                        condsRemovedVec,
                        false);

                    std::ostringstream ss;
                    ss << " hurts " << creature::sex::HimHerIt(creaturePtr->Sex(), false) << " for "
                       << DAMAGE_FINAL.Abs() << " damage.";

                    const combat::ContentAndNamePos CNP(
                        "The poison in ", ss.str(), "", combat::NamePosition::TargetBefore);

                    hitInfoVec.emplace_back(combat::HitInfo(
                        true, Which(), CNP, DAMAGE_FINAL, condsAddedVec, condsRemovedVec));
                }

                break;
            }
            case Conditions::Pounced:
            {
                // can't spend turn getting up if turn already consumed/spent
                if (hasTurnBeenConsumed)
                {
                    return;
                }

                // if previous condition tripped the creature, then don't get up
                for (auto const & NEXT_HIT_INFO : hitInfoVec)
                {
                    if (NEXT_HIT_INFO.CondsAddedContains(creature::Conditions::Tripped))
                    {
                        return;
                    }
                }

                // can't get up if can't take action
                if (creaturePtr->CanTakeAction() == false)
                {
                    return;
                }

                if (creature::Stats::Test(
                        creaturePtr,
                        stats::Traits::Strength,
                        static_cast<creature::Stats::With>(
                            creature::Stats::With::Luck | creature::Stats::With::RaceRoleBonus)))
                {
                    creaturePtr->ConditionRemove(Conditions::Pounced);

                    const combat::ContentAndNamePos CNP(
                        " gets up after being pounced on.", combat::NamePosition::TargetBefore);

                    hitInfoVec.emplace_back(combat::HitInfo(
                        false, Which(), CNP, 0_health, {}, { Conditions::Pounced }));

                    hasTurnBeenConsumed = true;
                }
                break;
            }
            case Conditions::AsleepMagical:
            {
                // if previous condition slept the creature, then don't wake
                for (auto const & NEXT_HIT_INFO : hitInfoVec)
                {
                    if (NEXT_HIT_INFO.CondsAddedContains(creature::Conditions::AsleepNatural)
                        || NEXT_HIT_INFO.CondsAddedContains(creature::Conditions::AsleepMagical))
                    {
                        return;
                    }
                }

                if (misc::random::Int(100) < (8 + creaturePtr->Rank().As<int>()))
                {
                    creaturePtr->ConditionRemove(Conditions::AsleepMagical);

                    const combat::ContentAndNamePos CNP(
                        " wakes from a magical sleep.", combat::NamePosition::TargetBefore);

                    hitInfoVec.emplace_back(combat::HitInfo(
                        false, Which(), CNP, 0_health, {}, { Conditions::AsleepMagical }));
                }

                break;
            }
            case Conditions::Good:
            case Conditions::Bold:
            case Conditions::Heroic:
            case Conditions::Daunted:
            case Conditions::Panic:
            case Conditions::Unconscious:
            case Conditions::Stone:
            case Conditions::Dead:
            case Conditions::Count:
            default:
            {
                break;
            }
        }
    }

} // namespace creature
} // namespace heroespath
