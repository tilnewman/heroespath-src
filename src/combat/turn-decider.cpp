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
// turn-decider.cpp
//
#include "turn-decider.hpp"

#include "combat/combat-display.hpp"
#include "combat/encounter.hpp"
#include "combat/turn-info.hpp"
#include "creature/algorithms.hpp"
#include "creature/creature.hpp"
#include "game/game-data-file.hpp"
#include "game/phase-enum.hpp"
#include "log/log-macros.hpp"
#include "misc/random.hpp"
#include "misc/vectors.hpp"
#include "spell/spell.hpp"

#include <algorithm>
#include <sstream>
#include <tuple>
#include <vector>

namespace heroespath
{
namespace combat
{

    const EffectTypeVec_t TurnDecider::HARM_EFFECTTYPES_VEC_{
        combat::EffectType::CreatureHarmDamage,
        combat::EffectType::CreatureHarmMisc,
        combat::EffectType::ItemHarmBreak,
        combat::EffectType::ItemHarmMisc
    };

    const EffectTypeVec_t TurnDecider::HELP_EFFECTTYPES_VEC_{ combat::EffectType::CreatureHelpHeal,
                                                              combat::EffectType::CreatureHelpMisc,
                                                              combat::EffectType::ItemHelpFix,
                                                              combat::EffectType::ItemHelpMisc };

    const TurnActionInfo TurnDecider::Decide(
        const creature::CreaturePtr_t CREATURE_DECIDING_PTR,
        CombatDisplayCPtrC_t COMBAT_DISPLAY_CPTRC)
    {
        // only do 'Nothing' if can't take any action
        if (CREATURE_DECIDING_PTR->CanTakeAction() == false)
        {
            return TurnActionInfo(TurnAction::Nothing);
        }

        auto const TURN_INFO{ Encounter::Instance()->GetTurnInfoCopy(CREATURE_DECIDING_PTR) };

        // find out if any possible targets (players) are holding projectile weapons
        auto const ARE_ANY_HOLDING_PROJECTILE_WEAPONS{ []() {
            auto const ALL_LIVING_PLAYERS_PVEC{ creature::Algorithms::Players(
                creature::Algorithms::Living) };

            for (auto const & NEXT_LIVING_PLAYER_PTR : ALL_LIVING_PLAYERS_PVEC)
            {
                if (NEXT_LIVING_PLAYER_PTR->IsHoldingProjectileWeapon())
                {
                    return true;
                }
            }
            return false;
        }() };

        // find out if any possible targets (players) are flying
        auto const ARE_ANY_FLYING{ []() {
            auto const ALL_LIVING_PLAYERS_PVEC{ creature::Algorithms::Players(
                creature::Algorithms::Living) };

            for (auto const & NEXT_LIVING_PLAYER_PTR : ALL_LIVING_PLAYERS_PVEC)
            {
                if (Encounter::Instance()->GetTurnInfoCopy(NEXT_LIVING_PLAYER_PTR).GetIsFlying())
                {
                    return true;
                }
            }
            return false;
        }() };

        // If flying but no opponents are flying and no opponents are holding projectile weapons,
        // then this enemy has to land so that combat does not become deadlocked.
        if ((Encounter::Instance()->GetTurnInfoCopy(CREATURE_DECIDING_PTR).GetIsFlying())
            && (ARE_ANY_FLYING == false) && (ARE_ANY_HOLDING_PROJECTILE_WEAPONS == false))
        {
            return TurnActionInfo(TurnAction::Land);
        }
        else
        {
            // decide if flying
            auto const FLY_TURN_ACTION_INFO{ DecideIfFlying(TURN_INFO, CREATURE_DECIDING_PTR) };
            if (FLY_TURN_ACTION_INFO.Action() != TurnAction::Nothing)
            {
                return FLY_TURN_ACTION_INFO;
            }
        }

        auto const LIVING_PLAYERS_IN_ATTACK_RANGE{ [&]() {
            creature::CreaturePVec_t livingPlayersInAttackRangePVec;
            COMBAT_DISPLAY_CPTRC->FindCreaturesThatCanBeAttackedOfType(
                livingPlayersInAttackRangePVec, CREATURE_DECIDING_PTR, true);
            return livingPlayersInAttackRangePVec;
        }() };

        creature::CreaturePVec_t fellowEnemiesInMeleeRangePVec;
        COMBAT_DISPLAY_CPTRC->FindCreaturesAllRoundOfType(
            fellowEnemiesInMeleeRangePVec, CREATURE_DECIDING_PTR, false, true);

        // decide if running away (retreating), if cowardly or wary and outnumbered
        auto const RETREAT_TURN_ACTION_INFO{ DecideIfRetreating(
            TURN_INFO,
            CREATURE_DECIDING_PTR,
            COMBAT_DISPLAY_CPTRC,
            LIVING_PLAYERS_IN_ATTACK_RANGE.size(),
            fellowEnemiesInMeleeRangePVec.size()) };

        if (RETREAT_TURN_ACTION_INFO.Action() != TurnAction::Nothing)
        {
            return RETREAT_TURN_ACTION_INFO;
        }

        const creature::CreaturePtr_t MOST_DESIRED_TARGET_PTR{ FindMostDesiredTarget(
            TURN_INFO, CREATURE_DECIDING_PTR, COMBAT_DISPLAY_CPTRC) };

        // decide if casting a spell
        auto const CASTING_TURN_ACTION_INFO{ DecideIfCasting(
            TURN_INFO, CREATURE_DECIDING_PTR, MOST_DESIRED_TARGET_PTR) };

        if ((CASTING_TURN_ACTION_INFO.Action() != TurnAction::Nothing)
            && CASTING_TURN_ACTION_INFO.Spell())
        {
            return CASTING_TURN_ACTION_INFO;
        }

        auto const MOST_DESIRED_TARGET_DISTANCE{ COMBAT_DISPLAY_CPTRC->GetBlockingDistanceBetween(
            CREATURE_DECIDING_PTR, MOST_DESIRED_TARGET_PTR) };

        auto const IS_FLYING{ TURN_INFO.GetIsFlying() };

        auto const DOES_FLYING_SEPARATE_MOST_DESIRED_TARGET{ !(
            IS_FLYING
            == Encounter::Instance()->GetTurnInfoCopy(MOST_DESIRED_TARGET_PTR).GetIsFlying()) };

        auto const CAN_ATTACK_ON_RANGE{ (
            (std::abs(MOST_DESIRED_TARGET_DISTANCE) <= 1)
            || CREATURE_DECIDING_PTR->IsHoldingProjectileWeapon() || IS_FLYING) };

        auto const CAN_ATTACK_ON_FLIGHT{ (
            (DOES_FLYING_SEPARATE_MOST_DESIRED_TARGET == false)
            || CREATURE_DECIDING_PTR->IsHoldingProjectileWeapon() || IS_FLYING) };

        auto const CAN_ATTACK_MOST_DESIRED_TARGET_WITH{ CREATURE_DECIDING_PTR->HasWeaponsHeld()
                                                        && CAN_ATTACK_ON_RANGE
                                                        && CAN_ATTACK_ON_FLIGHT };

        // decide if moving toward most desired target, which technically could mean Advancing or
        // Retreating...
        auto const MOVE_TOWARD_TURN_ACTION_INFO{ DecideIfMovingTowardsMostDesiredTarget(
            TURN_INFO,
            CREATURE_DECIDING_PTR,
            COMBAT_DISPLAY_CPTRC,
            MOST_DESIRED_TARGET_DISTANCE,
            LIVING_PLAYERS_IN_ATTACK_RANGE.size(),
            CAN_ATTACK_MOST_DESIRED_TARGET_WITH) };

        if (MOVE_TOWARD_TURN_ACTION_INFO.Action() != TurnAction::Nothing)
        {
            return MOVE_TOWARD_TURN_ACTION_INFO;
        }

        // decide if taking beast specific actions (roar, fly, pounce, etc.)
        if (CREATURE_DECIDING_PTR->IsBeast())
        {
            auto const BEAST_TURN_ACTION_INFO{ DecideIfDoingBeastAction(
                TURN_INFO,
                CREATURE_DECIDING_PTR,
                MOST_DESIRED_TARGET_PTR,
                LIVING_PLAYERS_IN_ATTACK_RANGE,
                MOST_DESIRED_TARGET_DISTANCE) };

            if (BEAST_TURN_ACTION_INFO.Action() != TurnAction::Nothing)
            {
                return BEAST_TURN_ACTION_INFO;
            }
        }

        // attack most desired target if possible
        if (CAN_ATTACK_MOST_DESIRED_TARGET_WITH)
        {
            return TurnActionInfo(TurnAction::Attack, { MOST_DESIRED_TARGET_PTR });
        }

        // if no players are in range to attack but there are held weapons ready to attack and other
        // fellow creatures ahead, then move toward the closest player
        if (LIVING_PLAYERS_IN_ATTACK_RANGE.empty() && (CREATURE_DECIDING_PTR->HasWeaponsHeld())
            && (TURN_INFO.GetStrategyInfo().Advance() != strategy::AdvanceType::Cowardly)
            && (COMBAT_DISPLAY_CPTRC->CanAdvanceOrRetreat(CREATURE_DECIDING_PTR, true).empty()))
        {
            creature::CreaturePVec_t creaturesAheadPVec;
            COMBAT_DISPLAY_CPTRC->FindCreaturesAtBlockingPosOfType(
                creaturesAheadPVec,
                COMBAT_DISPLAY_CPTRC->FindBlockingPos(CREATURE_DECIDING_PTR) - 1,
                true);

            if (creaturesAheadPVec.empty() == false)
            {
                auto const DISTANCE_TO_CLOSEST_PLAYER{
                    COMBAT_DISPLAY_CPTRC->GetClosestBlockingDistanceByType(
                        CREATURE_DECIDING_PTR, true)
                };

                if (DISTANCE_TO_CLOSEST_PLAYER < 0)
                {
                    return TurnActionInfo(TurnAction::Advance);
                }
                else if (DISTANCE_TO_CLOSEST_PLAYER > 0)
                {
                    return TurnActionInfo(TurnAction::Retreat);
                }
            }
        }

        // cast an attack spell if able
        if (CREATURE_DECIDING_PTR->CanCastSpellByEffectType(HARM_EFFECTTYPES_VEC_))
        {
            return DecideSpell(
                CREATURE_DECIDING_PTR, MOST_DESIRED_TARGET_PTR, HARM_EFFECTTYPES_VEC_);
        }

        // cast any misc spell if able
        if (CREATURE_DECIDING_PTR->CanCastSpells())
        {
            auto const SPELL_CAST_TURN_ACTION_INFO{ ForcePickSpellToCast(
                CREATURE_DECIDING_PTR, MOST_DESIRED_TARGET_PTR) };

            if ((SPELL_CAST_TURN_ACTION_INFO.Action() != TurnAction::Nothing)
                && SPELL_CAST_TURN_ACTION_INFO.Spell())
            {
                return SPELL_CAST_TURN_ACTION_INFO;
            }
        }

        // change weapons if needed
        if ((LIVING_PLAYERS_IN_ATTACK_RANGE.empty() == false)
            && (CREATURE_DECIDING_PTR->HasWeaponsHeld() == false)
            && (CREATURE_DECIDING_PTR->HasWeapons() == true))
        {
            return TurnActionInfo(TurnAction::ChangeWeapon);
        }

        // see if there is a player to advance towards who is not the most desired target
        auto const IF_ADVANCING_TURNACTIONINFO{ DecideIfAdvancingTowardNonMostDesiredTarget(
            TURN_INFO, CREATURE_DECIDING_PTR, COMBAT_DISPLAY_CPTRC) };
        //
        if (IF_ADVANCING_TURNACTIONINFO.Action() != TurnAction::Nothing)
        {
            return IF_ADVANCING_TURNACTIONINFO;
        }

        // Okay, so we are reaching this end point in the logic tree much more
        // often than anticipated, so try and attack here if possible, ignoring
        // the most desired target.
        if (CREATURE_DECIDING_PTR->HasWeaponsHeld()
            && (LIVING_PLAYERS_IN_ATTACK_RANGE.empty() == false))
        {
            creature::CreaturePVec_t desiredTargetsInRangePVec{ LIVING_PLAYERS_IN_ATTACK_RANGE };
            AdjustCreatueVecForMurderousIntent(TURN_INFO, desiredTargetsInRangePVec);

            if (desiredTargetsInRangePVec.empty() == false)
            {
                M_HP_LOG_WRN(
                    "combat::TurnDecider::Decide("
                    << CREATURE_DECIDING_PTR->NameAndRaceAndRole()
                    << ") forced to take last-chance attack combat option.");

                return TurnActionInfo(
                    TurnAction::Attack,
                    { misc::Vector::SelectRandom(LIVING_PLAYERS_IN_ATTACK_RANGE) });
            }
        }

        // At this point nothing was chosen to be done, so block instead
        return TurnActionInfo(TurnAction::Block);
    }

    creature::CreaturePtr_t TurnDecider::FindMostDesiredTarget(
        const TurnInfo & CREATURE_DECIDING_TURN_INFO,
        const creature::CreaturePtr_t CREATURE_DECIDING_PTR,
        CombatDisplayCPtrC_t COMBAT_DISPLAY_CPTRC)
    {
        // pick (select) favorite targets by type
        const creature::CreaturePVec_t SELECT_TARGETS_PVEC{ FindSelectedTargets(
            CREATURE_DECIDING_TURN_INFO) };

        // eliminate (refine) targets by conditions/AI/etc.
        const creature::CreaturePVec_t REFINED_TARGETS_PVEC{ RefineTargets(
            SELECT_TARGETS_PVEC, CREATURE_DECIDING_TURN_INFO) };

        auto const IS_FLYING{ CREATURE_DECIDING_TURN_INFO.GetIsFlying() };

        if (REFINED_TARGETS_PVEC.empty() == false)
        {
            if (IS_FLYING == false)
            {
                auto const REFINED_NOTFLYING_TARGETS_PVEC{ creature::Algorithms::FindByIsFlying(
                    REFINED_TARGETS_PVEC, creature::Algorithms::CriteriaOpt::DoesNotMeet) };

                if (REFINED_NOTFLYING_TARGETS_PVEC.empty() == false)
                {
                    return misc::Vector::SelectRandom(REFINED_NOTFLYING_TARGETS_PVEC);
                }
            }

            return misc::Vector::SelectRandom(COMBAT_DISPLAY_CPTRC->FindClosestLiving(
                CREATURE_DECIDING_PTR, REFINED_TARGETS_PVEC));
        }

        if (SELECT_TARGETS_PVEC.empty() == false)
        {
            if (IS_FLYING == false)
            {
                auto const SELECT_NOTFLYING_TARGETS_PVEC{ creature::Algorithms::FindByIsFlying(
                    SELECT_TARGETS_PVEC, creature::Algorithms::CriteriaOpt::DoesNotMeet) };

                if (SELECT_NOTFLYING_TARGETS_PVEC.empty() == false)
                {
                    return misc::Vector::SelectRandom(SELECT_NOTFLYING_TARGETS_PVEC);
                }
            }

            // select the closest of the select targets at random
            return misc::Vector::SelectRandom(COMBAT_DISPLAY_CPTRC->FindClosestLiving(
                CREATURE_DECIDING_PTR, SELECT_TARGETS_PVEC));
        }

        auto const ALL_LIVING_PLAYERS_PVEC{ creature::Algorithms::Players(
            creature::Algorithms::Living) };

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ALL_LIVING_PLAYERS_PVEC.empty() == false),
            "combat::TurnDecider::FindMostDesiredTarget("
                << CREATURE_DECIDING_PTR->NameAndRaceAndRole()
                << ") found no living player characters.");

        // prefer players still a threat...
        auto const ALL_LIVING_THREAT_PLAYERS_PVEC{
            creature::Algorithms::FindByConditionMeaningNotAThreatPermenantly(
                ALL_LIVING_PLAYERS_PVEC, creature::Algorithms::CondSingleOpt::DoesNotHave)
        };

        //...but accept those 'not a threat' if no other choice
        auto const AVAILABLE_TARGETS_PRE_PVEC{ (
            (ALL_LIVING_THREAT_PLAYERS_PVEC.empty()) ? ALL_LIVING_PLAYERS_PVEC
                                                     : ALL_LIVING_THREAT_PLAYERS_PVEC) };

        creature::CreaturePVec_t availableTargetsPVec{ AVAILABLE_TARGETS_PRE_PVEC };

        AdjustCreatueVecForMurderousIntent(CREATURE_DECIDING_TURN_INFO, availableTargetsPVec);

        // prefer those reachable considering flying...
        auto const REACHABLE_AVAILABLE_TARGETS_PVEC{ (
            (IS_FLYING)
                ? availableTargetsPVec
                : creature::Algorithms::FindByIsFlying(
                      availableTargetsPVec, creature::Algorithms::CriteriaOpt::DoesNotMeet)) };

        //...but accept those 'not reachable' if no other choice
        auto const POSSIBLE_TARGETS_PVEC{ (
            (REACHABLE_AVAILABLE_TARGETS_PVEC.empty()) ? availableTargetsPVec
                                                       : REACHABLE_AVAILABLE_TARGETS_PVEC) };

        // prefer those closest
        auto const CLOSEST_POSSIBLE_TARGETS_PVEC{ COMBAT_DISPLAY_CPTRC->FindClosestLiving(
            CREATURE_DECIDING_PTR, POSSIBLE_TARGETS_PVEC) };

        M_ASSERT_OR_LOGANDTHROW_SS(
            (CLOSEST_POSSIBLE_TARGETS_PVEC.empty() == false),
            "combat::TurnDecider::FindMostDesiredTarget("
                << CREATURE_DECIDING_PTR->NameAndRaceAndRole()
                << ") FindClosestLiving() returned an empty vec.");

        return misc::Vector::SelectRandom(CLOSEST_POSSIBLE_TARGETS_PVEC);
    }

    creature::CreaturePVec_t TurnDecider::FindSelectedTargets(const TurnInfo & TURN_INFO)
    {
        auto const SELECT_TYPE_ENUM{ TURN_INFO.GetStrategyInfo().Select() };

        if (SELECT_TYPE_ENUM == strategy::SelectType::None)
        {
            return {};
        }

        auto const ALL_LIVING_PLAYERS_PVEC{ creature::Algorithms::Players(
            creature::Algorithms::Living) };

        auto const SELECTABLE_PLAYERS_PVEC{ (
            (TURN_INFO.GetIsFlying())
                ? ALL_LIVING_PLAYERS_PVEC
                : creature::Algorithms::FindByIsFlying(
                      ALL_LIVING_PLAYERS_PVEC, creature::Algorithms::CriteriaOpt::DoesNotMeet)) };

        if (SELECTABLE_PLAYERS_PVEC.empty())
        {
            return {};
        }

        creature::CreaturePVec_t selectedPlayersPVec;

        // add target selections by race
        if (SELECT_TYPE_ENUM & strategy::SelectType::Dragon)
        {
            misc::Vector::Append(
                creature::Algorithms::FindByRace(SELECTABLE_PLAYERS_PVEC, creature::race::Dragon),
                selectedPlayersPVec);
        }

        if (SELECT_TYPE_ENUM & strategy::SelectType::Gnome)
        {
            misc::Vector::Append(
                creature::Algorithms::FindByRace(SELECTABLE_PLAYERS_PVEC, creature::race::Gnome),
                selectedPlayersPVec);
        }

        if (SELECT_TYPE_ENUM & strategy::SelectType::Human)
        {
            misc::Vector::Append(
                creature::Algorithms::FindByRace(SELECTABLE_PLAYERS_PVEC, creature::race::Human),
                selectedPlayersPVec);
        }

        if (SELECT_TYPE_ENUM & strategy::SelectType::Pixie)
        {
            misc::Vector::Append(
                creature::Algorithms::FindByRace(SELECTABLE_PLAYERS_PVEC, creature::race::Pixie),
                selectedPlayersPVec);
        }

        if (SELECT_TYPE_ENUM & strategy::SelectType::Wolfen)
        {
            misc::Vector::Append(
                creature::Algorithms::FindByRace(SELECTABLE_PLAYERS_PVEC, creature::race::Wolfen),
                selectedPlayersPVec);
        }

        // add target selections by role
        if (SELECT_TYPE_ENUM & strategy::SelectType::Archer)
        {
            misc::Vector::Append(
                creature::Algorithms::FindByRole(SELECTABLE_PLAYERS_PVEC, creature::role::Archer),
                selectedPlayersPVec);
        }

        if (SELECT_TYPE_ENUM & strategy::SelectType::Bard)
        {
            misc::Vector::Append(
                creature::Algorithms::FindByRole(SELECTABLE_PLAYERS_PVEC, creature::role::Bard),
                selectedPlayersPVec);
        }

        if (SELECT_TYPE_ENUM & strategy::SelectType::Beastmaster)
        {
            misc::Vector::Append(
                creature::Algorithms::FindByRole(
                    SELECTABLE_PLAYERS_PVEC, creature::role::Beastmaster),
                selectedPlayersPVec);
        }

        if (SELECT_TYPE_ENUM & strategy::SelectType::Cleric)
        {
            misc::Vector::Append(
                creature::Algorithms::FindByRole(SELECTABLE_PLAYERS_PVEC, creature::role::Cleric),
                selectedPlayersPVec);
        }

        if (SELECT_TYPE_ENUM & strategy::SelectType::FireBrand)
        {
            misc::Vector::Append(
                creature::Algorithms::FindByRole(
                    SELECTABLE_PLAYERS_PVEC, creature::role::Firebrand),
                selectedPlayersPVec);
        }

        if (SELECT_TYPE_ENUM & strategy::SelectType::Knight)
        {
            misc::Vector::Append(
                creature::Algorithms::FindByRole(SELECTABLE_PLAYERS_PVEC, creature::role::Knight),
                selectedPlayersPVec);
        }

        if (SELECT_TYPE_ENUM & strategy::SelectType::Sorcerer)
        {
            misc::Vector::Append(
                creature::Algorithms::FindByRole(SELECTABLE_PLAYERS_PVEC, creature::role::Sorcerer),
                selectedPlayersPVec);
        }

        if (SELECT_TYPE_ENUM & strategy::SelectType::Sylavin)
        {
            misc::Vector::Append(
                creature::Algorithms::FindByRole(SELECTABLE_PLAYERS_PVEC, creature::role::Sylavin),
                selectedPlayersPVec);
        }

        if (SELECT_TYPE_ENUM & strategy::SelectType::Theif)
        {
            misc::Vector::Append(
                creature::Algorithms::FindByRole(SELECTABLE_PLAYERS_PVEC, creature::role::Thief),
                selectedPlayersPVec);
        }

        // add target selections by misc types
        if (SELECT_TYPE_ENUM & strategy::SelectType::Beast)
        {
            misc::Vector::Append(
                creature::Algorithms::FindByIsBeast(SELECTABLE_PLAYERS_PVEC), selectedPlayersPVec);
        }

        if (SELECT_TYPE_ENUM & strategy::SelectType::NotBeast)
        {
            misc::Vector::Append(
                creature::Algorithms::FindByIsBeast(
                    SELECTABLE_PLAYERS_PVEC, creature::Algorithms::CriteriaOpt::DoesNotMeet),
                selectedPlayersPVec);
        }

        if (SELECT_TYPE_ENUM & strategy::SelectType::CanFly)
        {
            misc::Vector::Append(
                creature::Algorithms::FindByCanFly(SELECTABLE_PLAYERS_PVEC), selectedPlayersPVec);
        }

        if (SELECT_TYPE_ENUM & strategy::SelectType::CantFly)
        {
            misc::Vector::Append(
                creature::Algorithms::FindByCanFly(
                    SELECTABLE_PLAYERS_PVEC, creature::Algorithms::CriteriaOpt::DoesNotMeet),
                selectedPlayersPVec);
        }

        if (SELECT_TYPE_ENUM & strategy::SelectType::Caster)
        {
            misc::Vector::Append(
                creature::Algorithms::FindByCanCast(SELECTABLE_PLAYERS_PVEC), selectedPlayersPVec);
        }

        if (SELECT_TYPE_ENUM & strategy::SelectType::Projectile)
        {
            misc::Vector::Append(
                creature::Algorithms::FindByIsHoldingProjWeapon(SELECTABLE_PLAYERS_PVEC),
                selectedPlayersPVec);
        }

        if (selectedPlayersPVec.size() > 1)
        {
            std::sort(selectedPlayersPVec.begin(), selectedPlayersPVec.end());
            selectedPlayersPVec.erase(
                std::unique(selectedPlayersPVec.begin(), selectedPlayersPVec.end()),
                selectedPlayersPVec.end());
        }

        return creature::Algorithms::FindByConditionMeaningNotAThreatPermenantly(
            selectedPlayersPVec, creature::Algorithms::CondSingleOpt::DoesNotHave);
    }

    creature::CreaturePVec_t TurnDecider::RefineTargets(
        const creature::CreaturePVec_t & SELECTED_TARGETS_PVEC, const TurnInfo & TURN_INFO)
    {
        auto const REFINE_TYPE_ENUM{ TURN_INFO.GetStrategyInfo().Refine() };

        if (SELECTED_TARGETS_PVEC.empty() || (REFINE_TYPE_ENUM == strategy::RefineType::None))
        {
            return SELECTED_TARGETS_PVEC;
        }

        creature::CreaturePVec_t refinedTargetsPVec;

        if (REFINE_TYPE_ENUM & strategy::RefineType::Murderer)
        {
            std::copy_if(
                SELECTED_TARGETS_PVEC.begin(),
                SELECTED_TARGETS_PVEC.end(),
                back_inserter(refinedTargetsPVec),
                [](const creature::CreaturePtr_t CCPTR) {
                    return CCPTR->HasCondition(creature::Conditions::Unconscious);
                });
        }

        if (REFINE_TYPE_ENUM & strategy::RefineType::Bloodthirsty)
        {
            auto const LOWEST_HEALTH_PVEC{ creature::Algorithms::FindLowestHealth(
                SELECTED_TARGETS_PVEC) };

            std::copy(
                LOWEST_HEALTH_PVEC.begin(),
                LOWEST_HEALTH_PVEC.end(),
                back_inserter(refinedTargetsPVec));
        }

        if (REFINE_TYPE_ENUM & strategy::RefineType::Coward)
        {
            creature::CreaturePVec_t cantTakeActionPVec;

            std::copy_if(
                SELECTED_TARGETS_PVEC.begin(),
                SELECTED_TARGETS_PVEC.end(),
                back_inserter(cantTakeActionPVec),
                [](const creature::CreaturePtr_t CCPTR) {
                    return (CCPTR->CanTakeAction() == false);
                });

            if (cantTakeActionPVec.empty() == false)
            {
                auto const CANT_ACT_BUT_NOT_PERM_PVEC{
                    creature::Algorithms::FindByConditionMeaningNotAThreatPermenantly(
                        cantTakeActionPVec, creature::Algorithms::CondSingleOpt::DoesNotHave)
                };

                if (CANT_ACT_BUT_NOT_PERM_PVEC.empty() == false)
                {
                    std::copy(
                        CANT_ACT_BUT_NOT_PERM_PVEC.begin(),
                        CANT_ACT_BUT_NOT_PERM_PVEC.end(),
                        back_inserter(refinedTargetsPVec));
                }
            }
        }

        if (REFINE_TYPE_ENUM & strategy::RefineType::Hit)
        {
            if (REFINE_TYPE_ENUM & strategy::RefineType::LastTo)
            {
                if (TURN_INFO.GetLastHitByCreaturePtrOpt())
                {
                    refinedTargetsPVec.emplace_back(TURN_INFO.GetLastHitByCreaturePtrOpt().value());
                }
            }
            else
            {
                if (TURN_INFO.GetFirstHitByCreaturePtrOpt())
                {
                    refinedTargetsPVec.emplace_back(
                        TURN_INFO.GetFirstHitByCreaturePtrOpt().value());
                }
            }
        }

        if (REFINE_TYPE_ENUM & strategy::RefineType::Attack)
        {
            if (REFINE_TYPE_ENUM & strategy::RefineType::LastTo)
            {
                if (TURN_INFO.GetLastAttackedByCreaturePtrOpt())
                {
                    refinedTargetsPVec.emplace_back(
                        TURN_INFO.GetLastAttackedByCreaturePtrOpt().value());
                }
            }
            else
            {
                if (TURN_INFO.GetFirstAttackedByCreaturePtrOpt())
                {
                    refinedTargetsPVec.emplace_back(
                        TURN_INFO.GetFirstAttackedByCreaturePtrOpt().value());
                }
            }
        }

        if (REFINE_TYPE_ENUM & strategy::RefineType::MusicMaker)
        {
            if (REFINE_TYPE_ENUM & strategy::RefineType::LastTo)
            {
                if (TURN_INFO.GetLastToMakeMusicCreaturePtrOpt())
                {
                    refinedTargetsPVec.emplace_back(
                        TURN_INFO.GetLastToMakeMusicCreaturePtrOpt().value());
                }
            }
            else
            {
                if (TURN_INFO.GetFirstToMakeMusicCreaturePtrOpt())
                {
                    refinedTargetsPVec.emplace_back(
                        TURN_INFO.GetFirstToMakeMusicCreaturePtrOpt().value());
                }
            }
        }

        if (REFINE_TYPE_ENUM & strategy::RefineType::Caster)
        {
            if (REFINE_TYPE_ENUM & strategy::RefineType::LastTo)
            {
                if (TURN_INFO.GetLastToCastCreaturePtrOpt())
                {
                    refinedTargetsPVec.emplace_back(
                        TURN_INFO.GetLastToCastCreaturePtrOpt().value());
                }
            }
            else
            {
                if (TURN_INFO.GetFirstToCastCreaturePtrOpt())
                {
                    refinedTargetsPVec.emplace_back(
                        TURN_INFO.GetFirstToCastCreaturePtrOpt().value());
                }
            }
        }

        if (REFINE_TYPE_ENUM & strategy::RefineType::Enchanted)
        {
            std::copy_if(
                SELECTED_TARGETS_PVEC.begin(),
                SELECTED_TARGETS_PVEC.end(),
                back_inserter(refinedTargetsPVec),
                [](const creature::CreaturePtr_t CCPTR) { return CCPTR->HasMagicalCondition(); });
        }

        if (REFINE_TYPE_ENUM & strategy::RefineType::NotEnchanted)
        {
            std::copy_if(
                SELECTED_TARGETS_PVEC.begin(),
                SELECTED_TARGETS_PVEC.end(),
                back_inserter(refinedTargetsPVec),
                [](const creature::CreaturePtr_t CCPTR) {
                    return (CCPTR->HasMagicalCondition() == false);
                });
        }

        if (REFINE_TYPE_ENUM & strategy::RefineType::MostDamage)
        {
            auto const CREATURE_PTR_OPT{ TURN_INFO.MostDamagedCreaturePtrOpt() };
            if (CREATURE_PTR_OPT)
            {
                refinedTargetsPVec.emplace_back(CREATURE_PTR_OPT.value());
            }
        }

        if (refinedTargetsPVec.size() > 1)
        {
            std::sort(refinedTargetsPVec.begin(), refinedTargetsPVec.end());

            refinedTargetsPVec.erase(
                std::unique(refinedTargetsPVec.begin(), refinedTargetsPVec.end()),
                refinedTargetsPVec.end());
        }

        return refinedTargetsPVec;
    }

    float TurnDecider::ChanceFromFrequency(const strategy::FrequencyType::Enum E)
    {
        if (E == strategy::FrequencyType::Commonly)
        {
            return 0.75f;
        }
        else if (E == strategy::FrequencyType::Neutral)
        {
            return 0.5f;
        }
        else if (E == strategy::FrequencyType::Often)
        {
            return 0.9f;
        }
        else if (E == strategy::FrequencyType::Rarely)
        {
            return 0.1f;
        }
        else if (E == strategy::FrequencyType::Seldom)
        {
            return 0.25f;
        }
        else if (E == strategy::FrequencyType::Always)
        {
            return 1.0f;
        }
        else // Never & everything else
        {
            return 0.0f;
        }
    }

    const TurnActionInfo TurnDecider::DecideIfRetreating(
        const TurnInfo & TURN_INFO,
        const creature::CreaturePtr_t CREATURE_DECIDING_PTR,
        CombatDisplayCPtrC_t COMBAT_DISPLAY_CPTRC,
        const std::size_t NUM_PLAYERS_IN_MELEE_RANGE,
        const std::size_t NUM_FELLOWS_IN_MELEE_RANGE)
    {
        if (COMBAT_DISPLAY_CPTRC->CanAdvanceOrRetreat(CREATURE_DECIDING_PTR, false).empty())
        {
            // separate chance to retreat if daunted
            if ((CREATURE_DECIDING_PTR->HasCondition(creature::Conditions::Daunted)
                 && (misc::random::Float() < game::GameDataFile::Instance()->GetCopyFloat(
                                                 "heroespath-fight-chance-daunted-will-retreat"))))
            {
                return TurnActionInfo(TurnAction::Retreat);
            }

            // separate chance to retreat if panicked
            if ((CREATURE_DECIDING_PTR->HasCondition(creature::Conditions::Panic)
                 && (misc::random::Float() < game::GameDataFile::Instance()->GetCopyFloat(
                                                 "heroespath-fight-chance-panicked-will-retreat"))))
            {
                return TurnActionInfo(TurnAction::Retreat);
            }

            if (((TURN_INFO.GetStrategyInfo().Retreat() == strategy::RetreatType::Coward)
                 && (NUM_PLAYERS_IN_MELEE_RANGE > 0))
                || ((TURN_INFO.GetStrategyInfo().Retreat() == strategy::RetreatType::Wary)
                    && (NUM_PLAYERS_IN_MELEE_RANGE > NUM_FELLOWS_IN_MELEE_RANGE))
                || (NUM_PLAYERS_IN_MELEE_RANGE
                    >= TURN_INFO.GetStrategyInfo().OutnumberedRetreatCount()))
            {
                // fly to evade sometimes
                auto const FLY_TURN_ACTION_INFO{ DecideIfFlying(TURN_INFO, CREATURE_DECIDING_PTR) };
                if (FLY_TURN_ACTION_INFO.Action() != TurnAction::Nothing)
                {
                    return FLY_TURN_ACTION_INFO;
                }
                else
                {
                    return TurnActionInfo(TurnAction::Retreat);
                }
            }
        }

        return TurnActionInfo();
    }

    const TurnActionInfo TurnDecider::DecideIfCasting(
        const TurnInfo & TURN_INFO,
        const creature::CreaturePtr_t CREATURE_DECIDING_PTR,
        const creature::CreaturePtr_t MOST_DESIRED_TARGET_PTR)
    {
        auto const CAN_CAST_HEALING_SPELLS{ CREATURE_DECIDING_PTR->CanCastSpellByEffectType(
            EffectType::CreatureHelpHeal) };

        auto const CAN_CAST_ATTACK_SPELLS{ CREATURE_DECIDING_PTR->CanCastSpellByEffectType(
            EffectType::CreatureHarmDamage) };

        // determine if casting a spell
        if ((CAN_CAST_HEALING_SPELLS || CAN_CAST_ATTACK_SPELLS)
            && (TURN_INFO.GetStrategyInfo().CastFreq() != strategy::FrequencyType::Never))
        {
            // heal self if needed
            if ((CAN_CAST_HEALING_SPELLS) && (CREATURE_DECIDING_PTR->HealthRatio() < 0.26f))
            {
                return DecideSpell(
                    CREATURE_DECIDING_PTR,
                    CREATURE_DECIDING_PTR,
                    { combat::EffectType::CreatureHelpHeal });
            }

            std::size_t maxCastCount{ 0 };

            if (TURN_INFO.GetStrategyInfo().CastFreq() == strategy::FrequencyType::Once)
            {
                maxCastCount = 1;
            }
            else if (TURN_INFO.GetStrategyInfo().CastFreq() == strategy::FrequencyType::Twice)
            {
                maxCastCount = 2;
            }
            else if (TURN_INFO.GetStrategyInfo().CastFreq() == strategy::FrequencyType::Thrice)
            {
                maxCastCount = 3;
            }

            // prevent casting spell if already cast too many times
            if ((maxCastCount == 0) || (TURN_INFO.CastCount() < maxCastCount))
            {
                auto const CAST_CHANCE{ ChanceFromFrequency(
                    TURN_INFO.GetStrategyInfo().CastFreq()) };

                // determine if will cast spell at random
                if ((TURN_INFO.GetStrategyInfo().CastFreq() == strategy::FrequencyType::Always)
                    || (misc::random::Float(1.0f) < CAST_CHANCE))
                {
                    auto const FELLOWS_WITH_LOWEST_HEALTH_PVEC{
                        creature::Algorithms::FindLowestHealthRatio(
                            creature::Algorithms::NonPlayers(creature::Algorithms::Living))
                    };

                    // heal fellow if one is injured enough
                    if (FELLOWS_WITH_LOWEST_HEALTH_PVEC.at(0)->HealthRatio() < 0.25f)
                    {
                        return DecideSpell(
                            CREATURE_DECIDING_PTR,
                            misc::Vector::SelectRandom(FELLOWS_WITH_LOWEST_HEALTH_PVEC),
                            { combat::EffectType::CreatureHelpHeal });
                    }
                    else
                    {
                        //...otherwise choose a random combat spell
                        return DecideSpell(
                            CREATURE_DECIDING_PTR, MOST_DESIRED_TARGET_PTR, HARM_EFFECTTYPES_VEC_);
                    }
                }
            }
        }

        return TurnActionInfo();
    }

    const TurnActionInfo TurnDecider::DecideIfMovingTowardsMostDesiredTarget(
        const TurnInfo & TURN_INFO,
        const creature::CreaturePtr_t CREATURE_DECIDING_PTR,
        CombatDisplayCPtrC_t COMBAT_DISPLAY_CPTRC,
        const int MOST_DESIRED_TARGET_DISTANCE,
        const bool NUM_PLAYERS_IN_ATTACK_RANGE,
        const bool CAN_ATTACK_MOST_DESIRED_TARGET_WITH)
    {
        auto const ADV_TYPE{ TURN_INFO.GetStrategyInfo().Advance() };

        if ((ADV_TYPE != strategy::AdvanceType::Cowardly) && (MOST_DESIRED_TARGET_DISTANCE != 0))
        {
            if (((ADV_TYPE == strategy::AdvanceType::None) && (NUM_PLAYERS_IN_ATTACK_RANGE == 0))
                || (ADV_TYPE == strategy::AdvanceType::Charger)
                || ((ADV_TYPE == strategy::AdvanceType::Hesitant)
                    && (TURN_INFO.GetWasHitLastTurn()
                        && (CAN_ATTACK_MOST_DESIRED_TARGET_WITH == false))))
            {
                if (MOST_DESIRED_TARGET_DISTANCE < 0)
                {
                    if (COMBAT_DISPLAY_CPTRC->CanAdvanceOrRetreat(CREATURE_DECIDING_PTR, true)
                            .empty())
                    {
                        return TurnActionInfo(TurnAction::Advance);
                    }
                }
                else
                {
                    if (COMBAT_DISPLAY_CPTRC->CanAdvanceOrRetreat(CREATURE_DECIDING_PTR, false)
                            .empty())
                    {
                        return TurnActionInfo(TurnAction::Retreat);
                    }
                }
            }
        }

        return TurnActionInfo();
    }

    const TurnActionInfo TurnDecider::DecideIfDoingBeastAction(
        const TurnInfo & TURN_INFO,
        const creature::CreaturePtr_t CREATURE_DECIDING_PTR,
        const creature::CreaturePtr_t MOST_DESIRED_TARGET_PTR,
        const creature::CreaturePVec_t & PLAYERS_IN_MELEE_RANGE_PVEC,
        const int MOST_DESIRED_TARGET_CREATURE_DISTANCE)
    {
        if (CREATURE_DECIDING_PTR->IsBeast())
        {
            creature::CreaturePtr_t creatureToActOn{ MOST_DESIRED_TARGET_PTR };

            // action, chance, rand_value
            using ActionChances_t = std::tuple<TurnAction::Enum, float, float>;
            std::vector<ActionChances_t> actionChancesVec;

            auto const ROAR_CHANCE{ (
                (PLAYERS_IN_MELEE_RANGE_PVEC.size() > 0)
                    ? ChanceFromFrequency(TURN_INFO.GetStrategyInfo().RoarFreq())
                    : 0.0f) };

            if (ROAR_CHANCE > 0.0f)
            {
                auto const RAND{ misc::random::Float(1.0f) };
                if (RAND < ROAR_CHANCE)
                {
                    actionChancesVec.emplace_back(
                        std::make_tuple(TurnAction::Roar, ROAR_CHANCE, RAND));
                }
            }

            auto const FLY_CHANCE{ (
                (CREATURE_DECIDING_PTR->CanFly() && (TURN_INFO.GetIsFlying() == false))
                    ? ChanceFromFrequency(TURN_INFO.GetStrategyInfo().FlyFreq())
                    : 0.0f) };

            if (FLY_CHANCE > 0.0f)
            {
                auto const RAND{ misc::random::Float(1.0f) };
                if (RAND < FLY_CHANCE)
                {
                    actionChancesVec.emplace_back(
                        std::make_tuple(TurnAction::Fly, FLY_CHANCE, RAND));
                }
            }

            auto const SKYPOUNCE_CHANCE{ (
                (TURN_INFO.GetIsFlying())
                    ? ChanceFromFrequency(TURN_INFO.GetStrategyInfo().FlyPounceFreq())
                    : 0.0f) };

            if (SKYPOUNCE_CHANCE > 0.0f)
            {
                auto const RAND{ misc::random::Float(1.0f) };
                if (RAND < FLY_CHANCE)
                {
                    actionChancesVec.emplace_back(
                        std::make_tuple(TurnAction::SkyPounce, SKYPOUNCE_CHANCE, RAND));
                }
            }

            auto const LANDPOUNCE_CHANCE{ (
                ((TURN_INFO.GetIsFlying() == false) && (PLAYERS_IN_MELEE_RANGE_PVEC.size() > 0))
                    ? ChanceFromFrequency(TURN_INFO.GetStrategyInfo().StandPounceFreq())
                    : 0.0f) };

            if (LANDPOUNCE_CHANCE > 0.0f)
            {
                auto const RAND{ misc::random::Float(1.0f) };
                if (RAND < FLY_CHANCE)
                {
                    actionChancesVec.emplace_back(
                        std::make_tuple(TurnAction::LandPounce, LANDPOUNCE_CHANCE, RAND));
                }
            }

            if (actionChancesVec.empty() == false)
            {
                if (actionChancesVec.size() > 1)
                {
                    std::sort(
                        actionChancesVec.begin(),
                        actionChancesVec.end(),
                        [](const ActionChances_t & A, const ActionChances_t & B) {
                            return std::get<2>(A) < std::get<2>(B);
                        });
                }

                auto const DECIDED_ACTION{ std::get<0>(actionChancesVec[0]) };

                if (DECIDED_ACTION == TurnAction::LandPounce)
                {
                    if (MOST_DESIRED_TARGET_CREATURE_DISTANCE == 0)
                    {
                        creatureToActOn = MOST_DESIRED_TARGET_PTR;
                    }
                    else
                    {
                        creatureToActOn = misc::Vector::SelectRandom(PLAYERS_IN_MELEE_RANGE_PVEC);
                    }
                }

                return TurnActionInfo(DECIDED_ACTION, { creatureToActOn });
            }
        }

        return TurnActionInfo();
    }

    const TurnActionInfo TurnDecider::ForcePickSpellToCast(
        const creature::CreaturePtr_t CREATURE_DECIDING_PTR,
        const creature::CreaturePtr_t MOST_DESIRED_TARGET_PTR)
    {
        auto const ALL_SPELLS_PVEC{ CREATURE_DECIDING_PTR->SpellsPVec() };

        spell::SpellPVec_t combatSpellsPVec;

        // Don't allow item based spells for now...
        // TODO -why not?
        std::copy_if(
            ALL_SPELLS_PVEC.begin(),
            ALL_SPELLS_PVEC.end(),
            back_inserter(combatSpellsPVec),
            [](auto const SPELL_PTR) {
                return (
                    (SPELL_PTR->Effect() != combat::EffectType::ItemHarmBreak)
                    && (SPELL_PTR->Effect() != combat::EffectType::ItemHarmMisc)
                    && (SPELL_PTR->Effect() != combat::EffectType::ItemHelpFix)
                    && (SPELL_PTR->Effect() != combat::EffectType::ItemHelpMisc)
                    && (SPELL_PTR->ValidPhases() & game::Phase::Combat));
            });

        if (combatSpellsPVec.empty())
        {
            return TurnActionInfo();
        }

        auto const RAND_FELLOW_WITH_LOWEST_HEALTH_PTR{ misc::Vector::SelectRandom(
            creature::Algorithms::FindLowestHealthRatio(
                creature::Algorithms::NonPlayers(creature::Algorithms::Living))) };

        auto const CAN_CAST_HEAL_SPELLS{ (
            RAND_FELLOW_WITH_LOWEST_HEALTH_PTR->HealthRatio() < 1.0f) };

        spell::SpellPVec_t finalSpellPVec;

        if (CAN_CAST_HEAL_SPELLS)
        {
            finalSpellPVec = combatSpellsPVec;
        }
        else
        {
            std::copy_if(
                combatSpellsPVec.begin(),
                combatSpellsPVec.end(),
                back_inserter(finalSpellPVec),
                [](auto const SPELL_PTR) {
                    return (SPELL_PTR->Effect() != combat::EffectType::CreatureHelpHeal);
                });
        }

        if (finalSpellPVec.empty())
        {
            return TurnActionInfo();
        }
        else
        {
            return DecideSpell(
                CREATURE_DECIDING_PTR,
                MOST_DESIRED_TARGET_PTR,
                { misc::Vector::SelectRandom(finalSpellPVec)->Effect() });
        }
    }

    const TurnActionInfo TurnDecider::DecideSpell(
        const creature::CreaturePtr_t CREATURE_DECIDING_PTR,
        const creature::CreaturePtr_t MOST_DESIRED_TARGET_PTR,
        const EffectTypeVec_t & SPELL_EFFECTTYPES_VEC)
    {
        auto const SPELL_PTR{ PickSpell(CREATURE_DECIDING_PTR, SPELL_EFFECTTYPES_VEC) };

        creature::CreaturePVec_t targetedCreaturesPVec;
        if (SPELL_PTR->Target() == TargetType::AllCompanions)
        {
            targetedCreaturesPVec = creature::Algorithms::PlayersByType(
                ((CREATURE_DECIDING_PTR->IsPlayerCharacter())
                     ? creature::Algorithms::TypeOpt::Player
                     : creature::Algorithms::TypeOpt::NonPlayer),
                creature::Algorithms::Living);
        }
        else if (SPELL_PTR->Target() == TargetType::AllOpponents)
        {
            targetedCreaturesPVec = creature::Algorithms::PlayersByType(
                ((CREATURE_DECIDING_PTR->IsPlayerCharacter())
                     ? creature::Algorithms::TypeOpt::NonPlayer
                     : creature::Algorithms::TypeOpt::Player),
                creature::Algorithms::Living);
        }
        else if (
            (SPELL_PTR->Target() == TargetType::SingleCompanion)
            || (SPELL_PTR->Target() == TargetType::SingleOpponent))
        {
            targetedCreaturesPVec.emplace_back(MOST_DESIRED_TARGET_PTR);
        }
        else
        {
            std::ostringstream ssErr;
            ssErr << "combat::TurnDecider::DecideSpell(creature_deciding=\""
                  << CREATURE_DECIDING_PTR->NameAndRaceAndRole()
                  << "\", most_desired_target_creature=\""
                  << MOST_DESIRED_TARGET_PTR->NameAndRaceAndRole() << "\", chosen_spell=\""
                  << SPELL_PTR->Name() << "\") had a TargetType of \""
                  << TargetType::ToString(SPELL_PTR->Target()) << "\" -which is not yet supported.";

            M_HP_LOG_ERR(ssErr.str());

            return TurnActionInfo();
        }

        return TurnActionInfo(SPELL_PTR, targetedCreaturesPVec);
    }

    const spell::SpellPtr_t TurnDecider::PickSpell(
        const creature::CreaturePtr_t CREATURE_DECIDING_PTR,
        const combat::EffectType::Enum SPELL_EFFECTTYPE)
    {
        return PickSpell(CREATURE_DECIDING_PTR, EffectTypeVec_t{ SPELL_EFFECTTYPE });
    }

    const spell::SpellPtr_t TurnDecider::PickSpell(
        const creature::CreaturePtr_t CREATURE_DECIDING_PTR,
        const EffectTypeVec_t & SPELL_EFFECTTYPE_VEC)
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            (SPELL_EFFECTTYPE_VEC.empty() == false),
            "combat::TurnDecider::PickSpell(creature_deciding="
                << CREATURE_DECIDING_PTR->Name() << ") but SPELL_EFFECTTYPE_VEC was empty.");

        auto const ALL_SPELLS_PVEC{ CREATURE_DECIDING_PTR->SpellsPVec() };

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ALL_SPELLS_PVEC.empty() == false),
            "combat::TurnDecider::PickSpell(creature_deciding="
                << CREATURE_DECIDING_PTR->Name() << ", SPELL_EFFECTTYPE_VEC.size()="
                << SPELL_EFFECTTYPE_VEC.size() << ", SPELL_EFFECTTYPE_VEC[0]="
                << combat::EffectType::ToString(SPELL_EFFECTTYPE_VEC[0])
                << " but that creature has no spells.");

        spell::SpellPVec_t spellsOfTypePVec;

        std::copy_if(
            ALL_SPELLS_PVEC.begin(),
            ALL_SPELLS_PVEC.end(),
            back_inserter(spellsOfTypePVec),
            [&SPELL_EFFECTTYPE_VEC](auto const SPELL_PTR) {
                for (auto const NEXT_SPELL_EFFECTTYPE : SPELL_EFFECTTYPE_VEC)
                {
                    if ((SPELL_PTR->ValidPhases() & game::Phase::Combat)
                        && (NEXT_SPELL_EFFECTTYPE == SPELL_PTR->Effect()))
                    {
                        return true;
                    }
                }
                return false;
            });

        auto const SPELL_COUNT{ spellsOfTypePVec.size() };

        M_ASSERT_OR_LOGANDTHROW_SS(
            (SPELL_COUNT > 0),
            "combat::TurnDecider::PickSpell(creature_deciding="
                << CREATURE_DECIDING_PTR->Name() << ", SPELL_EFFECTTYPE_VEC.size()="
                << SPELL_EFFECTTYPE_VEC.size() << ", SPELL_EFFECTTYPE_VEC[0]="
                << combat::EffectType::ToString(SPELL_EFFECTTYPE_VEC[0])
                << " resulted in no spells.");

        return misc::Vector::SelectRandom(spellsOfTypePVec);
    }

    const TurnActionInfo TurnDecider::DecideIfFlying(
        const TurnInfo & TURN_INFO, const creature::CreaturePtr_t CREATURE_DECIDING_PTR)
    {
        if ((TURN_INFO.GetIsFlying() == false) && (CREATURE_DECIDING_PTR->CanFly()))
        {
            if (misc::random::Float(1.0f)
                < ChanceFromFrequency(TURN_INFO.GetStrategyInfo().FlyFreq()))
            {
                return TurnActionInfo(TurnAction::Fly);
            }
        }

        return TurnActionInfo();
    }

    const TurnActionInfo TurnDecider::DecideIfAdvancingTowardNonMostDesiredTarget(
        const TurnInfo & CREATURE_DECIDING_TURN_INFO,
        const creature::CreaturePtr_t CREATURE_DECIDING_PTR,
        CombatDisplayCPtrC_t COMBAT_DISPLAY_CPTRC)
    {
        auto const ALL_LIVING_PLAYERS_PVEC{ creature::Algorithms::Players(
            creature::Algorithms::Living) };

        if (ALL_LIVING_PLAYERS_PVEC.empty())
        {
            return TurnActionInfo();
        }

        // prefer players still a threat...
        auto ALL_LIVING_THREAT_PLAYERS_PVEC{
            creature::Algorithms::FindByConditionMeaningNotAThreatPermenantly(
                ALL_LIVING_PLAYERS_PVEC,
                creature::Algorithms::CondSingleOpt::DoesNotHave,
                creature::UnconOpt::Include)
        };

        //...but accept those 'not a threat' if no other choice
        auto const AVAILABLE_TARGETS_PVEC{ (
            (ALL_LIVING_THREAT_PLAYERS_PVEC.empty()) ? ALL_LIVING_PLAYERS_PVEC
                                                     : ALL_LIVING_THREAT_PLAYERS_PVEC) };

        // must be reachable given flying or not
        auto const REACHABLE_AVAILABLE_TARGETS_PVEC{ (
            (CREATURE_DECIDING_TURN_INFO.GetIsFlying())
                ? AVAILABLE_TARGETS_PVEC
                : creature::Algorithms::FindByIsFlying(
                      AVAILABLE_TARGETS_PVEC, creature::Algorithms::CriteriaOpt::DoesNotMeet)) };

        if (REACHABLE_AVAILABLE_TARGETS_PVEC.empty())
        {
            return TurnActionInfo();
        }

        // prefer those closest
        auto const CLOSEST_REACH_AVAIL_TARGETS_PVEC{ COMBAT_DISPLAY_CPTRC->FindClosestLiving(
            CREATURE_DECIDING_PTR, REACHABLE_AVAILABLE_TARGETS_PVEC) };

        M_ASSERT_OR_LOGANDTHROW_SS(
            (CLOSEST_REACH_AVAIL_TARGETS_PVEC.empty() == false),
            "combat::TurnDecider::DecideIfAdvancingTowardNonMostDesiredTarget("
                << CREATURE_DECIDING_PTR->NameAndRaceAndRole()
                << ") FindClosestLiving() returned an empty vec.");

        auto const TARGET_CREATURE_PTR{ misc::Vector::SelectRandom(
            CLOSEST_REACH_AVAIL_TARGETS_PVEC) };

        auto const BLOCKING_DISTANCE_TO_TARGET{ COMBAT_DISPLAY_CPTRC->GetBlockingDistanceBetween(
            CREATURE_DECIDING_PTR, TARGET_CREATURE_PTR) };

        if ((BLOCKING_DISTANCE_TO_TARGET < 0)
            && COMBAT_DISPLAY_CPTRC->CanAdvanceOrRetreat(CREATURE_DECIDING_PTR, true).empty())
        {
            return TurnActionInfo(TurnAction::Advance);
        }

        if ((BLOCKING_DISTANCE_TO_TARGET > 0)
            && COMBAT_DISPLAY_CPTRC->CanAdvanceOrRetreat(CREATURE_DECIDING_PTR, false).empty())
        {
            return TurnActionInfo(TurnAction::Retreat);
        }

        return TurnActionInfo();
    }

    void TurnDecider::AdjustCreatueVecForMurderousIntent(
        const TurnInfo & CREATURE_DECIDING_TURN_INFO, creature::CreaturePVec_t & pVec_OutParam)
    {
        // determine if any available targets are unconscious
        auto const ARE_ANY_TARGETS_UNCONSCIOUS{ [&]() {
            for (auto const & NEXT_TARGET_CREATURE_PTR : pVec_OutParam)
            {
                if (NEXT_TARGET_CREATURE_PTR->HasCondition(creature::Conditions::Unconscious))
                {
                    return true;
                }
            }
            return false;
        }() };

        if (ARE_ANY_TARGETS_UNCONSCIOUS
            && (CREATURE_DECIDING_TURN_INFO.GetStrategyInfo().Refine()
                & combat::strategy::RefineType::Murderer))
        {
            // If a murderer and some available targets are unconscious, then only target those
            pVec_OutParam = creature::Algorithms::FindByHasCondition(
                pVec_OutParam, creature::Conditions::Unconscious);
        }
        else if (
            ARE_ANY_TARGETS_UNCONSCIOUS
            && (misc::random::Float(1.0f)
                < game::GameDataFile::Instance()->GetCopyFloat(
                      "heroespath-fight-chance-enemies-ignore-unconscious")))
        {
            // most of the time, don't consider unconscious targets
            pVec_OutParam = creature::Algorithms::FindByHasCondition(
                pVec_OutParam,
                creature::Conditions::Unconscious,
                creature::Algorithms::CondAllOpt::HasAll,
                creature::Algorithms::CondSingleOpt::DoesNotHave);
        }
    }

} // namespace combat
} // namespace heroespath
