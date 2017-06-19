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

    stats::Stat_t FightClub::IsValuetHigherThanRatioOfStat(const stats::Stat_t STAT_VALUE,
                                                           const stats::Stat_t STAT_MAX,
                                                           const float         RATIO)
    {
        return (STAT_VALUE >= static_cast<stats::Stat_t>(static_cast<float>(STAT_MAX) * RATIO));
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

        auto const MOST_DAMAGE_CREATURE_PAIR{ turnInfo.GetMostDamageCreature() };
        if (CREATURE_EFFECT.GetDamageTotal() > MOST_DAMAGE_CREATURE_PAIR.first)
        {
            turnInfo.SetMostDamageCreature(std::make_pair(CREATURE_EFFECT.GetDamageTotal(),
                                                          creatureAttackingPtrC));
        }

        Encounter::Instance()->SetTurnInfo(creatureDefendingPtrC, turnInfo);

        return FightResult(CREATURE_EFFECT);
    }


    void FightClub::HandleDamage(
        creature::CreaturePtrC_t       creatureDefendingPtrC,
        HitInfoVec_t &                 hitInfoVec,
        const stats::Health_t          HEALTH_ADJ,
        creature::CondEnumVec_t & condsAddedVec,
        creature::CondEnumVec_t & condsRemovedVec)
    {
        if (HEALTH_ADJ == 0)
        {
            return;
        }
      
        if (HEALTH_ADJ > 0)
        {
            creatureDefendingPtrC->HealthCurrentAdj(HEALTH_ADJ);

            if (creatureDefendingPtrC->HealthCurrent() > creatureDefendingPtrC->HealthNormal())
            {
                creatureDefendingPtrC->HealthCurrentSet(creatureDefendingPtrC->HealthNormal());

                RemoveAddedCondition(creature::Conditions::Dazed,
                                     creatureDefendingPtrC,
                                     hitInfoVec,
                                     condsRemovedVec);
            }

            RemoveAddedCondition(creature::Conditions::Unconscious,
                                 creatureDefendingPtrC,
                                 hitInfoVec,
                                 condsRemovedVec);

            return;
        }

        //check if already dead
        if (creatureDefendingPtrC->HasCondition(creature::Conditions::Dead) ||
            IsCondContainedInAddedConds(creature::Conditions::Dead, hitInfoVec))
        {
            return;
        }

        auto const IS_ALREADY_UNCONSCIOUS{
            creatureDefendingPtrC->HasCondition(creature::Conditions::Unconscious) ||
                IsCondContainedInAddedConds(creature::Conditions::Unconscious, hitInfoVec) };

        //at this point HEALTH_ADJ is negative
        auto const DAMAGE_ABS{ std::abs(HEALTH_ADJ) };

        //check if the damage will kill
        if (IS_ALREADY_UNCONSCIOUS ||
            ((creatureDefendingPtrC->IsPlayerCharacter() == false) &&
            ((DAMAGE_ABS > (creatureDefendingPtrC->HealthNormal() * 2) ||
                (DAMAGE_ABS >= creatureDefendingPtrC->HealthCurrent())))))
        {
            creatureDefendingPtrC->HealthCurrentSet(0);

            const creature::Conditions::Enum CONDITION_DEAD_ENUM{ creature::Conditions::Dead };

            creatureDefendingPtrC->ConditionAdd(CONDITION_DEAD_ENUM);
            condsAddedVec.push_back(CONDITION_DEAD_ENUM);

            //remove the unconscious condition if already there
            if (IS_ALREADY_UNCONSCIOUS)
            {
                RemoveAddedCondition(creature::Conditions::Unconscious,
                                        creatureDefendingPtrC,
                                        hitInfoVec,
                                        condsRemovedVec);
            }

            return;
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

                creatureDefendingPtrC->ConditionAdd(creature::Conditions::Unconscious);
                condsAddedVec.push_back(creature::Conditions::Unconscious);

                RemoveAddedCondition(creature::Conditions::Dazed,
                                     creatureDefendingPtrC,
                                     hitInfoVec,
                                     condsRemovedVec);

                RemoveAddedCondition(creature::Conditions::Daunted,
                                     creatureDefendingPtrC,
                                     hitInfoVec,
                                     condsRemovedVec);
            }
            else
            {
                AddConditionsBasedOnDamage(creatureDefendingPtrC,
                                           DAMAGE_ABS,
                                           condsAddedVec);
            }
        }
    }


    void FightClub::AddConditionsBasedOnDamage(
        creature::CreaturePtrC_t       creatureDefendingPtrC,
        const stats::Health_t          DAMAGE_ABS,
        creature::CondEnumVec_t & condsAddedVecParam)
    {
        creature::CondEnumVec_t condsVecToAdd{ creature::Conditions::Daunted,
                                                    creature::Conditions::Dazed,
                                                    creature::Conditions::Tripped };
        
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
            if ((misc::random::Float(1.0f) < CHANCE_CONDS_ADDED_RATIO) &&
                (creatureDefendingPtrC->HasCondition(NEXT_COND_ENUM) == false))
            {
                creatureDefendingPtrC->ConditionAdd(NEXT_COND_ENUM);
                condsAddedVecParam.push_back(NEXT_COND_ENUM);
            }
        }
    }


    void FightClub::RemoveAddedCondition(const creature::Conditions::Enum COND_ENUM,
                                         creature::CreaturePtrC_t         creaturePtrC,
                                         HitInfoVec_t &                   hitInfoVec,
                                         creature::CondEnumVec_t &   condsRemovedVec)
    {
        if (creaturePtrC->HasCondition(COND_ENUM))
        {
            if (creaturePtrC->ConditionRemove(COND_ENUM))
            {
                condsRemovedVec.push_back(COND_ENUM);
            }
        }

        for (auto & nextHitInfo : hitInfoVec)
        {
            nextHitInfo.CondsAddedRemove(COND_ENUM);
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

        auto const ATTACK_ACC_RAW{ creatureAttackingPtrC->Stats().Acc().Current() };
        auto attackAccToUse{ ATTACK_ACC_RAW };

        //If the attacking creature is an archer who is using a projectile weapon,
        //then add an accuracy bonus.
        if ((creatureAttackingPtrC->Role().Which() == creature::role::Archer) &&
            (WEAPON_PTR->WeaponType() & item::weapon_type::Projectile))
        {
            auto const ARCHER_ACC_BONUS_RATIO{ GameDataFile::Instance()->GetCopyFloat(
                "heroespath-fight-archer-projectile-accuracy-bonus-ratio") };

            attackAccToUse += static_cast<stats::Stat_t>(
                static_cast<float>(ATTACK_ACC_RAW) * ARCHER_ACC_BONUS_RATIO);

            auto const ARCHER_RANK_BONUS_RATIO{ GameDataFile::Instance()->GetCopyFloat(
                "heroespath-fight-archer-projectile-rank-bonus-ratio") };

            attackAccToUse += static_cast<int>(static_cast<float>(
                creatureAttackingPtrC->Rank()) * ARCHER_RANK_BONUS_RATIO);
        }

        auto const ATTACK_ACC_RAND_MIN{ stats::Stat::Reduce(attackAccToUse) };
        auto const ATTACK_ACC_RAND_MAX{ std::max(ATTACK_ACC_RAW, attackAccToUse) };
        auto const ATTACK_ACC_RAND{ misc::random::Int(ATTACK_ACC_RAND_MIN, ATTACK_ACC_RAND_MAX) };

        auto const STAT_RATIO_AMAZING{ GameDataFile::Instance()->GetCopyFloat(
            "heroespath-fight-stats-amazing-ratio") };

        auto const STAT_HIGHER_THAN_AVERAGE{ GameDataFile::Instance()->GetCopyInt(
            "heroespath-fight-stats-base-high-val") };

        auto const IS_ATTACK_AMAZING_ACC{
            (attackAccToUse >= STAT_HIGHER_THAN_AVERAGE) &&
            IsValuetHigherThanRatioOfStat(ATTACK_ACC_RAND, attackAccToUse, STAT_RATIO_AMAZING)};

        auto const DEFEND_SPD_RAW{ creatureDefendingPtrC->Stats().Spd().Current()};
        auto defendSpdToUse{ DEFEND_SPD_RAW };

        //If the defending creature is a Pixie then add 20% speed bonus plus half defender's rank.
        if (creatureDefendingPtrC->IsPixie())
        {
            auto const PIXIE_DEFEND_SPD_RANK_BONUS_RATIO{ GameDataFile::Instance()->GetCopyFloat(
                "heroespath-fight-pixie-defend-speed-rank-bonus-ratio") };

            defendSpdToUse += static_cast<int>(static_cast<float>(creatureDefendingPtrC->Rank()) *
                PIXIE_DEFEND_SPD_RANK_BONUS_RATIO);
        }

        auto const DEFEND_SPD_RAND_MIN{ stats::Stat::Reduce(defendSpdToUse) };
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
            auto const ATTACKER_LUCK_RAND{ creature::Stats::RatioOfStat(creatureAttackingPtrC,
                                                                        stats::stat::Luck,
                                                                        false,
                                                                        false,
                                                                        true) };

            auto const DEFENDER_LUCK_RAND{ creature::Stats::RatioOfStat(creatureDefendingPtrC,
                                                                        stats::stat::Luck,
                                                                        false,
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

        stats::Health_t damage{ 0 };
        auto isPowerHit{ false };
        auto isCriticalHit{ false };
        creature::CondEnumVec_t condsAddedVec;
        creature::CondEnumVec_t condsRemovedVec;
        if (wasHit)
        {
            damage = DetermineDamage(WEAPON_PTR,
                                     creatureAttackingPtrC,
                                     creatureDefendingPtrC,
                                     isPowerHit,
                                     isCriticalHit);

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
                       condsAddedVec,
                       condsRemovedVec,
                       Text::WeaponActionVerb(WEAPON_PTR, false));
    }


    stats::Health_t FightClub::DetermineDamage(
        const item::ItemPtr_t    WEAPON_PTR,
        creature::CreaturePtrC_t creatureAttackingPtrC,
        creature::CreaturePtrC_t creatureDefendingPtrC,
        bool &                   isPowerHit_OutParam,
        bool &                   isCriticalHit_OutParam)
    {
        const stats::Health_t DAMAGE_FROM_WEAPON{ misc::random::Int(
            WEAPON_PTR->DamageMin(), WEAPON_PTR->DamageMax()) };

        //add extra damage based on rank
        auto const RANK_DAMAGE_BONUS_ADJ_RATIO{ GameDataFile::Instance()->GetCopyFloat(
            "heroespath-fight-rank-damage-bonus-ratio") };

        const stats::Health_t DAMAGE_FROM_RANK{
            static_cast<stats::Health_t>(static_cast<float>(creatureAttackingPtrC->Rank()) *
                RANK_DAMAGE_BONUS_ADJ_RATIO) };

        //If strength stat is at or over the min of STAT_FLOOR,
        //then add a damage bonus based on half a strength ratio "roll".
        const stats::Stat_t STAT_FLOOR{ GameDataFile::Instance()->GetCopyInt(
            "heroespath-fight-stats-value-floor") };

        /*M_HP_LOG_DBG("\t 1*********  FightClub::DetermineDamage("
            << "item=" << WEAPON_PTR->Name()
            << ", attacker=" << creatureAttackingPtrC->Name()
            << ", defending=" << creatureDefendingPtrC->Name()
            << ") weapon_min=" << WEAPON_PTR->DamageMin()
            << ", weapon_max=" << WEAPON_PTR->DamageMax()
            << ", RANK_DAMAGE_BONUS_ADJ_RATIO=" << RANK_DAMAGE_BONUS_ADJ_RATIO
            << ", DAMAGE_FROM_RANK=" << DAMAGE_FROM_RANK
            << ", STAT_FLOOR=" << STAT_FLOOR);
            */
        //
        stats::Health_t damageFromStrength{ 0 };
        auto const STAT_STR{ creatureAttackingPtrC->Stats().Str().Current() };
        if (STAT_STR > STAT_FLOOR)
        {
            auto const RAND_STR_STAT{ creature::Stats::RatioOfStat(
                creatureAttackingPtrC, stats::stat::Strength) };

            auto const STR_BONUS_ADJ_RATIO{ GameDataFile::Instance()->GetCopyFloat(
                "heroespath-fight-damage-strength-bonus-ratio") };

            damageFromStrength += static_cast<stats::Health_t>(
                static_cast<float>(RAND_STR_STAT) * STR_BONUS_ADJ_RATIO);

            damageFromStrength -= STAT_FLOOR;

            if (damageFromStrength < 0)
            {
                damageFromStrength = 0;
            }

            /*M_HP_LOG_DBG("\t 2*********  FightClub::DetermineDamage()"
                << ", STAT_STR=" << STAT_STR
                << ", RAND_STR_STAT=" << RAND_STR_STAT
                << ", STR_BONUS_ADJ_RATIO=" << STR_BONUS_ADJ_RATIO
                << ", damageFromStrength=" << damageFromStrength
                << ", STAT_FLOOR=" << STAT_FLOOR);
                */
        }
        
        const stats::Health_t DAMAGE_BASE{
            DAMAGE_FROM_WEAPON + DAMAGE_FROM_RANK + damageFromStrength };

        //there is a rare chance of a power hit for players
        auto const STRENGTH{ creatureAttackingPtrC->Stats().Str().Current() };
        auto const STRENGTH_RAND_MIN{ stats::Stat::Reduce(STRENGTH) };
        auto const STRENGTH_RAND_MAX{ STRENGTH };
        auto const STRENGTH_RAND{ misc::random::Int(STRENGTH_RAND_MIN, STRENGTH_RAND_MAX) };

        auto const POWER_HIT_CHANCE_RATIO{ GameDataFile::Instance()->GetCopyFloat(
            "heroespath-fight-hit-power-chance-ratio") };

        //low str can mean greater chance of power hit so compensate here
        isPowerHit_OutParam = ((creatureAttackingPtrC->IsPlayerCharacter()) &&
                               (STRENGTH_RAND == STRENGTH_RAND_MAX) &&
                               ((STRENGTH >= STAT_FLOOR) ||
                                    (misc::random::Float(1.0f) < POWER_HIT_CHANCE_RATIO)));

        //there is a rare chance of a critical hit for players
        auto const ACC{ creatureAttackingPtrC->Stats().Acc().Current() };
        auto const ACC_RAND_MIN{ stats::Stat::Reduce(ACC) };
        auto const ACC_RAND_MAX{ ACC };
        auto const ACC_RAND{ misc::random::Int(ACC_RAND_MIN, ACC_RAND_MAX) };

        auto const CRITICAL_HIT_CHANCE_RATIO{ GameDataFile::Instance()->GetCopyFloat(
            "heroespath-fight-hit-critical-chance-ratio") };

        //low acc can mean greater chance of critical hit so compensate here
        isCriticalHit_OutParam = ((creatureAttackingPtrC->IsPlayerCharacter()) &&
                                  (ACC_RAND == ACC_RAND_MAX) &&
                                  ((ACC >= STAT_FLOOR) ||
                                    (misc::random::Float(1.0f) < CRITICAL_HIT_CHANCE_RATIO)));

        stats::Health_t damageFinal{ DAMAGE_BASE };
        stats::Health_t damageFinalA{ damageFinal };

        if (isPowerHit_OutParam)
        {
            damageFinal += damageFromStrength;
        }

        if (isCriticalHit_OutParam)
        {
            damageFinal += DAMAGE_FROM_WEAPON;
        }

        auto const ARMOR_ZERO_DAMAGE_MULT{ GameDataFile::Instance()->GetCopyFloat(
            "heroespath-fight-no-damage-armor-mult") };

        //reduce damage based on the defending creature's armor
        if (creatureDefendingPtrC->ArmorRating() >= static_cast<int>(
            static_cast<float>(damageFinal) * ARMOR_ZERO_DAMAGE_MULT))
        {
            damageFinal = 0;
        }
        else if (damageFinal > creatureDefendingPtrC->ArmorRating())
        {
            damageFinal -= creatureDefendingPtrC->ArmorRating();
        }
        else
        {
            damageFinal = static_cast<stats::Health_t>( static_cast<float>(damageFinal) *
                (static_cast<float>(damageFinal) / static_cast<float>(
                    creatureDefendingPtrC->ArmorRating())));
        }

        /*M_HP_LOG_DBG("\t 3*********  FightClub::DetermineDamage()"
            << "  DAMAGE_FROM_WEAPON=" << DAMAGE_FROM_WEAPON
            << ", DAMAGE_FROM_RANK=" << DAMAGE_FROM_RANK
            << ", damageFromStrength=" << damageFromStrength
            << ", DAMAGE_BASE=" << DAMAGE_BASE
            << ", STRENGTH=" << STRENGTH
            << ", STRENGTH_RAND=" << STRENGTH_RAND
            << ", isPowerHit_OutParam=" << std::boolalpha << isPowerHit_OutParam
            << ", ACC=" << ACC
            << ", ACC_RAND=" << ACC_RAND
            << ", isCriticalHit_OutParam=" << isCriticalHit_OutParam
            << ", damageFinalA=" << damageFinalA
            << ", creatureDefendingPtrC->ArmorRating()=" << creatureDefendingPtrC->ArmorRating()
            << ", damageFinal=" << damageFinal);
            */
        //pixies are dealt less damage than other creatures because of
        //their small size and speed
        if ((damageFinal > 0) &&
            (creatureDefendingPtrC->Race().Which() == creature::race::Pixie))
        {
            auto PIXIE_DAMAGE_ADJ_RATIOB{ 0.0f };
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

                PIXIE_DAMAGE_ADJ_RATIOB = PIXIE_DAMAGE_ADJ_RATIO;
                damageFinal = static_cast<stats::Health_t>(static_cast<float>(damageFinal) *
                    PIXIE_DAMAGE_ADJ_RATIO);
            }
        }

        if(damageFinal < 0)
        {
            damageFinal = 0;
        }

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

        creatureCastingPtr->ManaCurrentAdj(SPELL_CPTR->ManaCost() * -1);

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
                stats::Health_t healthAdj{ 0 };
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

                //TODO Handle Encounter::Instance()->TurnInfo
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

        creaturePlayingPtr->ManaCurrentAdj(SONG_CPTR->ManaCost() * -1);

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
                stats::Health_t healthAdj{ 0 };
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

                //TODO Handle Encounter::Instance()->TurnInfo
            }
        }

        return FightResult(creatureEffectVec);
    }


    const FightResult FightClub::Pounce(creature::CreaturePtrC_t creaturePouncingPtrC,
                                        creature::CreaturePtrC_t creatureDefendingPtrC)
    {
        HitInfo hitInfo;

        if (creatureDefendingPtrC->HasCondition(creature::Conditions::Tripped))
        {
            const ContentAndNamePos CNP("",
                                        " pounces on ",
                                        ".",
                                        NamePosition::SourceThenTarget);

            hitInfo = HitInfo(true, HitType::Pounce, CNP);
        }
        else
        {
            if (creature::Stats::Versus(creaturePouncingPtrC,
                                        { stats::stat::Speed, stats::stat::Accuracy },
                                        creatureDefendingPtrC,
                                        { stats::stat::Speed },
                                        0,
                                        0,
                                        false,
                                        true,
                                        true,
                                        true,
                                        true))
            {
                creatureDefendingPtrC->ConditionAdd(creature::Conditions::Tripped);

                const ContentAndNamePos CNP("",
                                            " pounces on ",
                                            ".",
                                            NamePosition::SourceThenTarget);

                const creature::CondEnumVec_t CONDS_ADDED_VEC{
                    creature::Conditions::Tripped };

                hitInfo = HitInfo(true,
                                  HitType::Pounce,
                                  CNP,
                                  0, //TODO pouncing should do some damage
                                  CONDS_ADDED_VEC,
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
        }

        return FightResult( CreatureEffect(creatureDefendingPtrC,
                                           HitInfoVec_t(1, hitInfo)) );
    }


    const FightResult FightClub::Roar(creature::CreaturePtrC_t creatureRoaringPtrC,
                                      CombatDisplayCPtrC_t     COMBAT_DISPLAY_CPTRC)
    {
        auto const LIVING_OPPONENT_CREATURES_PVEC{ creature::Algorithms::PlayersByType(
            ! creatureRoaringPtrC->IsPlayerCharacter(), true) };

        CreatureEffectVec_t creatureEffectsVec;

        auto const IS_ROARING_CREATURE_FLYING{
            combat::Encounter::Instance()->GetTurnInfoCopy(creatureRoaringPtrC).GetIsFlying() };

        //Give each defending creature a chance to resist being panicked.
        //The farther away each defending creature is the better chance
        //of resisting he/she/it has.
        for (auto const NEXT_DEFEND_CREATURE_PTR : LIVING_OPPONENT_CREATURES_PVEC)
        {
            if (NEXT_DEFEND_CREATURE_PTR->HasCondition(creature::Conditions::Panic))
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

            const stats::Stat_t DISATANCE_BONUS{ nextBlockingDisatnce * 2 };

            if (creature::Stats::Versus(creatureRoaringPtrC,
                                        stats::stat::Strength,
                                        NEXT_DEFEND_CREATURE_PTR,
                                        stats::stat::Intelligence,
                                        0,
                                        DISATANCE_BONUS,
                                        false,
                                        true,
                                        true,
                                        true,
                                        true))
            {
                NEXT_DEFEND_CREATURE_PTR->ConditionAdd(creature::Conditions::Panic);

                const ContentAndNamePos CNP("'s roar panics ",
                                        NamePosition::SourceThenTarget);

                const HitInfo HIT_INFO(
                    true,
                    HitType::Roar,
                    CNP,
                    0,
                    creature::CondEnumVec_t(1, creature::Conditions::Panic),
                    creature::CondEnumVec_t());

                creatureEffectsVec.push_back( CreatureEffect(NEXT_DEFEND_CREATURE_PTR,
                                                             HitInfoVec_t(1, HIT_INFO)) );
            }
            else
            {
                const ContentAndNamePos CNP(" resisted the fear.",
                                            NamePosition::TargetBefore);

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

        auto const LIVE_ATTBLE_NP_CRTS_PVEC{
            creature::Algorithms::FindByAlive(attackableNonPlayerCreaturesPVec) };

        M_ASSERT_OR_LOGANDTHROW_SS((LIVE_ATTBLE_NP_CRTS_PVEC.empty() == false),
            "game::combat::FightClub::HandleAttack() FindNonPlayerCreatureToAttack() returned "
            << "no LIVING attackable creatures.");

        //attack those with the lowest relative health first, which will correspond
        //to the health bar seen on screen
        auto const LIVE_ATTBLE_LOWH_NP_CRTS_PVEC{
            creature::Algorithms::FindLowestHealthRatio(LIVE_ATTBLE_NP_CRTS_PVEC) };

        M_ASSERT_OR_LOGANDTHROW_SS((LIVE_ATTBLE_LOWH_NP_CRTS_PVEC.empty() == false),
            "game::combat::FightClub::HandleAttack() FindNonPlayerCreatureToAttack() returned "
            << "no LIVING LOWEST HEALTH RATIO attackable creatures.");

        //skip creatures who are not a threat
        auto const LIVE_ATTBLE_LOWH_NP_NOTPERMDIS_CRTS_PVEC{
            creature::Algorithms::FindByConditionMeaningNotAThreatPermenantly(
                LIVE_ATTBLE_LOWH_NP_CRTS_PVEC, false) };

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


    bool FightClub::IsCondContainedInAddedConds(const creature::Conditions::Enum E,
                                                const HitInfoVec_t & HIT_INFO_VEC)
    {
        for (auto const & NEXT_HIT_INFO : HIT_INFO_VEC)
        {
            if (NEXT_HIT_INFO.CondsAddedContains(E))
            {
                return true;
            }
        }

        return false;
    }

}
}
