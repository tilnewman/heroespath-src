// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// condition.hpp
//
#include "condition.hpp"

#include "combat/creature-interaction.hpp"
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
        const Conditions::Enum TYPE, const bool IS_MAGICAL, const TraitSet & TRAIT_SET)
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

        const auto TRAIT_STR { traitSet_.ToString(false, false, false, true) };
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

        const auto TRAIT_STR { traitSet_.ToString(false, false, false, true) };
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
    void Condition::InitialChange(const CreaturePtr_t) const {}

    void Condition::FinalChange(const CreaturePtr_t) const {}

    void Condition::PerTurnEffect(
        const combat::CreatureInteraction & CREATURE_INTERACTION,
        const CreaturePtr_t CREATURE_PTR,
        combat::HitInfoVec_t & hitInfoVec,
        bool & hasTurnBeenConsumed) const
    {
        switch (Which())
        {
            case Conditions::Dazed:
            {
                // if previous condition Dazed the creature, then don't clear
                for (const auto & NEXT_HIT_INFO : hitInfoVec)
                {
                    if (NEXT_HIT_INFO.CondsAddedContains(creature::Conditions::Dazed))
                    {
                        return;
                    }
                }

                if (misc::Random(100) < (10 + CREATURE_PTR->Rank().Get()))
                {
                    CREATURE_PTR->ConditionRemove(Conditions::Dazed);

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
                for (const auto & NEXT_HIT_INFO : hitInfoVec)
                {
                    if (NEXT_HIT_INFO.CondsAddedContains(creature::Conditions::Tripped))
                    {
                        return;
                    }
                }

                // don't get up if can't take action
                if (CREATURE_PTR->CanTakeAction() == false)
                {
                    return;
                }

                CREATURE_PTR->ConditionRemove(Conditions::Tripped);

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
                for (const auto & NEXT_HIT_INFO : hitInfoVec)
                {
                    if (NEXT_HIT_INFO.CondsAddedContains(creature::Conditions::AsleepNatural)
                        || NEXT_HIT_INFO.CondsAddedContains(creature::Conditions::AsleepMagical))
                    {
                        return;
                    }
                }

                if (misc::Random(100) < (15 + CREATURE_PTR->Rank().GetAs<int>()))
                {
                    CREATURE_PTR->ConditionRemove(Conditions::AsleepNatural);

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
                        CREATURE_PTR,
                        Traits::Strength,
                        static_cast<creature::Stats::With>(
                            creature::Stats::With::Luck | creature::Stats::With::RaceRoleBonus))
                    && (misc::Random(9) == 0))
                {
                    creature::CondEnumVec_t condsRemoved;

                    CREATURE_INTERACTION.RemoveAddedCondition(
                        creature::Conditions::Poisoned, CREATURE_PTR, hitInfoVec, condsRemoved);

                    std::ostringstream ss;
                    ss << "The poison has left ";

                    if (CREATURE_PTR->IsPlayerCharacter() == false)
                    {
                        ss << "the ";
                    }

                    const auto CONTENT_NAME_POS { combat::ContentAndNamePos(
                        ss.str(), "'s body.", "", combat::NamePosition::TargetBefore) };

                    hitInfoVec.emplace_back(combat::HitInfo(
                        false, Which(), CONTENT_NAME_POS, 0_health, {}, condsRemoved));
                }
                else
                {
                    const auto DAMAGE_BASE { Health_t::Make(
                        (CREATURE_PTR->IsPixie()) ? 0 : misc::Random(1, 5)) };

                    const auto DAMAGE_RAND_MAX { Health_t::Make(
                        misc::Max(1.0f, (CREATURE_PTR->HealthNormal().GetAs<float>() * 0.1f))) };

                    const auto DAMAGE_FROM_HEALTH_NORMAL { misc::Random(
                        1_health, DAMAGE_RAND_MAX) };

                    const Health_t DAMAGE_FINAL { -(DAMAGE_BASE + DAMAGE_FROM_HEALTH_NORMAL) };

                    CondEnumVec_t condsAddedVec;
                    CondEnumVec_t condsRemovedVec;

                    CREATURE_INTERACTION.HandleDamage(
                        CREATURE_PTR,
                        hitInfoVec,
                        DAMAGE_FINAL,
                        condsAddedVec,
                        condsRemovedVec,
                        false);

                    std::ostringstream ss;
                    ss << " hurts " << creature::sex::HimHerIt(CREATURE_PTR->Sex(), false)
                       << " for " << DAMAGE_FINAL.MakePositiveCopy() << " damage.";

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
                for (const auto & NEXT_HIT_INFO : hitInfoVec)
                {
                    if (NEXT_HIT_INFO.CondsAddedContains(creature::Conditions::Tripped))
                    {
                        return;
                    }
                }

                // can't get up if can't take action
                if (CREATURE_PTR->CanTakeAction() == false)
                {
                    return;
                }

                if (creature::Stats::Test(
                        CREATURE_PTR,
                        Traits::Strength,
                        static_cast<creature::Stats::With>(
                            creature::Stats::With::Luck | creature::Stats::With::RaceRoleBonus)))
                {
                    CREATURE_PTR->ConditionRemove(Conditions::Pounced);

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
                for (const auto & NEXT_HIT_INFO : hitInfoVec)
                {
                    if (NEXT_HIT_INFO.CondsAddedContains(creature::Conditions::AsleepNatural)
                        || NEXT_HIT_INFO.CondsAddedContains(creature::Conditions::AsleepMagical))
                    {
                        return;
                    }
                }

                if (misc::Random(100) < (8 + CREATURE_PTR->Rank().Get()))
                {
                    CREATURE_PTR->ConditionRemove(Conditions::AsleepMagical);

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
            default: { break;
            }
        }
    }

} // namespace creature
} // namespace heroespath
