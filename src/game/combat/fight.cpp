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
// fight.cpp
//
#include "fight.hpp"

#include "game/log-macros.hpp"
#include "game/creature/creature.hpp"
#include "game/creature/conditions.hpp"
#include "game/creature/algorithms.hpp"
#include "game/creature/stats.hpp"
#include "game/item/item.hpp"
#include "game/item/armor-ratings.hpp"
#include "game/spell/spell-base.hpp"
#include "game/combat/combat-display.hpp"
#include "game/combat/encounter.hpp"
#include "game/combat/combat-text.hpp"
#include "game/name-position-enum.hpp"
#include "game/song/song.hpp"
#include "game/game-data-file.hpp"

#include "misc/random.hpp"
#include "misc/vectors.hpp"

#include <string>
#include <algorithm>
#include <cmath>


namespace game
{
namespace combat
{

    stats::Trait_t FightClub::IsValuetHigherThanRatioOfStat(const stats::Trait_t STAT_VALUE,
                                                           const stats::Trait_t STAT_MAX,
                                                           const float         RATIO)
    {
        return (STAT_VALUE >= static_cast<stats::Trait_t>(static_cast<float>(STAT_MAX) * RATIO));
    }


    const FightResult FightClub::Fight(creature::CreaturePtrC_t creatureAttackingPtrC,
                                       creature::CreaturePtrC_t creatureDefendingPtrC,
                                       const bool               WILL_FORCE_HIT)
    {
        auto const HIT_INFO_VEC{ AttackWithAllWeapons(creatureAttackingPtrC,
                                                      creatureDefendingPtrC,
                                                      WILL_FORCE_HIT) };

        auto const CREATURE_EFFECT{ CreatureEffect(creatureDefendingPtrC, HIT_INFO_VEC) };

        auto turnInfo{ Encounter::Instance()->GetTurnInfoCopy(creatureDefendingPtrC) };

        if (turnInfo.GetFirstAttackedByCreature() == nullptr)
        {
            turnInfo.SetFirstAttackedByCreature(creatureAttackingPtrC);
        }

        turnInfo.SetLastAttackedByCreature(creatureAttackingPtrC);

        if (CREATURE_EFFECT.GetWasHit())
        {
            turnInfo.SetWasHitLastTurn(true);

            if (turnInfo.GetFirstHitByCreature() == nullptr)
            {
                turnInfo.SetFirstHitByCreature(creatureAttackingPtrC);
            }

            if (turnInfo.GetLastHitByCreature() == nullptr)
            {
                turnInfo.SetLastHitByCreature(creatureAttackingPtrC);
            }
        }

        auto const MOST_DAMAGE_CREATURE_PAIR{ turnInfo.GetMostDamageCreaturePair() };
        if (CREATURE_EFFECT.GetDamageTotal() > MOST_DAMAGE_CREATURE_PAIR.first)
        {
            turnInfo.SetMostDamageCreaturePair(std::make_pair(CREATURE_EFFECT.GetDamageTotal(),
                                                              creatureAttackingPtrC));
        }

        Encounter::Instance()->SetTurnInfo(creatureDefendingPtrC, turnInfo);

        return FightResult(CREATURE_EFFECT);
    }


    void FightClub::HandleDamage(
        creature::CreaturePtrC_t  creatureDefendingPtrC,
        HitInfoVec_t &            hitInfoVec,
        const stats::Trait_t      HEALTH_ADJ,
        creature::CondEnumVec_t & condsAddedVec,
        creature::CondEnumVec_t & condsRemovedVec,
        const bool                CAN_ADD_CONDITIONS)
    {
        using namespace creature;

        if (HEALTH_ADJ == 0)
        {
            return;
        }

        if (HEALTH_ADJ > 0)
        {
            creatureDefendingPtrC->HealthCurrentAdj(HEALTH_ADJ);

            if (creatureDefendingPtrC->HealthCurrent() == creatureDefendingPtrC->HealthNormal())
            {
                RemoveAddedConditions({ Conditions::Dazed, Conditions::Daunted },
                                      creatureDefendingPtrC,
                                      hitInfoVec,
                                      condsRemovedVec);

                if (AreAnyOfCondsContained({ Conditions::Panic },
                                           creatureDefendingPtrC,
                                           hitInfoVec))
                {
                    RemoveAddedCondition(Conditions::Panic,
                                         creatureDefendingPtrC,
                                         hitInfoVec,
                                         condsRemovedVec);

                    creatureDefendingPtrC->ConditionAdd(Conditions::Daunted);
                    condsAddedVec.push_back(Conditions::Daunted);
                }
            }

            RemoveAddedCondition(Conditions::Unconscious,
                                 creatureDefendingPtrC,
                                 hitInfoVec,
                                 condsRemovedVec);

            return;
        }

        //check if already dead
        if (AreAnyOfCondsContained({ Conditions::Dead },
                                   creatureDefendingPtrC,
                                   hitInfoVec))
        {
            return;
        }

        auto const IS_ALREADY_UNCONSCIOUS{ AreAnyOfCondsContained(
            { Conditions::Unconscious }, creatureDefendingPtrC, hitInfoVec) };

        //at this point HEALTH_ADJ is negative
        auto const DAMAGE_ABS{ std::abs(HEALTH_ADJ) };

        //logic needed to determine if damage will kill
        auto const IS_NONPLAYER_CHARACTER{ creatureDefendingPtrC->IsPlayerCharacter() == false };
        auto const IS_DAMAGE_DOUBLE{ (DAMAGE_ABS > (creatureDefendingPtrC->HealthNormal() * 2)) };
        auto const WILL_DAMAGE_KILL{ (DAMAGE_ABS >= creatureDefendingPtrC->HealthCurrent()) };
        auto const NOT_PIXIE{ (creatureDefendingPtrC->IsPixie() == false) };

        //check if the damage will kill
        if (IS_ALREADY_UNCONSCIOUS ||
            (IS_NONPLAYER_CHARACTER && ((IS_DAMAGE_DOUBLE && NOT_PIXIE) || WILL_DAMAGE_KILL)))
        {
            creatureDefendingPtrC->HealthCurrentSet(0);
            creatureDefendingPtrC->ConditionAdd(Conditions::Dead);
            condsAddedVec.push_back(Conditions::Dead);

            auto turnInfo{ combat::Encounter::Instance()->GetTurnInfoCopy(creatureDefendingPtrC) };
            turnInfo.SetIsFlying(false);
            turnInfo.SetWasHitLastTurn(true);
            turnInfo.SetTurnActionInfo( TurnActionInfo() );
            combat::Encounter::Instance()->SetTurnInfo(creatureDefendingPtrC, turnInfo);

            //remove the unconscious condition if already there
            if (IS_ALREADY_UNCONSCIOUS)
            {
                RemoveAddedCondition(Conditions::Unconscious,
                                     creatureDefendingPtrC,
                                     hitInfoVec,
                                     condsRemovedVec);
            }


            return;
        }

        if (DAMAGE_ABS > 0)
        {
            //wake from natural sleep if hit
            RemoveAddedCondition(Conditions::AsleepNatural,
                                 creatureDefendingPtrC,
                                 hitInfoVec,
                                 condsRemovedVec);
        }

        creatureDefendingPtrC->HealthCurrentAdj(DAMAGE_ABS * -1);
        if (creatureDefendingPtrC->HealthCurrent() < 0)
        {
            creatureDefendingPtrC->HealthCurrentSet(0);
        }

        if (IS_ALREADY_UNCONSCIOUS == false)
        {
            if (creatureDefendingPtrC->IsPlayerCharacter() &&
                (creatureDefendingPtrC->HealthCurrent() <= 0))
            {
                creatureDefendingPtrC->HealthCurrentSet(1);

                creatureDefendingPtrC->ConditionAdd(Conditions::Unconscious);
                condsAddedVec.push_back(Conditions::Unconscious);

                RemoveAddedConditions({ Conditions::Dazed,
                                        Conditions::Daunted,
                                        Conditions::AsleepNatural,
                                        Conditions::AsleepMagical,
                                        Conditions::Bold,
                                        Conditions::Heroic,
                                        Conditions::Panic,
                                        Conditions::Tripped },
                                      creatureDefendingPtrC,
                                      hitInfoVec,
                                      condsRemovedVec);

                Encounter::Instance()->SetIsFlying(creatureDefendingPtrC, false);
            }
            else
            {
                if (CAN_ADD_CONDITIONS)
                {
                    AddConditionsBasedOnDamage(creatureDefendingPtrC,
                                               DAMAGE_ABS,
                                               condsAddedVec,
                                               condsRemovedVec,
                                               hitInfoVec);
                }
            }
        }
    }


    void FightClub::AddConditionsBasedOnDamage(
        creature::CreaturePtrC_t  creatureDefendingPtrC,
        const stats::Trait_t     DAMAGE_ABS,
        creature::CondEnumVec_t & condsAddedVecParam,
        creature::CondEnumVec_t & condsRemovedVecParam,
        HitInfoVec_t &            hitInfoVec)
    {
        using namespace creature;

        creature::CondEnumVec_t condsVecToAdd{ Conditions::Daunted,
                                               Conditions::Dazed,
                                               Conditions::Tripped };

        misc::Vector::ShuffleVec(condsVecToAdd);

        if (DAMAGE_ABS > (creatureDefendingPtrC->HealthNormal() -
            (creatureDefendingPtrC->HealthNormal() / 8)))
        {
            //leave condsVec as is, with all three conditions contained
        }
        else if (DAMAGE_ABS > (creatureDefendingPtrC->HealthNormal() -
                    (creatureDefendingPtrC->HealthNormal() / 4)))
        {
            condsVecToAdd.resize(2);
        }
        else if (DAMAGE_ABS > (creatureDefendingPtrC->HealthNormal() -
                    (creatureDefendingPtrC->HealthNormal() / 2)))
        {
            condsVecToAdd.resize(1);
        }
        else
        {
            return;
        }

        auto const CHANCE_CONDS_ADDED_RATIO{ GameDataFile::Instance()->GetCopyFloat(
            "heroespath-fight-chance-conditions-added-from-damage-ratio") };

        for (auto const NEXT_COND_ENUM : condsVecToAdd)
        {
            //only a 50/50 chance of adding conditions
            if (misc::random::Float(1.0f) < CHANCE_CONDS_ADDED_RATIO)
            {
                if ((NEXT_COND_ENUM == Conditions::Dazed) && AreAnyOfCondsContained(
                    { Conditions::AsleepMagical,
                      Conditions::AsleepNatural,
                      Conditions::Dazed,
                      Conditions::Unconscious },
                    creatureDefendingPtrC,
                    hitInfoVec))
                {
                    continue;
                }

                if ((NEXT_COND_ENUM == Conditions::Daunted) && AreAnyOfCondsContained(
                    { Conditions::Daunted,
                      Conditions::Unconscious },
                    creatureDefendingPtrC,
                    hitInfoVec))
                {
                    continue;
                }

                if ((NEXT_COND_ENUM == Conditions::Tripped) && AreAnyOfCondsContained(
                    { Conditions::Tripped,
                      Conditions::AsleepMagical,
                      Conditions::AsleepNatural,
                      Conditions::Unconscious,
                      Conditions::Pounced },
                    creatureDefendingPtrC,
                    hitInfoVec))
                {
                    continue;
                }


                if ((NEXT_COND_ENUM == Conditions::Daunted) && AreAnyOfCondsContained(
                    { Conditions::Heroic,
                      Conditions::Bold },
                    creatureDefendingPtrC,
                    hitInfoVec))
                {
                    RemoveAddedConditions({ Conditions::Heroic, Conditions::Bold },
                                         creatureDefendingPtrC,
                                         hitInfoVec,
                                         condsRemovedVecParam);
                    continue;
                }

                creatureDefendingPtrC->ConditionAdd(NEXT_COND_ENUM);
                condsAddedVecParam.push_back(NEXT_COND_ENUM);
            }
        }
    }


    bool FightClub::RemoveAddedConditions(
        const creature::CondEnumVec_t & CONDS_VEC,
        creature::CreaturePtrC_t        creaturePtrC,
        HitInfoVec_t &                  hitInfoVec,
        creature::CondEnumVec_t &       condsRemovedVec)
    {
        auto wasAnyRemoved{ false };

        for (auto const NEXT_COND_ENUM : CONDS_VEC)
        {
            if (RemoveAddedCondition(NEXT_COND_ENUM,
                                     creaturePtrC,
                                     hitInfoVec,
                                     condsRemovedVec))
            {
                wasAnyRemoved = true;
            }
        }

        return wasAnyRemoved;
    }


    bool FightClub::RemoveAddedCondition(const creature::Conditions::Enum COND_ENUM,
                                         creature::CreaturePtrC_t         creaturePtrC,
                                         HitInfoVec_t &                   hitInfoVec,
                                         creature::CondEnumVec_t &        condsRemovedVec)
    {
        for (auto & nextHitInfo : hitInfoVec)
        {
            nextHitInfo.CondsAddedRemove(COND_ENUM);
        }

        if (creaturePtrC->HasCondition(COND_ENUM))
        {
            creaturePtrC->ConditionRemove(COND_ENUM);
            condsRemovedVec.push_back(COND_ENUM);
            return true;
        }
        else
        {
            return false;
        }
    }


    const HitInfoVec_t FightClub::AttackWithAllWeapons(
        creature::CreaturePtrC_t creatureAttackingPtrC,
        creature::CreaturePtrC_t creatureDefendingPtrC,
        const bool               WILL_FORCE_HIT)
    {
        HitInfoVec_t hitInfoVec;

        auto const WEAPONS_PVEC{ creatureAttackingPtrC->CurrentWeaponsCopy() };
        for (auto const NEXT_ITEM_PTR : WEAPONS_PVEC)
        {
            if (NEXT_ITEM_PTR->IsWeapon())
            {
                auto const NEXT_HIT_INFO{ AttackWithSingleWeapon(hitInfoVec,
                                                                 NEXT_ITEM_PTR,
                                                                 creatureAttackingPtrC,
                                                                 creatureDefendingPtrC,
                                                                 WILL_FORCE_HIT) };

                hitInfoVec.push_back(NEXT_HIT_INFO);

                if (NEXT_HIT_INFO.CondsAddedContains(creature::Conditions::Dead) ||
                    creatureDefendingPtrC->HasCondition(creature::Conditions::Dead))
                {
                    break;
                }
            }
        }

        return hitInfoVec;
    }


    //Determine if attacking creature's accuracy overcomes the defending
    //creature's speed to see if there was a hit.
    const HitInfo FightClub::AttackWithSingleWeapon(
        HitInfoVec_t &           hitInfoVec,
        const item::ItemPtr_t    WEAPON_PTR,
        creature::CreaturePtrC_t creatureAttackingPtrC,
        creature::CreaturePtrC_t creatureDefendingPtrC,
        const bool               WILL_FORCE_HIT)
    {
        auto hasHitBeenDetermined{ false };
        auto wasHit{ false };

        auto const ATTACK_ACC_RAW{ creatureAttackingPtrC->Accuracy() };
        auto attackAccToUse{ ATTACK_ACC_RAW };

        //If the attacking creature is an archer who is using a projectile weapon,
        //then add an accuracy bonus.
        if ((creatureAttackingPtrC->Role() == creature::role::Archer) &&
            (WEAPON_PTR->WeaponType() & item::weapon_type::Projectile))
        {
            auto const ARCHER_ACC_BONUS_RATIO{ GameDataFile::Instance()->GetCopyFloat(
                "heroespath-fight-archer-projectile-accuracy-bonus-ratio") };

            attackAccToUse += static_cast<stats::Trait_t>(
                static_cast<float>(ATTACK_ACC_RAW) * ARCHER_ACC_BONUS_RATIO);

            auto const ARCHER_RANK_BONUS_RATIO{ GameDataFile::Instance()->GetCopyFloat(
                "heroespath-fight-archer-projectile-rank-bonus-ratio") };

            attackAccToUse += static_cast<int>(static_cast<float>(
                creatureAttackingPtrC->Rank()) * ARCHER_RANK_BONUS_RATIO);
        }

        auto const ATTACK_ACC_RAND_MIN{ static_cast<stats::Trait_t>(
            static_cast<float>(attackAccToUse) * 0.4f) };

        auto const ATTACK_ACC_RAND_MAX{ std::max(ATTACK_ACC_RAW, attackAccToUse) };
        auto const ATTACK_ACC_RAND{ misc::random::Int(ATTACK_ACC_RAND_MIN, ATTACK_ACC_RAND_MAX) };

        auto const STAT_RATIO_AMAZING{ GameDataFile::Instance()->GetCopyFloat(
            "heroespath-fight-stats-amazing-ratio") };

        auto const STAT_HIGHER_THAN_AVERAGE{ GameDataFile::Instance()->GetCopyInt(
            "heroespath-fight-stats-base-high-val") };

        auto const IS_ATTACK_AMAZING_ACC{
            (attackAccToUse >= STAT_HIGHER_THAN_AVERAGE) &&
            IsValuetHigherThanRatioOfStat(ATTACK_ACC_RAND, attackAccToUse, STAT_RATIO_AMAZING)};

        auto const DEFEND_SPD_RAW{ creatureDefendingPtrC->Speed() };
        auto defendSpdToUse{ DEFEND_SPD_RAW };

        //If the defending creature is a Pixie then add a speed bonus based on rank.
        if (creatureDefendingPtrC->IsPixie())
        {
            defendSpdToUse += static_cast<int>(static_cast<float>(creatureDefendingPtrC->Rank()) *
                GameDataFile::Instance()->GetCopyFloat(
                    "heroespath-fight-pixie-defend-speed-rank-bonus-ratio"));
        }

        //If the defending creature is Blocking, then add a speed bonus.
        if (combat::Encounter::Instance()->GetTurnInfoCopy(
            creatureDefendingPtrC).GetTurnActionInfo().Action() == TurnAction::Block)
        {
            defendSpdToUse += static_cast<int>(static_cast<float>(DEFEND_SPD_RAW) *
                GameDataFile::Instance()->GetCopyFloat(
                    "heroespath-fight-block-defend-speed-bonus-ratio"));
        }

        auto const DEFEND_SPD_RAND_MIN{ static_cast<stats::Trait_t>(
            static_cast<float>(defendSpdToUse) * 0.4f) };

        auto const DEFEND_SPD_RAND_MAX{ std::max(DEFEND_SPD_RAW, defendSpdToUse) };
        auto const DEFEND_SPD_RAND{ misc::random::Int(DEFEND_SPD_RAND_MIN, DEFEND_SPD_RAND_MAX) };

        auto const IS_DEFENSE_AMAZING_DODGE{
            (defendSpdToUse >= STAT_HIGHER_THAN_AVERAGE) &&
            IsValuetHigherThanRatioOfStat(DEFEND_SPD_RAND, defendSpdToUse, STAT_RATIO_AMAZING)};

        if (IS_ATTACK_AMAZING_ACC && (IS_DEFENSE_AMAZING_DODGE == false))
        {
            //This case allows any attacker the rare chance to hit, even when the odds are
            //overwhelmingly bad, such as when the attackers accuracy max is less than the
            //defender's speed min.
            wasHit = true;
            hasHitBeenDetermined = true;
        }
        else if ((IS_ATTACK_AMAZING_ACC == false) && IS_DEFENSE_AMAZING_DODGE)
        {
            //This case allows any defender the rare chance to dodge even if the odds are
            //overwhelmingly bad, such as when the defender's speed max is less then the
            //attacker's accuracy min.
            wasHit = false;
            hasHitBeenDetermined = true;
        }

        //In this case, either both the attacker and defending did their best and tied for
        //"rolling" their highest possible rand/score or the rand "rolled" scores were equal.
        //So the hit is determined by whose stat+rank is higher.
        //This is the case most likely to decide if hit or miss.
        if (false == hasHitBeenDetermined)
        {
            auto const ATTACK_ACC_RANK_ADJ{ ATTACK_ACC_RAND +
                static_cast<int>(creatureAttackingPtrC->Rank()) };

            auto const DEFEND_SPD_RANK_ADJ{ DEFEND_SPD_RAND +
                static_cast<int>(creatureDefendingPtrC->Rank()) };

            if (ATTACK_ACC_RANK_ADJ > DEFEND_SPD_RANK_ADJ)
            {
                wasHit = true;
                hasHitBeenDetermined = true;
            }
            else if (ATTACK_ACC_RANK_ADJ < DEFEND_SPD_RANK_ADJ)
            {
                wasHit = false;
                hasHitBeenDetermined = true;
            }
        }

        //In this case both the rand "rolls" and the stat+rank are equal,
        //so determine if the attack hit or miss based on luck.
        if (false == hasHitBeenDetermined)
        {
            auto const ATTACKER_LUCK_RAND{ creature::Stats::Roll(creatureAttackingPtrC,
                                                                 stats::Traits::Luck,
                                                                 false,
                                                                 true) };

            auto const DEFENDER_LUCK_RAND{ creature::Stats::Roll(creatureDefendingPtrC,
                                                                 stats::Traits::Luck,
                                                                 false,
                                                                 true) };
            if (ATTACKER_LUCK_RAND == DEFENDER_LUCK_RAND)
            {
                //In this case, attaker and defender tied on luck rolls,
                //so the hit is determined by who has the greater luck roll + rank.
                auto const ATTACK_LCK_RANK_ADJ{ ATTACKER_LUCK_RAND +
                    static_cast<int>(creatureAttackingPtrC->Rank()) };

                auto const DEFEND_LCK_RANK_ADJ{ DEFENDER_LUCK_RAND +
                    static_cast<int>(creatureDefendingPtrC->Rank()) };

                if (ATTACK_LCK_RANK_ADJ > DEFEND_LCK_RANK_ADJ)
                {
                    wasHit = true;
                    hasHitBeenDetermined = true;
                }
                else if (ATTACK_LCK_RANK_ADJ < DEFEND_LCK_RANK_ADJ)
                {
                    wasHit = false;
                    hasHitBeenDetermined = true;
                }
            }
            else
            {
                wasHit = (ATTACKER_LUCK_RAND > DEFENDER_LUCK_RAND);
                hasHitBeenDetermined = true;
            }
        }

        //In this case, everything that could be equal WAS equal, so let the player win.
        if (false == hasHitBeenDetermined)
        {
            if (creatureAttackingPtrC->IsPlayerCharacter() &&
                creatureDefendingPtrC->IsPlayerCharacter())
            {
                //In this case, both attacker and defender are players,
                //so let fair coin toss determine the hit.
                wasHit = misc::random::Bool();
                hasHitBeenDetermined = true;
            }
            else
            {
                wasHit = creatureAttackingPtrC->IsPlayerCharacter();
                hasHitBeenDetermined = true;
            }
        }

        //handle forced hits
        if (WILL_FORCE_HIT && (false == wasHit))
        {
            wasHit = true;
            hasHitBeenDetermined = true;
        }

        stats::Trait_t damage{ 0 };
        auto isPowerHit{ false };
        auto isCriticalHit{ false };
        auto didArmorAbsorb{ false };
        creature::CondEnumVec_t condsAddedVec;
        creature::CondEnumVec_t condsRemovedVec;
        if (wasHit)
        {
            damage = DetermineDamage(WEAPON_PTR,
                                     creatureAttackingPtrC,
                                     creatureDefendingPtrC,
                                     isPowerHit,
                                     isCriticalHit,
                                     didArmorAbsorb);

            HandleDamage(creatureDefendingPtrC,
                         hitInfoVec,
                         damage,
                         condsAddedVec,
                         condsRemovedVec);
        }

        return HitInfo(wasHit,
                       WEAPON_PTR,
                       damage,
                       isCriticalHit,
                       isPowerHit,
                       didArmorAbsorb,
                       condsAddedVec,
                       condsRemovedVec,
                       Text::WeaponActionVerb(WEAPON_PTR, false));
    }


    stats::Trait_t FightClub::DetermineDamage(
        const item::ItemPtr_t    WEAPON_PTR,
        creature::CreaturePtrC_t creatureAttackingPtrC,
        creature::CreaturePtrC_t creatureDefendingPtrC,
        bool &                   isPowerHit_OutParam,
        bool &                   isCriticalHit_OutParam,
        bool &                   didArmorAbsorb_OutParam)
    {
        const stats::Trait_t DAMAGE_FROM_WEAPON_RAW{ misc::random::Int(
            WEAPON_PTR->DamageMin(), WEAPON_PTR->DamageMax()) };

        //If weapon is fist and creature attacking is wearing gauntlets, then triple the damage.
        stats::Trait_t extraDamage{ 0 };
        if (WEAPON_PTR->WeaponType() & item::weapon_type::Fists)
        {
            auto const EQUIPPED_ITEMS_PVEC{ creatureAttackingPtrC->Inventory().ItemsEquipped() };
            for (auto const NEXT_ITEM_PTR : EQUIPPED_ITEMS_PVEC)
            {
                if ((NEXT_ITEM_PTR->ArmorType() & item::armor_type::Gauntlets) &&
                    (NEXT_ITEM_PTR->Armor_Info().base != item::armor::base_type::Plain))
                {
                    extraDamage = 2 * DAMAGE_FROM_WEAPON_RAW;
                    break;
                }
            }
        }

        //If weapon is bite and creature has fangs, then tripple the damage.
        if ((WEAPON_PTR->WeaponType() & item::weapon_type::Bite) &&
            (creatureAttackingPtrC->Body().HasFangs()))
        {
            extraDamage = DAMAGE_FROM_WEAPON_RAW;
        }

        auto const DAMAGE_FROM_WEAPON{ DAMAGE_FROM_WEAPON_RAW + extraDamage };

        //add extra damage based on rank
        auto const RANK_DAMAGE_BONUS_ADJ_RATIO{ GameDataFile::Instance()->GetCopyFloat(
            "heroespath-fight-rank-damage-bonus-ratio") };

        const stats::Trait_t DAMAGE_FROM_RANK{
            static_cast<stats::Trait_t>(static_cast<float>(creatureAttackingPtrC->Rank()) *
                RANK_DAMAGE_BONUS_ADJ_RATIO) };

        //If strength stat is at or over the min of STAT_FLOOR,
        //then add a damage bonus based on half a strength ratio "roll".
        const stats::Trait_t STAT_FLOOR{ GameDataFile::Instance()->GetCopyInt(
            "heroespath-fight-stats-value-floor") };

        stats::Trait_t damageFromStrength{ 0 };
        auto const STRENGTH_CURRENT{ creatureAttackingPtrC->Strength() };
        if (STRENGTH_CURRENT > STAT_FLOOR)
        {
            auto const RAND_STR_STAT{ creature::Stats::Roll(creatureAttackingPtrC,
                                                            stats::Traits::Strength) };

            auto const STR_BONUS_ADJ_RATIO{ GameDataFile::Instance()->GetCopyFloat(
                "heroespath-fight-damage-strength-bonus-ratio") };

            damageFromStrength += static_cast<stats::Trait_t>(
                static_cast<float>(RAND_STR_STAT) * STR_BONUS_ADJ_RATIO);

            damageFromStrength -= STAT_FLOOR;

            if (damageFromStrength < 0)
            {
                damageFromStrength = 0;
            }
        }

        const stats::Trait_t DAMAGE_BASE{
            DAMAGE_FROM_WEAPON + DAMAGE_FROM_RANK + damageFromStrength };

        //there is a rare chance of a power hit for players
        auto const STRENGTH_TEST{ creature::Stats::Test(creatureAttackingPtrC,
                                                        stats::Traits::Strength,
                                                        0.25f,
                                                        true,
                                                        true) };

        auto const POWER_HIT_CHANCE_RATIO{ GameDataFile::Instance()->GetCopyFloat(
            "heroespath-fight-hit-power-chance-ratio") };

        isPowerHit_OutParam = (creatureAttackingPtrC->IsPlayerCharacter() &&
                               STRENGTH_TEST &&
                               (STRENGTH_CURRENT >= STAT_FLOOR) &&
                               (misc::random::Float(1.0f) < POWER_HIT_CHANCE_RATIO));

        //there is a rare chance of a critical hit for players and non-players
        auto const ACCURACY_TEST{ creature::Stats::Test(creatureAttackingPtrC,
                                                        stats::Traits::Accuracy,
                                                        0.25f,
                                                        true,
                                                        true) };

        auto const LUCK_TEST{ creature::Stats::Test(creatureAttackingPtrC,
                                                    stats::Traits::Luck,
                                                    0.25f,
                                                    true,
                                                    true) };

        auto const CRITICAL_HIT_CHANCE_RATIO{ GameDataFile::Instance()->GetCopyFloat(
            "heroespath-fight-hit-critical-chance-ratio") };

        auto const ACCURACY_CURRENT{ creatureAttackingPtrC->Accuracy() };

        isCriticalHit_OutParam = ((creatureAttackingPtrC->IsPlayerCharacter() || LUCK_TEST) &&
                                  ACCURACY_TEST &&
                                  ((ACCURACY_CURRENT >= STAT_FLOOR) || LUCK_TEST) &&
                                  (misc::random::Float(1.0f) < CRITICAL_HIT_CHANCE_RATIO));

        stats::Trait_t damageFinal{ DAMAGE_BASE };

        const stats::Trait_t SPECIAL_HIT_DAMAGE_MIN{
            static_cast<stats::Trait_t>(creatureAttackingPtrC->Rank()) +
                GameDataFile::Instance()->GetCopyInt("heroespath-fight-hit-special-damage-min") };

        if (isPowerHit_OutParam)
        {
            damageFinal += std::max(std::max(damageFromStrength,
                (damageFinal / SPECIAL_HIT_DAMAGE_MIN)), SPECIAL_HIT_DAMAGE_MIN);
        }

        if (isCriticalHit_OutParam)
        {
            damageFinal += std::max(DAMAGE_FROM_WEAPON, SPECIAL_HIT_DAMAGE_MIN);
        }

        auto const DAMAGE_AFTER_SPECIALS{ damageFinal };

        //reduce damage based on the defending creature's armor
        auto armorRatingToUse{ creatureDefendingPtrC->ArmorRating() };
        if (creatureDefendingPtrC->HasCondition(creature::Conditions::Tripped))
        {
            armorRatingToUse -= (item::ArmorRatings::Instance()->ArmoredLesserSteel() +
                item::ArmorRatings::Instance()->ArmoredGreaterSteel()) / 4;

            if (armorRatingToUse < 0)
            {
                armorRatingToUse = 0;
            }
        }

        damageFinal -= static_cast<stats::Trait_t>(static_cast<float>(damageFinal) *
            (static_cast<float>(armorRatingToUse) /
                static_cast<float>(item::ArmorRatings::Instance()->ArmoredGreaterDiamond()) ) );

        //check if armor absorbed all the damage
        if ((DAMAGE_AFTER_SPECIALS > 0) && (damageFinal <= 0))
        {
            didArmorAbsorb_OutParam = true;
        }

        //pixies are dealt less damage than other creatures because of
        //their small size and speed
        if ((damageFinal > 0) &&
            (creatureDefendingPtrC->Race() == creature::race::Pixie))
        {
            auto const PIXIE_DAMAGE_FLOOR{ GameDataFile::Instance()->GetCopyInt(
                "heroespath-fight-pixie-damage-floor") };

            if (damageFinal < PIXIE_DAMAGE_FLOOR)
            {
                damageFinal = 1;
            }
            else
            {
                auto const PIXIE_DAMAGE_ADJ_RATIO{ GameDataFile::Instance()->GetCopyFloat(
                    "heroespath-fight-pixie-damage-adj-ratio") };

                damageFinal = static_cast<stats::Trait_t>(static_cast<float>(damageFinal) *
                    PIXIE_DAMAGE_ADJ_RATIO);
            }
        }

        //prevent negative damage
        if(damageFinal < 0)
        {
            damageFinal = 0;
        }

        //if power or critical hit, then assure there is some damage dealt
        if ((isPowerHit_OutParam || isCriticalHit_OutParam) &&
            (damageFinal < SPECIAL_HIT_DAMAGE_MIN))
        {
            didArmorAbsorb_OutParam = false;

            if (isPowerHit_OutParam)
            {
                damageFinal += std::max(damageFromStrength, SPECIAL_HIT_DAMAGE_MIN);
            }

            if (isCriticalHit_OutParam)
            {
                damageFinal += std::max(DAMAGE_FROM_WEAPON, SPECIAL_HIT_DAMAGE_MIN);
            }
        }

        //update TurnInfo
        auto turnInfo{ Encounter::Instance()->GetTurnInfoCopy(creatureDefendingPtrC) };

        if (turnInfo.GetFirstAttackedByCreature() == nullptr)
        {
            turnInfo.SetFirstAttackedByCreature(creatureAttackingPtrC);
        }

        if (turnInfo.GetFirstHitByCreature() == nullptr)
        {
            turnInfo.SetFirstHitByCreature(creatureAttackingPtrC);
        }

        turnInfo.SetLastAttackedByCreature(creatureAttackingPtrC);
        turnInfo.SetLastHitByCreature(creatureAttackingPtrC);

        auto mostDamageCreaturePair{ turnInfo.GetMostDamageCreaturePair() };
        if (mostDamageCreaturePair.first < std::abs(damageFinal))
        {
            mostDamageCreaturePair.first = std::abs(damageFinal);
            mostDamageCreaturePair.second = creatureAttackingPtrC;
            turnInfo.SetMostDamageCreaturePair(mostDamageCreaturePair);
        }

        turnInfo.SetWasHitLastTurn(true);

        Encounter::Instance()->SetTurnInfo(creatureDefendingPtrC, turnInfo);

        return damageFinal * -1;
    }



    const FightResult FightClub::Cast(const spell::SpellPtr_t          SPELL_CPTR,
                                      creature::CreaturePtrC_t         creatureCastingPtr,
                                      const creature::CreaturePVec_t & creaturesCastUponPVec)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((SPELL_CPTR != nullptr),
            "game::combat::FightClub::Cast() was given a null SPELL_CPTR.");

        M_ASSERT_OR_LOGANDTHROW_SS((creaturesCastUponPVec.empty() == false),
            "game::combat::FightClub::Cast(spell=" << SPELL_CPTR->Name()
            << ", creature_casting=" << creatureCastingPtr->NameAndRaceAndRole()
            << ", creatures_cast_upon=empty) was given an empty creaturesCastUponPVec.");

        creatureCastingPtr->TraitCurrentAdj(stats::Traits::Mana, SPELL_CPTR->ManaCost() * -1);

        if (((SPELL_CPTR->Target() == TargetType::SingleCompanion) ||
            (SPELL_CPTR->Target() == TargetType::SingleOpponent)) &&
                (creaturesCastUponPVec.size() > 1))
        {
            std::ostringstream ssErr;
            ssErr << "game::combat::FightClub::Cast(spell=" << SPELL_CPTR->Name()
                  << ", creature_casting=" << creatureCastingPtr->NameAndRaceAndRole()
                  << ", creatures_cast_upon=\"" << misc::Vector::Join<creature::CreaturePtr_t>(
                      creaturesCastUponPVec,
                      false,
                      false,
                      0,
                      false,
                      []
                      (const creature::CreaturePtr_t CPTR) -> const std::string
                      { return CPTR->NameAndRaceAndRole(); })
                  << "\") spell target_type=" << TargetType::ToString(SPELL_CPTR->Target())
                  << " but there were " << creaturesCastUponPVec.size()
                  << " creatures being cast upon.  There should have been only 1.";
            throw std::runtime_error(ssErr.str());
        }

        //update caster's TurnInfo
        auto casterTurnInfo{ Encounter::Instance()->GetTurnInfoCopy(creatureCastingPtr) };
        casterTurnInfo.CastCountIncrement();
        Encounter::Instance()->SetTurnInfo(creatureCastingPtr, casterTurnInfo);

        //update caster's TurnActionInfo
        Encounter::Instance()->SetTurnActionInfo(creatureCastingPtr,
            TurnActionInfo(SPELL_CPTR, creaturesCastUponPVec) );

        //attempt to have spell effect each target creature
        CreatureEffectVec_t creatureEffectVec;
        for (auto nextCreatureCastUponPtr : creaturesCastUponPVec)
        {
            if (nextCreatureCastUponPtr->IsAlive() == false)
            {
                const ContentAndNamePos CNP(" is dead.", NamePosition::TargetBefore);

                const HitInfo HIT_INFO(false,
                                       SPELL_CPTR,
                                       CNP);

                creatureEffectVec.push_back( CreatureEffect(nextCreatureCastUponPtr,
                                                            HitInfoVec_t(1, HIT_INFO)) );
            }
            else
            {
                ContentAndNamePos actionPhraseCNP;
                stats::Trait_t healthAdj{ 0 };
                creature::CondEnumVec_t condsAddedVec;
                creature::CondEnumVec_t condsRemovedVec;

                auto const DID_SPELL_SUCCEED{ SPELL_CPTR->EffectCreature(
                    creatureCastingPtr,
                    nextCreatureCastUponPtr,
                    healthAdj,
                    condsAddedVec,
                    condsRemovedVec,
                    actionPhraseCNP) };

                HitInfoVec_t hitInfoVec;

                if (DID_SPELL_SUCCEED)
                {
                    HandleDamage(nextCreatureCastUponPtr,
                                 hitInfoVec,
                                 healthAdj,
                                 condsAddedVec,
                                 condsRemovedVec);
                }

                hitInfoVec.push_back( HitInfo(DID_SPELL_SUCCEED,
                                              SPELL_CPTR,
                                              actionPhraseCNP,
                                              healthAdj,
                                              condsAddedVec,
                                              condsRemovedVec) );

                creatureEffectVec.push_back( CreatureEffect(nextCreatureCastUponPtr,
                                                            hitInfoVec) );

                //update target's TurnInfo
                auto turnInfo{ Encounter::Instance()->GetTurnInfoCopy(nextCreatureCastUponPtr) };

                turnInfo.SetWasHitLastTurn(false);

                if ((SPELL_CPTR->Effect() == EffectType::CreatureHarmDamage) ||
                    (SPELL_CPTR->Effect() == EffectType::CreatureHarmMisc))
                {
                    if (turnInfo.GetFirstAttackedByCreature() == nullptr)
                    {
                        turnInfo.SetFirstAttackedByCreature(creatureCastingPtr);
                    }

                    turnInfo.SetLastAttackedByCreature(creatureCastingPtr);

                    if (DID_SPELL_SUCCEED)
                    {
                        if (turnInfo.GetFirstHitByCreature() == nullptr)
                        {
                            turnInfo.SetFirstHitByCreature(creatureCastingPtr);
                        }

                        turnInfo.SetLastHitByCreature(creatureCastingPtr);

                        auto mostDamageCreaturePair{ turnInfo.GetMostDamageCreaturePair() };
                        if (mostDamageCreaturePair.first < std::abs(healthAdj))
                        {
                            mostDamageCreaturePair.first = std::abs(healthAdj);
                            mostDamageCreaturePair.second = creatureCastingPtr;
                            turnInfo.SetMostDamageCreaturePair(mostDamageCreaturePair);
                        }

                        turnInfo.SetWasHitLastTurn(true);
                    }
                }

                turnInfo.SetLastToCastCreature(creatureCastingPtr);

                if (DID_SPELL_SUCCEED && (turnInfo.GetFirstToCastCreature() == nullptr))
                {
                    turnInfo.SetFirstToCastCreature(creatureCastingPtr);
                }

                Encounter::Instance()->SetTurnInfo(nextCreatureCastUponPtr, turnInfo);
            }
        }

        return FightResult(creatureEffectVec);
    }


    const FightResult FightClub::PlaySong(
        const song::SongPtr_t            SONG_CPTR,
        creature::CreaturePtrC_t         creaturePlayingPtr,
        const creature::CreaturePVec_t & creaturesListeningPVec)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((SONG_CPTR != nullptr),
            "game::combat::FightClub::PlaySong() was given a null SONG_CPTR.");

        M_ASSERT_OR_LOGANDTHROW_SS((creaturesListeningPVec.empty() == false),
            "game::combat::FightClub::PlaySong(song=" << SONG_CPTR->Name()
            << ", creature_playing=" << creaturePlayingPtr->NameAndRaceAndRole()
            << ", creatures_listening=empty) was given an empty creaturesListeningPVec.");

        creaturePlayingPtr->TraitCurrentAdj(stats::Traits::Mana, SONG_CPTR->ManaCost() * -1);

        if (((SONG_CPTR->Target() == TargetType::SingleCompanion) ||
            (SONG_CPTR->Target() == TargetType::SingleOpponent)) &&
                (creaturesListeningPVec.size() > 1))
        {
            std::ostringstream ssErr;
            ssErr << "game::combat::FightClub::PlaySong(song=" << SONG_CPTR->Name()
                  << ", creature_playing=" << creaturePlayingPtr->NameAndRaceAndRole()
                  << ", creatures_listening=\"" << misc::Vector::Join<creature::CreaturePtr_t>(
                      creaturesListeningPVec,
                      false,
                      false,
                      0,
                      false,
                      []
                      (const creature::CreaturePtr_t CPTR) -> const std::string
                      { return CPTR->NameAndRaceAndRole(); })
                  << "\") song target_type=" << TargetType::ToString(SONG_CPTR->Target())
                  << " but there were " << creaturesListeningPVec.size()
                  << " creatures listening.  There should have been only 1.";
            throw std::runtime_error(ssErr.str());
        }

        //update player's TurnInfo
        auto playerTurnInfo{ Encounter::Instance()->GetTurnInfoCopy(creaturePlayingPtr) };
        playerTurnInfo.SongCountIncrement();
        Encounter::Instance()->SetTurnInfo(creaturePlayingPtr, playerTurnInfo);

        //update player's TurnActionInfo
        Encounter::Instance()->SetTurnActionInfo(creaturePlayingPtr,
                TurnActionInfo(SONG_CPTR, creaturesListeningPVec));

        //attempt to have spell effect each target creature
        CreatureEffectVec_t creatureEffectVec;
        for (auto nextCreatureCastUponPtr : creaturesListeningPVec)
        {
            if (nextCreatureCastUponPtr->IsAlive() == false)
            {
                const ContentAndNamePos CNP(" is dead.", NamePosition::TargetBefore);

                const HitInfo HIT_INFO(false,
                                       SONG_CPTR,
                                       CNP);

                creatureEffectVec.push_back( CreatureEffect(nextCreatureCastUponPtr,
                                                            HitInfoVec_t(1, HIT_INFO)) );
            }
            else
            {
                ContentAndNamePos actionPhraseCNP;
                stats::Trait_t healthAdj{ 0 };
                creature::CondEnumVec_t condsAddedVec;
                creature::CondEnumVec_t condsRemovedVec;

                auto const DID_SONG_SUCCEED{ SONG_CPTR->EffectCreature(
                    creaturePlayingPtr,
                    nextCreatureCastUponPtr,
                    healthAdj,
                    condsAddedVec,
                    condsRemovedVec,
                    actionPhraseCNP) };

                HitInfoVec_t hitInfoVec;

                if (DID_SONG_SUCCEED)
                {
                    HandleDamage(nextCreatureCastUponPtr,
                                 hitInfoVec,
                                 healthAdj,
                                 condsAddedVec,
                                 condsRemovedVec);
                }

                hitInfoVec.push_back( HitInfo(DID_SONG_SUCCEED,
                                              SONG_CPTR,
                                              actionPhraseCNP,
                                              healthAdj,
                                              condsAddedVec,
                                              condsRemovedVec) );

                creatureEffectVec.push_back( CreatureEffect(nextCreatureCastUponPtr,
                                                            hitInfoVec) );

                //handle TurnInfo
                auto turnInfo{ Encounter::Instance()->GetTurnInfoCopy(nextCreatureCastUponPtr) };

                if (turnInfo.GetFirstToMakeMusicCreature() == nullptr)
                {
                    turnInfo.SetFirstToMakeMusicCreature(creaturePlayingPtr);
                }

                turnInfo.SetLastToMakeMusicCreature(creaturePlayingPtr);

                turnInfo.SetWasHitLastTurn(false);

                if ((SONG_CPTR->Effect() == EffectType::CreatureHarmDamage) ||
                    (SONG_CPTR->Effect() == EffectType::CreatureHarmMisc))
                {
                    if (turnInfo.GetFirstAttackedByCreature() == nullptr)
                    {
                        turnInfo.SetFirstAttackedByCreature(creaturePlayingPtr);
                    }

                    if (DID_SONG_SUCCEED)
                    {
                        turnInfo.SetFirstHitByCreature(creaturePlayingPtr);
                        turnInfo.SetLastAttackedByCreature(creaturePlayingPtr);
                        turnInfo.SetLastHitByCreature(creaturePlayingPtr);

                        auto mostDamageCreaturePair{ turnInfo.GetMostDamageCreaturePair() };
                        if (mostDamageCreaturePair.first < std::abs(healthAdj))
                        {
                            mostDamageCreaturePair.first = std::abs(healthAdj);
                            mostDamageCreaturePair.second = creaturePlayingPtr;
                            turnInfo.SetMostDamageCreaturePair(mostDamageCreaturePair);
                        }

                        turnInfo.SetWasHitLastTurn(true);
                    }
                }

                Encounter::Instance()->SetTurnInfo(nextCreatureCastUponPtr, turnInfo);
            }
        }

        return FightResult(creatureEffectVec);
    }


    const FightResult FightClub::Pounce(creature::CreaturePtrC_t creaturePouncingPtrC,
                                        creature::CreaturePtrC_t creatureDefendingPtrC)
    {
        using namespace creature;

        //update player's TurnActionInfo
        Encounter::Instance()->SetTurnActionInfo(creaturePouncingPtrC,
            TurnActionInfo(((Encounter::Instance()->GetTurnInfoCopy(
                creaturePouncingPtrC).GetIsFlying()) ?
                    TurnAction::SkyPounce : TurnAction::LandPounce), { creatureDefendingPtrC }));

        HitInfo hitInfo;

        auto const DID_ROLL_SUCCEED{ creature::Stats::Versus(
            creaturePouncingPtrC,
            { stats::Traits::Speed, stats::Traits::Accuracy },
            creatureDefendingPtrC,
            { stats::Traits::Speed },
            0,
            0,
            true,
            true,
            true,
            true) };

        auto const DID_SUCCEED{ (DID_ROLL_SUCCEED ||
            creatureDefendingPtrC->HasCondition(Conditions::Tripped) ||
            creatureDefendingPtrC->HasCondition(Conditions::AsleepNatural) ||
            creatureDefendingPtrC->HasCondition(Conditions::AsleepMagical) ||
            creatureDefendingPtrC->HasCondition(Conditions::Unconscious)) };

        stats::Trait_t healthAdj{ 0 };

        if (DID_SUCCEED)
        {
            //TODO pouncing should do some damage
            healthAdj = 0;

            HitInfoVec_t hitInfoVec;
            creature::CondEnumVec_t condsAddedVec;
            creature::CondEnumVec_t condsRemovedVec;

            if (creatureDefendingPtrC->HasCondition(Conditions::Tripped))
            {
                creatureDefendingPtrC->ConditionRemove(Conditions::Tripped);
                condsRemovedVec.push_back(Conditions::Tripped);
            }

            HandleDamage(creatureDefendingPtrC,
                         hitInfoVec,
                         healthAdj,
                         condsAddedVec,
                         condsRemovedVec);

            creatureDefendingPtrC->ConditionAdd(Conditions::Pounced);

            const ContentAndNamePos CNP("",
                                        " pounces on ",
                                        ".",
                                        NamePosition::SourceThenTarget);

            hitInfo = HitInfo(true,
                              HitType::Pounce,
                              CNP,
                              healthAdj,
                              { Conditions::Pounced },
                              creature::CondEnumVec_t());
        }
        else
        {
            const ContentAndNamePos CNP("",
                                        " tried to pounce on ",
                                        "but missed.",
                                        NamePosition::SourceThenTarget);

            hitInfo = HitInfo(false, HitType::Pounce, CNP);
        }

        //update defender's TurnInfo
        auto turnInfo{ Encounter::Instance()->GetTurnInfoCopy(creatureDefendingPtrC) };

        if (turnInfo.GetFirstAttackedByCreature() == nullptr)
        {
            turnInfo.SetFirstAttackedByCreature(creaturePouncingPtrC);
        }

        turnInfo.SetLastAttackedByCreature(creaturePouncingPtrC);

        turnInfo.SetWasHitLastTurn(false);

        if (DID_SUCCEED)
        {
            if (turnInfo.GetFirstHitByCreature() == nullptr)
            {
                turnInfo.SetFirstHitByCreature(creaturePouncingPtrC);
            }

            turnInfo.SetLastHitByCreature(creaturePouncingPtrC);

            auto mostDamageCreaturePair{ turnInfo.GetMostDamageCreaturePair() };
            if (mostDamageCreaturePair.first < std::abs(healthAdj))
            {
                mostDamageCreaturePair.first = std::abs(healthAdj);
                mostDamageCreaturePair.second = creaturePouncingPtrC;
                turnInfo.SetMostDamageCreaturePair(mostDamageCreaturePair);
            }

            turnInfo.SetWasHitLastTurn(true);
        }

        Encounter::Instance()->SetTurnInfo(creatureDefendingPtrC, turnInfo);


        return FightResult( CreatureEffect(creatureDefendingPtrC,
                                           HitInfoVec_t(1, hitInfo)) );
    }


    const FightResult FightClub::Roar(creature::CreaturePtrC_t creatureRoaringPtrC,
                                      CombatDisplayCPtrC_t     COMBAT_DISPLAY_CPTRC)
    {
        using namespace creature;

        CreaturePVec_t listeningCreaturesPVec;
        COMBAT_DISPLAY_CPTRC->GetCreaturesInRoaringDistance(creatureRoaringPtrC,
                                                            listeningCreaturesPVec);

        //update player's TurnActionInfo
        Encounter::Instance()->SetTurnActionInfo(creatureRoaringPtrC,
            TurnActionInfo(TurnAction::Roar, listeningCreaturesPVec) );

        CreatureEffectVec_t creatureEffectsVec;

        auto const IS_ROARING_CREATURE_FLYING{
            combat::Encounter::Instance()->GetTurnInfoCopy(creatureRoaringPtrC).GetIsFlying() };

        //Give each defending creature a chance to resist being panicked.
        //The farther away each defending creature is the better chance
        //of resisting he/she/it has.
        for (auto const NEXT_DEFEND_CREATURE_PTR : listeningCreaturesPVec)
        {
            if (NEXT_DEFEND_CREATURE_PTR->HasCondition(Conditions::Panic))
            {
                const ContentAndNamePos CNP(" is already panicked.",
                                            NamePosition::TargetBefore);

                const HitInfo HIT_INFO(false, HitType::Roar, CNP);

                creatureEffectsVec.push_back( CreatureEffect(NEXT_DEFEND_CREATURE_PTR,
                                                             HitInfoVec_t(1, HIT_INFO)) );

                continue;
            }

            auto nextBlockingDisatnce{ std::abs(COMBAT_DISPLAY_CPTRC->
                GetBlockingDistanceBetween(creatureRoaringPtrC, NEXT_DEFEND_CREATURE_PTR)) };

            //if flying, then consider it farther away and less likely to be panicked
            if (Encounter::Instance()->GetTurnInfoCopy(NEXT_DEFEND_CREATURE_PTR).GetIsFlying() &&
                (IS_ROARING_CREATURE_FLYING == false))
            {
                ++nextBlockingDisatnce;
            }

            const stats::Trait_t DISATANCE_BONUS{ nextBlockingDisatnce * 2 };

            if (creature::Stats::Versus(creatureRoaringPtrC,
                                        stats::Traits::Strength,
                                        NEXT_DEFEND_CREATURE_PTR,
                                        stats::Traits::Intelligence,
                                        0,
                                        DISATANCE_BONUS,
                                        true,
                                        true,
                                        true,
                                        true))
            {
                //no TurnInfo settings need to be made for roar

                //remove the Daunted condition before adding the Panicked condition
                NEXT_DEFEND_CREATURE_PTR->ConditionRemove(Conditions::Daunted);

                NEXT_DEFEND_CREATURE_PTR->ConditionAdd(Conditions::Panic);

                const ContentAndNamePos CNP("'s roar panics ",
                                            NamePosition::SourceThenTarget);

                const HitInfo HIT_INFO(
                    true,
                    HitType::Roar,
                    CNP,
                    0,
                    creature::CondEnumVec_t(1, Conditions::Panic),
                    creature::CondEnumVec_t());

                creatureEffectsVec.push_back( CreatureEffect(NEXT_DEFEND_CREATURE_PTR,
                                                             HitInfoVec_t(1, HIT_INFO)) );
            }
            else
            {
                const ContentAndNamePos CNP("",
                                            " resisted the fear of ",
                                            "'s roar.",
                                            NamePosition::TargetThenSource);

                const HitInfo HIT_INFO(false, HitType::Roar, CNP);

                creatureEffectsVec.push_back( CreatureEffect(NEXT_DEFEND_CREATURE_PTR,
                                                             HitInfoVec_t(1, HIT_INFO)) );
            }
        }

        return FightResult(creatureEffectsVec);
    }


    creature::CreaturePtr_t FightClub::FindNonPlayerCreatureToAttack(
        creature::CreaturePtrC_t creatureAttackingtrC,
        CombatDisplayCPtrC_t     COMBAT_DISPLAY_CPTRC)
    {
        creature::CreaturePVec_t attackableNonPlayerCreaturesPVec;
        COMBAT_DISPLAY_CPTRC->FindCreaturesThatCanBeAttackedOfType(
            attackableNonPlayerCreaturesPVec, creatureAttackingtrC, false);

        M_ASSERT_OR_LOGANDTHROW_SS((attackableNonPlayerCreaturesPVec.empty() == false),
            "game::combat::FightClub::HandleAttack() FindNonPlayerCreatureToAttack() "
            << "returned no attackable creatures.");

        //attack those with the lowest relative health first, which will correspond
        //to the health bar seen on screen
        auto const LIVE_ATTBLE_LOWH_NP_CRTS_PVEC{
            creature::Algorithms::FindLowestHealthRatio(attackableNonPlayerCreaturesPVec) };

        M_ASSERT_OR_LOGANDTHROW_SS((LIVE_ATTBLE_LOWH_NP_CRTS_PVEC.empty() == false),
            "game::combat::FightClub::HandleAttack() FindNonPlayerCreatureToAttack() returned "
            << "no LIVING LOWEST HEALTH RATIO attackable creatures.");

        //skip creatures who are not a threat
        auto const LIVE_ATTBLE_LOWH_NP_NOTPERMDIS_CRTS_PVEC{
            creature::Algorithms::FindByConditionMeaningNotAThreatPermenantly(
                LIVE_ATTBLE_LOWH_NP_CRTS_PVEC, false, true) };

        //attack those that are temporarily disabled first
        auto const LIVE_ATTBLE_LOWH_NP_NOTPERMDIS_TMPDIS_CRTS_PVEC{
            creature::Algorithms::FindByConditionMeaningNotAThreatTemporarily(
                LIVE_ATTBLE_LOWH_NP_NOTPERMDIS_CRTS_PVEC) };

        if (LIVE_ATTBLE_LOWH_NP_NOTPERMDIS_TMPDIS_CRTS_PVEC.empty())
        {
            if (LIVE_ATTBLE_LOWH_NP_NOTPERMDIS_CRTS_PVEC.empty())
            {
                return misc::Vector::SelectRandom(COMBAT_DISPLAY_CPTRC->FindClosestAmongOfType(
                    creatureAttackingtrC, LIVE_ATTBLE_LOWH_NP_CRTS_PVEC, false));
            }
            else
            {
                return misc::Vector::SelectRandom(COMBAT_DISPLAY_CPTRC->FindClosestAmongOfType(
                    creatureAttackingtrC, LIVE_ATTBLE_LOWH_NP_NOTPERMDIS_CRTS_PVEC, false));
            }
        }
        else
        {
            return misc::Vector::SelectRandom(COMBAT_DISPLAY_CPTRC->
                FindClosestAmongOfType(creatureAttackingtrC,
                                       LIVE_ATTBLE_LOWH_NP_NOTPERMDIS_TMPDIS_CRTS_PVEC,
                                       false));
        }
    }


    bool FightClub::AreAnyOfCondsContained(const creature::CondEnumVec_t & CONDS_VEC,
                                           const creature::CreaturePtrC_t  CREATURE_PTR,
                                           const HitInfoVec_t &            HIT_INFO_VEC)
    {
        for (auto const NEXT_COND_ENUM : CONDS_VEC)
        {
            if (CREATURE_PTR->HasCondition(NEXT_COND_ENUM))
            {
                return true;
            }

            for (auto const & NEXT_HIT_INFO : HIT_INFO_VEC)
            {
                if (NEXT_HIT_INFO.CondsAddedContains(NEXT_COND_ENUM))
                {
                    return true;
                }
            }
        }

        return false;
    }

}
}
