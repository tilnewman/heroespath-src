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

#include "game/creature/creature.hpp"
#include "game/creature/conditions.hpp"
#include "game/creature/algorithms.hpp"
#include "game/item/item.hpp"
#include "game/spell/spell-base.hpp"
#include "game/combat/combat-display.hpp"
#include "game/combat/encounter.hpp"
#include "game/combat/combat-text.hpp"

#include "misc/random.hpp"
#include "misc/vectors.hpp"

#include <string>
#include <algorithm>
#include <cmath>


namespace game
{
namespace combat
{

    const stats::Stat_t FightClub::STAT_HIGHER_THAN_AVERAGE_{ 20 };
    const float FightClub::STAT_RATIO_AMAZING_{ 0.5f };


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
        auto const HIT_INFO_VEC{ AttackWithAllWeapons(creatureAttackingPtrC, creatureDefendingPtrC, WILL_FORCE_HIT) };
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
            turnInfo.SetMostDamageCreature(std::make_pair(CREATURE_EFFECT.GetDamageTotal(), creatureAttackingPtrC));
        }

        Encounter::Instance()->SetTurnInfo(creatureDefendingPtrC, turnInfo);

        return FightResult(CREATURE_EFFECT);
    }


    const creature::ConditionEnumVec_t FightClub::HandleDamage(creature::CreaturePtrC_t creatureDefendingPtrC,
                                                               HitInfoVec_t &           hitInfoVec,
                                                               const stats::Health_t    HEALTH_ADJ)
    {
        if (HEALTH_ADJ == 0)
        {
            return creature::ConditionEnumVec_t();
        }
       
        if (HEALTH_ADJ > 0)
        {
            creatureDefendingPtrC->HealthCurrentAdj(HEALTH_ADJ);

            if (creatureDefendingPtrC->HealthCurrent() > creatureDefendingPtrC->HealthNormal())
            {
                creatureDefendingPtrC->HealthCurrentSet(creatureDefendingPtrC->HealthNormal());
               
                if (creatureDefendingPtrC->HasCondition(creature::Conditions::Dazed))
                {
                    creatureDefendingPtrC->ConditionRemove(creature::Conditions::Dazed);
                }
            }

            if (creatureDefendingPtrC->HasCondition(creature::Conditions::Unconscious))
            {
                creatureDefendingPtrC->ConditionRemove(creature::Conditions::Unconscious);
            }

            return creature::ConditionEnumVec_t();
        }

        //at this point HEALTH_ADJ is negative
        auto const DAMAGE_ABS{ std::abs(HEALTH_ADJ) };

        creature::ConditionEnumVec_t conditionsAddedVec;

        auto const IS_ALREADY_KILLED{ creatureDefendingPtrC->HasCondition(creature::Conditions::Dead) ||
                                      IsConditionContained(creature::Conditions::Dead, hitInfoVec) };

        if (IS_ALREADY_KILLED == false)
        {
            auto willKill{ false };

            auto const IS_ALREADY_UNCONSCIOUS{ creatureDefendingPtrC->HasCondition(creature::Conditions::Unconscious) ||
                                               IsConditionContained(creature::Conditions::Unconscious, hitInfoVec) };

            if (IS_ALREADY_UNCONSCIOUS)
            {
                willKill = true;
            }
            else if ((creatureDefendingPtrC->IsPlayerCharacter() == false) &&
                     ((DAMAGE_ABS > (creatureDefendingPtrC->HealthNormal() * 2) ||
                        (DAMAGE_ABS > creatureDefendingPtrC->HealthCurrent()))))
            {
                willKill = true;
            }

            if (willKill)
            {
                creatureDefendingPtrC->HealthCurrentSet(0);

                const creature::Conditions::Enum CONDITION_DEAD_ENUM{ creature::Conditions::Dead };
                creatureDefendingPtrC->ConditionAdd(CONDITION_DEAD_ENUM);
                conditionsAddedVec.push_back(CONDITION_DEAD_ENUM);

                //remove the unconscious condition if already there
                if (IS_ALREADY_UNCONSCIOUS)
                {
                    creatureDefendingPtrC->ConditionRemove(creature::Conditions::Unconscious);

                    for (auto & nextHitInfo : hitInfoVec)
                    {
                        nextHitInfo.RemoveCondition(creature::Conditions::Unconscious);
                    }
                }
            }
            else
            {
                creatureDefendingPtrC->HealthCurrentAdj(DAMAGE_ABS * -1);
                if (creatureDefendingPtrC->HealthCurrent() < 0)
                {
                    creatureDefendingPtrC->HealthCurrentSet(0);
                }

                auto const IS_ALREADY_DAZED{ creatureDefendingPtrC->HasCondition(creature::Conditions::Dazed) ||
                                             IsConditionContained(creature::Conditions::Dazed, hitInfoVec) };

                if (IS_ALREADY_UNCONSCIOUS == false)
                {
                    if (creatureDefendingPtrC->IsPlayerCharacter() && (creatureDefendingPtrC->HealthCurrent() <= 0))
                    {
                        creatureDefendingPtrC->HealthCurrentSet(1);

                        const creature::Conditions::Enum CONDITION_UNCON_ENUM{ creature::Conditions::Unconscious };
                        creatureDefendingPtrC->ConditionAdd(CONDITION_UNCON_ENUM);
                        conditionsAddedVec.push_back(CONDITION_UNCON_ENUM);

                        //remove the dazed condition if already there
                        if (IS_ALREADY_DAZED)
                        {
                            creatureDefendingPtrC->ConditionRemove(creature::Conditions::Dazed);

                            for (auto & nextHitInfo : hitInfoVec)
                            {
                                nextHitInfo.RemoveCondition(creature::Conditions::Dazed);
                            }
                        }
                    }
                    else
                    {
                        if (IS_ALREADY_DAZED == false)
                        {
                            auto halfHealthNormal{ creatureDefendingPtrC->HealthNormal() / 2 };
                            if (0 == halfHealthNormal)
                            {
                                halfHealthNormal = 1;
                            }

                            if (DAMAGE_ABS >= halfHealthNormal)
                            {
                                const creature::Conditions::Enum CONDITION_DAZED_ENUM{ creature::Conditions::Dazed };
                                creatureDefendingPtrC->ConditionAdd(CONDITION_DAZED_ENUM);
                                conditionsAddedVec.push_back(CONDITION_DAZED_ENUM);
                            }
                        }
                    }
                }
            }
        }

        return conditionsAddedVec;
    }


    const HitInfoVec_t FightClub::AttackWithAllWeapons(creature::CreaturePtrC_t creatureAttackingPtrC,
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

                if (NEXT_HIT_INFO.ContainsCondition(creature::Conditions::Dead) ||
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
    const HitInfo FightClub::AttackWithSingleWeapon(HitInfoVec_t &           hitInfoVec,
                                                    const item::ItemPtr_t    WEAPON_PTR,
                                                    creature::CreaturePtrC_t creatureAttackingPtrC,
                                                    creature::CreaturePtrC_t creatureDefendingPtrC,
                                                    const bool               WILL_FORCE_HIT)
    {
        auto hasHitBeenDetermined{ false };
        auto wasHit{ false };
        auto hitType{ HitType::Count };
        auto dodgeType{ DodgeType::Count };

        auto const ATTACK_ACC_RAW{ creatureAttackingPtrC->Stats().Acc().Current() };
        auto attackAccToUse{ ATTACK_ACC_RAW };

        //If the attacking creature is an archer who is using a projectile weapon to attack with,
        //then add a 20% accuracy bonus plus half the attacker's Rank.
        if ((creatureAttackingPtrC->Role().Which() == creature::role::Archer) &&
            (WEAPON_PTR->WeaponType() & item::weapon_type::Projectile))
        {
            attackAccToUse += static_cast<stats::Stat_t>(static_cast<float>(ATTACK_ACC_RAW) *
                STAT_RATIO_AMAZING_);

            attackAccToUse += static_cast<int>(creatureAttackingPtrC->Rank()) / 2;
        }

        auto const ATTACK_ACC_RAND_MIN{ stats::Stat::Reduce(attackAccToUse) };
        auto const ATTACK_ACC_RAND_MAX{ std::max(ATTACK_ACC_RAW, attackAccToUse) };
        auto const ATTACK_ACC_RAND{ misc::random::Int(ATTACK_ACC_RAND_MIN, ATTACK_ACC_RAND_MAX) };

        auto const IS_ATTACK_AMAZING_ACC{
            (attackAccToUse >= STAT_HIGHER_THAN_AVERAGE_) &&
            IsValuetHigherThanRatioOfStat(ATTACK_ACC_RAND, attackAccToUse, STAT_RATIO_AMAZING_)};

        auto const DEFEND_SPD_RAW{ creatureDefendingPtrC->Stats().Spd().Current()};
        auto defendSpdToUse{ DEFEND_SPD_RAW };

        //If the defending creature is a Pixie then add 20% speed bonus plus half defender's rank.
        if (creatureDefendingPtrC->IsPixie())
        {
            defendSpdToUse += static_cast<stats::Stat_t>(static_cast<float>(DEFEND_SPD_RAW) *
                STAT_RATIO_AMAZING_);

            defendSpdToUse += static_cast<int>(creatureDefendingPtrC->Rank()) / 2;
        }

        auto const DEFEND_SPD_RAND_MIN{ stats::Stat::Reduce(defendSpdToUse) };
        auto const DEFEND_SPD_RAND_MAX{ std::max(DEFEND_SPD_RAW, defendSpdToUse) };
        auto const DEFEND_SPD_RAND{ misc::random::Int(DEFEND_SPD_RAND_MIN, DEFEND_SPD_RAND_MAX) };
       
        auto const IS_DEFENSE_AMAZING_DODGE{
            (defendSpdToUse >= STAT_HIGHER_THAN_AVERAGE_) &&
            IsValuetHigherThanRatioOfStat(DEFEND_SPD_RAND, defendSpdToUse, STAT_RATIO_AMAZING_)};

        if (IS_ATTACK_AMAZING_ACC && (IS_DEFENSE_AMAZING_DODGE == false))
        {
            //This case allows any attacker the rare chance to hit, even when the odds are
            //overwhelmingly bad, such as when the attackers accuracy max is less than the
            //defender's speed min.
            wasHit = true;
            hitType = HitType::AmazingAccuracy;
            hasHitBeenDetermined = true;
        }
        else if ((IS_ATTACK_AMAZING_ACC == false) && IS_DEFENSE_AMAZING_DODGE)
        {
            //This case allows any defender the rare chance to dodge even if the odds are
            //overwhelmingly bad, such as when the defender's speed max is less then the
            //attacker's accuracy min.
            wasHit = false;
            dodgeType = DodgeType::AmazingSpeed;
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
                hitType = HitType::Accuracy;
                hasHitBeenDetermined = true;
            }
            else if (ATTACK_ACC_RANK_ADJ < DEFEND_SPD_RANK_ADJ)
            {
                wasHit = false;
                dodgeType = DodgeType::Speed;
                hasHitBeenDetermined = true;
            }
        }

        //In this case both the rand "rolls" and the stat+rank are equal,
        //so determine if the attack hit or miss based on luck.
        if (false == hasHitBeenDetermined)
        {
            auto const ATTACK_LCK{ creatureAttackingPtrC->Stats().Lck().Current() };
            auto const ATTACK_LCK_RAND_MIN{ stats::Stat::Reduce(ATTACK_LCK) };
            auto const ATTACK_LCK_RAND_MAX{ ATTACK_LCK };
            auto const ATTACK_LCK_RAND{ misc::random::Int(ATTACK_LCK_RAND_MIN,
                                                            ATTACK_LCK_RAND_MAX) };

            auto const IS_ATTACK_AMAZING_LCK{
                (ATTACK_LCK >= STAT_HIGHER_THAN_AVERAGE_) &&
                IsValuetHigherThanRatioOfStat(ATTACK_LCK_RAND,
                                                ATTACK_LCK,
                                                STAT_RATIO_AMAZING_) };

            auto const DEFEND_LCK{ creatureDefendingPtrC->Stats().Lck().Current() };
            auto const DEFEND_LCK_RAND_MIN{ stats::Stat::Reduce(DEFEND_LCK) };
            auto const DEFEND_LCK_RAND_MAX{ DEFEND_LCK };
            auto const DEFEND_LCK_RAND{ misc::random::Int(DEFEND_LCK_RAND_MIN,
                                                            DEFEND_LCK_RAND_MAX) };

            auto const IS_DEFEND_AMAZING_LCK{
                (DEFEND_LCK >= STAT_HIGHER_THAN_AVERAGE_) &&
                IsValuetHigherThanRatioOfStat(DEFEND_LCK_RAND,
                                                DEFEND_LCK,
                                                STAT_RATIO_AMAZING_) };

            if (IS_ATTACK_AMAZING_LCK && (IS_DEFEND_AMAZING_LCK == false))
            {
                wasHit = true;
                hitType = HitType::Luck;
                hasHitBeenDetermined = true;
            }
            else if ((IS_ATTACK_AMAZING_LCK == false) && IS_DEFEND_AMAZING_LCK)
            {
                wasHit = false;
                dodgeType = DodgeType::Luck;
                hasHitBeenDetermined = true;
            }
            else
            {
                //In this case, neither attaker or defender had amazing luck,
                //so the hit is determined by who has the greater lck+rank.
                auto const ATTACK_LCK_RANK_ADJ{ ATTACK_LCK_RAND +
                    static_cast<int>(creatureAttackingPtrC->Rank()) };

                auto const DEFEND_LCK_RANK_ADJ{ DEFEND_LCK_RAND +
                    static_cast<int>(creatureDefendingPtrC->Rank()) };

                if (ATTACK_LCK_RANK_ADJ > DEFEND_LCK_RANK_ADJ)
                {
                    wasHit = true;
                    hitType = HitType::Accuracy;
                    hasHitBeenDetermined = true;
                }
                else if (ATTACK_LCK_RANK_ADJ < DEFEND_LCK_RANK_ADJ)
                {
                    wasHit = false;
                    dodgeType = DodgeType::Speed;
                    hasHitBeenDetermined = true;
                }
            }
        }

        //In this case, everything that could be equal WAS equal, so determine hit
        //based on 'fair coin toss'.
        if (false == hasHitBeenDetermined)
        {
            if (misc::random::Bool())
            {
                wasHit = true;
                hitType = HitType::AmazingLuck;
            }
            else
            {
                wasHit = false;
                dodgeType = DodgeType::AmazingLuck;
            }

            hasHitBeenDetermined = true;
        }

       
        //handle forced hit
        if (WILL_FORCE_HIT && (false == wasHit))
        {
            wasHit = true;
            dodgeType = DodgeType::Count;

            if (HitType::Count == hitType)
            {
                hitType = HitType::Accuracy;
            }

            hasHitBeenDetermined = true;
        }

        stats::Health_t damage{ 0 };
        auto isPowerHit{ false };
        auto isCriticalHit{ false };
        creature::ConditionEnumVec_t conditionsVec;
        if (wasHit)
        {
            damage = DetermineDamage(WEAPON_PTR,
                                     creatureAttackingPtrC,
                                     creatureDefendingPtrC,
                                     isPowerHit,
                                     isCriticalHit);

            conditionsVec = HandleDamage(creatureDefendingPtrC, hitInfoVec, damage);
        }

        return HitInfo(WEAPON_PTR,
                       hitType,
                       dodgeType,
                       damage,
                       isCriticalHit,
                       isPowerHit,
                       conditionsVec,
                       Text::WeaponActionVerb(WEAPON_PTR, false));
    }


    const HitInfo FightClub::CastSpellUpon(
        HitInfoVec_t &                       hitInfoVec,
        const spell::SpellPtr_t              SPELL_CPTR,
        const std::string &                  EFFECT_STR,
        creature::CreaturePtrC_t             creatureCastingPtrC,
        creature::CreaturePtrC_t             creatureCastUponPtrC,
        const stats::Health_t                HEALTH_ADJ,
        const creature::ConditionEnumVec_t & CONDITIONS_VEC)
    {
        auto const CONDS_FROM_DAMAGE{ HandleDamage(creatureCastUponPtrC, hitInfoVec, HEALTH_ADJ) };
       
        return HitInfo(SPELL_CPTR,
                       EFFECT_STR,
                       HEALTH_ADJ,
                       misc::Vector::And(CONDITIONS_VEC, CONDS_FROM_DAMAGE),
                       SPELL_CPTR->ActionPhrase(creatureCastingPtrC,
                                                creatureCastUponPtrC));
    }


    stats::Health_t FightClub::DetermineDamage(const item::ItemPtr_t    WEAPON_PTR,
                                               creature::CreaturePtrC_t creatureAttackingPtrC,
                                               creature::CreaturePtrC_t creatureDefendingPtrC,
                                               bool &                   isPowerHit_OutParam,
                                               bool &                   isCriticalHit_OutParam)
    {
        const stats::Health_t DAMAGE_FROM_WEAPON{ misc::random::Int(WEAPON_PTR->DamageMin(),
                                                                    WEAPON_PTR->DamageMax()) };

        //add extra damage based on rank
        auto const RANK_DIVISOR{ 3 };
        const stats::Health_t DAMAGE_FROM_RANK{
            static_cast<stats::Health_t>(creatureAttackingPtrC->Rank()) / RANK_DIVISOR };

        //add extra damage based on the attacking creature's strength
        auto const STAT_FLOOR{ 10 };
        auto strAdj{creatureAttackingPtrC->Stats().Str().Current() - STAT_FLOOR };
        if (strAdj < 0)
        {
            strAdj = 0;
        }
        const stats::Health_t DAMAGE_FROM_STRENGTH{ stats::Stat::Reduce(strAdj) };

        const stats::Health_t DAMAGE_BASE{
            DAMAGE_FROM_WEAPON + DAMAGE_FROM_RANK + DAMAGE_FROM_STRENGTH };

        //there is a rare chance of a power hit for players
        auto const STRENGTH{ creatureAttackingPtrC->Stats().Str().Current() };
        auto const STRENGTH_RAND_MIN{ stats::Stat::Reduce(STRENGTH) };
        auto const STRENGTH_RAND_MAX{ STRENGTH };
        auto const STRENGTH_RAND{ misc::random::Int(STRENGTH_RAND_MIN, STRENGTH_RAND_MAX) };

        //low str can mean greater chance of power hit so compensate here
        isPowerHit_OutParam = ((creatureAttackingPtrC->IsPlayerCharacter()) &&
                               (STRENGTH_RAND == STRENGTH_RAND_MAX) &&
                               ((STRENGTH >= STAT_FLOOR) ||
                                    (misc::random::Int(STAT_FLOOR / 2) == 0)));

        //there is a rare chance of a critical hit for players
        auto const ACC{ creatureAttackingPtrC->Stats().Acc().Current() };
        auto const ACC_RAND_MIN{ stats::Stat::Reduce(ACC) };
        auto const ACC_RAND_MAX{ ACC };
        auto const ACC_RAND{ misc::random::Int(ACC_RAND_MIN, ACC_RAND_MAX) };

        //low acc can mean greater chance of critical hit so compensate here
        isCriticalHit_OutParam = ((creatureAttackingPtrC->IsPlayerCharacter()) &&
                                  (ACC_RAND == ACC_RAND_MAX) &&
                                  ((ACC >= STAT_FLOOR) ||
                                    (misc::random::Int(STAT_FLOOR / 2) == 0)));

        stats::Health_t damageFinal{ DAMAGE_BASE };

        if (isPowerHit_OutParam)
        {
            damageFinal += DAMAGE_FROM_STRENGTH;
        }

        if (isCriticalHit_OutParam)
        {
            damageFinal += DAMAGE_FROM_WEAPON;
        }

        //reduce damage based on the defending creature's armor
        if (creatureDefendingPtrC->ArmorRating() >= (damageFinal * 4))
        {
            damageFinal = 0;
        }
        else if (damageFinal > creatureDefendingPtrC->ArmorRating())
        {
            damageFinal -= creatureDefendingPtrC->ArmorRating();
        }
        else
        {
            damageFinal = static_cast<stats::Health_t>( static_cast<float>(damageFinal) * (static_cast<float>(damageFinal) / static_cast<float>(creatureDefendingPtrC->ArmorRating())));
        }

        if (damageFinal > 0)
        {
            //pixies are dealt less damage than other creatures because of their small size and speed
            if (creatureDefendingPtrC->Race().Which() == creature::race::Pixie)
            {
                const stats::Health_t PIXIE_HEALTH_DIVISOR{ 5 };
                if (damageFinal < (PIXIE_HEALTH_DIVISOR * 2))
                {
                    damageFinal = 1;
                }
                else
                {
                    damageFinal = static_cast<stats::Health_t>(static_cast<float>(damageFinal) / static_cast<float>(PIXIE_HEALTH_DIVISOR));
                }
            }
        }
        else
        {
            damageFinal = 0;
        }

        return damageFinal * -1;
    }



    const FightResult FightClub::Cast(const spell::SpellPtr_t          SPELL_CPTR,
                                      creature::CreaturePtrC_t         creatureCastingPtr,
                                      const creature::CreaturePVec_t & creaturesCastUponPVec)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((creaturesCastUponPVec.empty() == false),
            "game::combat::FightClub::Cast(spell=" << SPELL_CPTR->Name() << ", creature_casting="
            << creatureCastingPtr->NameAndRaceAndRole() << ", creatures_cast_upon=empty) was "
            << "given an empty creaturesCastUponPVec.");

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
            auto const HEALTH_ADJ{((nextCreatureCastUponPtr->IsAlive()) ?
                SPELL_CPTR->HealthAdj(creatureCastingPtr, nextCreatureCastUponPtr) : 0) };

            creature::ConditionEnumVec_t conditionsVec;
            auto const SPELL_RESULT_STR{ SPELL_CPTR->EffectCreature(creatureCastingPtr,
                                                                    nextCreatureCastUponPtr,
                                                                    conditionsVec) };

            HitInfoVec_t hitInfoVec;
            hitInfoVec.push_back( CastSpellUpon(hitInfoVec,
                                               SPELL_CPTR,
                                               SPELL_RESULT_STR,
                                               creatureCastingPtr,
                                               nextCreatureCastUponPtr,
                                               HEALTH_ADJ,
                                               conditionsVec) );

            creatureEffectVec.push_back(
                CreatureEffect(nextCreatureCastUponPtr, hitInfoVec, SPELL_CPTR) );

            //TODO Handle Encounter::Instance()->TurnInfo
        }

        return FightResult(creatureEffectVec);
    }


    const FightResult FightClub::Pounce(creature::CreaturePtrC_t creaturePouncingPtrC,
                                        creature::CreaturePtrC_t creatureDefendingPtrC)
    {
        auto const POUNCER_SPD{ creaturePouncingPtrC->Stats().Spd().Current() };
        auto const POUNCER_SPD_RAND_MIN{ stats::Stat::Reduce(POUNCER_SPD) };
        auto const POUNCER_SPD_RAND_MAX{ POUNCER_SPD };
        auto const POUNCER_SPD_RAND{ misc::random::Int(POUNCER_SPD_RAND_MIN, POUNCER_SPD_RAND_MAX) };

        auto const POUNCER_ACC{ creaturePouncingPtrC->Stats().Acc().Current() };
        auto const POUNCER_ACC_RAND_MIN{ stats::Stat::Reduce(POUNCER_ACC) };
        auto const POUNCER_ACC_RAND_MAX{ POUNCER_ACC };
        auto const POUNCER_ACC_RAND{ misc::random::Int(POUNCER_ACC_RAND_MIN, POUNCER_ACC_RAND_MAX) };

        auto didPounce{ false };

        if ((POUNCER_SPD_RAND == POUNCER_SPD_RAND_MAX) || (POUNCER_ACC_RAND == POUNCER_ACC_RAND_MAX))
        {
            //if either of the pouncer's attribute "rolls" were natural max then the pounce occurs
            didPounce = true;
        }
        else if ((creatureDefendingPtrC->HasConditionNotAThreatTemp() || creatureDefendingPtrC->HasConditionNotAThreatPerm()) &&
                 (POUNCER_ACC_RAND != POUNCER_ACC_RAND_MIN))
        {
            //if the defender is temp disabled then the pounce succeeds unless the accuracy "roll" was natural min
            didPounce = true;
        }
        else
        {
            auto const DEFENDER_SPD{ creatureDefendingPtrC->Stats().Spd().Current() };
            auto const DEFENDER_SPD_RAND_MIN{ stats::Stat::Reduce(DEFENDER_SPD) };
            auto const DEFENDER_SPD_RAND_MAX{ DEFENDER_SPD };
            auto const DEFENDER_SPD_RAND{ misc::random::Int(DEFENDER_SPD_RAND_MIN, DEFENDER_SPD_RAND_MAX) };

            if (((POUNCER_SPD_RAND + static_cast<int>(creaturePouncingPtrC->Rank())) > (DEFENDER_SPD_RAND + static_cast<int>(creatureDefendingPtrC->Rank()))) ||
                ((POUNCER_ACC_RAND + static_cast<int>(creaturePouncingPtrC->Rank())) > (DEFENDER_SPD_RAND + static_cast<int>(creatureDefendingPtrC->Rank()))))
            {
                //if the pouncer's speed or accuracy "rolls" are greater than the defender's speed "roll" then the pouce occurs
                didPounce = true;
            }
        }

        if (didPounce)
        {
            creature::ConditionEnumVec_t nonHitConditionsVec;
            if (creatureDefendingPtrC->HasCondition(creature::Conditions::Tripped) == false)
            {
                auto const CONDITION_TRIPPED{ creature::Conditions::Tripped };
                creatureDefendingPtrC->ConditionAdd(CONDITION_TRIPPED);
                nonHitConditionsVec.push_back(CONDITION_TRIPPED);
            }

            auto const HIT_INFO_VEC{ Fight(creaturePouncingPtrC, creatureDefendingPtrC, false).FirstEffect().GetHitInfoVec() };
            return FightResult( CreatureEffect(creatureDefendingPtrC, HIT_INFO_VEC, nullptr, nonHitConditionsVec, true) );
        }
        else
        {
            return FightResult( CreatureEffect(creatureDefendingPtrC) );
        }
    }


    const FightResult FightClub::Roar(creature::CreaturePtrC_t creatureRoaringPtrC,
                                      CombatDisplayCPtrC_t     COMBAT_DISPLAY_CPTRC)
    {
        auto const LIVING_OPPONENT_CREATURES_PVEC{ creature::Algorithms::PlayersByType( ! creatureRoaringPtrC->IsPlayerCharacter(), true) };

        auto const ROARER_STRENGTH{ creatureRoaringPtrC->Stats().Str().Current() };
        auto const ROARER_STRENGTH_RAND_MIN{ stats::Stat::Reduce(ROARER_STRENGTH) };
        auto const ROARER_STRENGTH_RAND_MAX{ ROARER_STRENGTH };

        auto const ROARER_LCK{ creatureRoaringPtrC->Stats().Lck().Current() };
        auto const ROARER_LCK_RAND_MIN_HALF((ROARER_LCK / 5));
        auto const ROARER_LCK_RAND_MAX_HALF{ ROARER_LCK / 2 };

        auto const IS_ROARING_CREATURE_FLYING{ Encounter::Instance()->GetTurnInfoCopy(creatureRoaringPtrC).GetIsFlying() };

        auto const ROARING_FRIGHTEN_ROLL_MIN{ ROARER_STRENGTH_RAND_MIN + ROARER_LCK_RAND_MIN_HALF };
        auto const ROARING_FRIGHTEN_ROLL_MAX{ ROARER_STRENGTH_RAND_MAX + ROARER_LCK_RAND_MAX_HALF };
        auto const ROARING_CREATURE_FRIGHTEN_ROLL{ misc::random::Int(ROARING_FRIGHTEN_ROLL_MIN, ROARING_FRIGHTEN_ROLL_MAX)};

        CreatureEffectVec_t creatureEffectsVec;

        //Give each defending creature a chance to resist being panicked.
        //The farther away each defending creature is the better chance of resisting he/she/it has.
        for (auto const NEXT_DEFEND_CREATURE_PTR : LIVING_OPPONENT_CREATURES_PVEC)
        {
            if (NEXT_DEFEND_CREATURE_PTR->HasCondition(creature::Conditions::Panic))
            {
                continue;
            }

            auto nextBlockingDisatnce{ std::abs(COMBAT_DISPLAY_CPTRC->GetBlockingDistanceBetween(creatureRoaringPtrC, NEXT_DEFEND_CREATURE_PTR)) };

            //if flying, then consider it farther away and less likely to be panicked
            if (Encounter::Instance()->GetTurnInfoCopy(NEXT_DEFEND_CREATURE_PTR).GetIsFlying() &&
                (IS_ROARING_CREATURE_FLYING == false))
            {
                ++nextBlockingDisatnce;
            }

            auto const DISATANCE_OFFSET{ nextBlockingDisatnce * 5 };

            auto const DEFENDER_INTELLIGENCE{ creatureRoaringPtrC->Stats().Int().Current() };
            auto const DEFENDER_INTELLIGENCE_RAND_MIN{ stats::Stat::Reduce(DEFENDER_INTELLIGENCE) };
            auto const DEFENDER_INTELLIGENCE_RAND_MAX{ DEFENDER_INTELLIGENCE };

            auto const DEFENDER_LCK{ creatureRoaringPtrC->Stats().Lck().Current() };
            auto const DEFENDER_LCK_RAND_MIN_HALF((DEFENDER_LCK / 5));
            auto const DEFENDER_LCK_RAND_MAX_HALF{ DEFENDER_LCK / 2 };

            auto const DEFEND_FRIGHTEN_ROLL_MIN{ DEFENDER_INTELLIGENCE_RAND_MIN + DEFENDER_LCK_RAND_MIN_HALF + DISATANCE_OFFSET };
            auto const DEFEND_FRIGHTEN_ROLL_MAX{ DEFENDER_INTELLIGENCE_RAND_MAX + DEFENDER_LCK_RAND_MAX_HALF + DISATANCE_OFFSET };
            auto const DEFEND_CREATURE_FRIGHTEN_RESIST_ROLL{ misc::random::Int(DEFEND_FRIGHTEN_ROLL_MIN, DEFEND_FRIGHTEN_ROLL_MAX) };

            auto didFrighten{ false };
            if ((ROARING_CREATURE_FRIGHTEN_ROLL == ROARING_FRIGHTEN_ROLL_MAX) && (DEFEND_CREATURE_FRIGHTEN_RESIST_ROLL != DEFEND_FRIGHTEN_ROLL_MAX))
            {
                //if the roaring creature's roll was max and the defending creature's was not, then frighten
                didFrighten = true;
            }
            else if ((ROARING_CREATURE_FRIGHTEN_ROLL != ROARING_FRIGHTEN_ROLL_MAX) && (DEFEND_CREATURE_FRIGHTEN_RESIST_ROLL == DEFEND_FRIGHTEN_ROLL_MAX))
            {
                //if the defending creature's roll was max and the roaring creature's was not, then don't frighten
                didFrighten = false;
            }
            else if ((ROARING_CREATURE_FRIGHTEN_ROLL + static_cast<int>(creatureRoaringPtrC->Rank())) == (DEFEND_CREATURE_FRIGHTEN_RESIST_ROLL + static_cast<int>(NEXT_DEFEND_CREATURE_PTR->Rank())))
            {
                //if rank offset rolls are equal, then decide if frightened by fair coint toss
                didFrighten = misc::random::Bool();
            }
            else if ((ROARING_CREATURE_FRIGHTEN_ROLL + static_cast<int>(creatureRoaringPtrC->Rank())) > (DEFEND_CREATURE_FRIGHTEN_RESIST_ROLL + static_cast<int>(NEXT_DEFEND_CREATURE_PTR->Rank())))
            {
                //if the roaring creature's rolls offset with rank was higher than the defending creature's rank offset roll, then frighten
                didFrighten = true;
            }

            if (didFrighten)
            {
                auto const CONDITION_VEC{ creature::Conditions::Panic };

                NEXT_DEFEND_CREATURE_PTR->ConditionAdd(CONDITION_VEC);

                creature::ConditionEnumVec_t nonHitConditionsVec;
                nonHitConditionsVec.push_back(CONDITION_VEC);

                creatureEffectsVec.push_back( CreatureEffect(NEXT_DEFEND_CREATURE_PTR, HitInfoVec_t(), nullptr, nonHitConditionsVec) );
            }
        }

        return FightResult(creatureEffectsVec);
    }


    creature::CreaturePtr_t FightClub::FindNonPlayerCreatureToAttack(creature::CreaturePtrC_t creatureAttackingtrC,
                                                                     CombatDisplayCPtrC_t     COMBAT_DISPLAY_CPTRC)
    {
        creature::CreaturePVec_t attackableNonPlayerCreaturesPVec;
        COMBAT_DISPLAY_CPTRC->FindCreaturesThatCanBeAttackedOfType(attackableNonPlayerCreaturesPVec, creatureAttackingtrC, false);

        M_ASSERT_OR_LOGANDTHROW_SS((attackableNonPlayerCreaturesPVec.empty() == false), "game::combat::FightClub::HandleAttack() FindNonPlayerCreatureToAttack() returned no attackable creatures.");

        auto const LIVING_ATTACKABLE_NONPLAYER_CREATURES_PVEC{ creature::Algorithms::FindByAlive(attackableNonPlayerCreaturesPVec) };

        M_ASSERT_OR_LOGANDTHROW_SS((LIVING_ATTACKABLE_NONPLAYER_CREATURES_PVEC.empty() == false), "game::combat::FightClub::HandleAttack() FindNonPlayerCreatureToAttack() returned no LIVING attackable creatures.");

        //attack those with the lowest relative health first, which will correspond to the health bar seen on screen
        auto const LIVING_ATTACKABLE_LOWESTHEALTHRATIO_NONPLAYER_CREATURES_PVEC{ creature::Algorithms::FindLowestHealthRatio(LIVING_ATTACKABLE_NONPLAYER_CREATURES_PVEC) };

        M_ASSERT_OR_LOGANDTHROW_SS((LIVING_ATTACKABLE_LOWESTHEALTHRATIO_NONPLAYER_CREATURES_PVEC.empty() == false), "game::combat::FightClub::HandleAttack() FindNonPlayerCreatureToAttack() returned no LIVING LOWEST HEALTH RATIO attackable creatures.");

        //skip creatures who are not a threat
        auto const LIVING_ATTACKABLE_LOWESTHEALTHRATIO_NONPLAYER_NOTPERMDISABLED_CREATURES_PVEC{ creature::Algorithms::FindByConditionMeaningNotAThreatPermenantly(LIVING_ATTACKABLE_LOWESTHEALTHRATIO_NONPLAYER_CREATURES_PVEC, false) };

        //attack those that are temporarily disabled first
        auto const LIVING_ATTACKABLE_LOWESTHEALTHRATIO_NONPLAYER_NOTPERMDISABLED_TEMPDISABLED_CREATURES_PVEC{ creature::Algorithms::FindByConditionMeaningNotAThreatTemporarily(LIVING_ATTACKABLE_LOWESTHEALTHRATIO_NONPLAYER_NOTPERMDISABLED_CREATURES_PVEC) };

        if (LIVING_ATTACKABLE_LOWESTHEALTHRATIO_NONPLAYER_NOTPERMDISABLED_TEMPDISABLED_CREATURES_PVEC.empty())
        {
            if (LIVING_ATTACKABLE_LOWESTHEALTHRATIO_NONPLAYER_NOTPERMDISABLED_CREATURES_PVEC.empty())
            {
                return misc::Vector::SelectRandom(COMBAT_DISPLAY_CPTRC->FindClosestAmongOfType(creatureAttackingtrC, LIVING_ATTACKABLE_LOWESTHEALTHRATIO_NONPLAYER_CREATURES_PVEC, false));
            }
            else
            {
                return misc::Vector::SelectRandom(COMBAT_DISPLAY_CPTRC->FindClosestAmongOfType(creatureAttackingtrC, LIVING_ATTACKABLE_LOWESTHEALTHRATIO_NONPLAYER_NOTPERMDISABLED_CREATURES_PVEC, false));
            }
        }
        else
        {
            return misc::Vector::SelectRandom(COMBAT_DISPLAY_CPTRC->FindClosestAmongOfType(creatureAttackingtrC, LIVING_ATTACKABLE_LOWESTHEALTHRATIO_NONPLAYER_NOTPERMDISABLED_TEMPDISABLED_CREATURES_PVEC, false));
        }
    }


    bool FightClub::IsConditionContained(const creature::Conditions::Enum E, const HitInfoVec_t & HIT_INFO_VEC)
    {
        for (auto const & NEXT_HIT_INFO : HIT_INFO_VEC)
        {
            if (NEXT_HIT_INFO.ContainsCondition(E))
            {
                return true;
            }
        }

        return false;
    }

}
}
