//
// fight.cpp
//
#include "fight.hpp"

#include "sfml-util/random.hpp"

#include "heroespath/creature/creature.hpp"
#include "heroespath/creature/conditions.hpp"
#include "heroespath/creature/algorithms.hpp"
#include "heroespath/item/item.hpp"
#include "heroespath/spell/spell-base.hpp"
#include "heroespath/combat/combat-display.hpp"
#include "heroespath/combat/encounter.hpp"
#include "heroespath/combat/combat-text.hpp"


namespace heroespath
{
namespace combat
{

    const float FightClub::STAT_MIN_DIVISOR_    (5.0f);
    const float FightClub::STAT_MIN_MULTIPLIER_ (2.0f);


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


    const creature::ConditionSVec_t FightClub::HandleDamage(creature::CreaturePtrC_t creatureDefendingPtrC,
                                                            HitInfoVec_t &           hitInfoVec,
                                                            const stats::Health_t    TOTAL_DAMAGE)
    {
        creature::ConditionSVec_t conditionsAddedSVec;

        auto const IS_ALREADY_KILLED{ creatureDefendingPtrC->HasCondition(creature::condition::Dead) ||
                                      IsConditionContained(creature::condition::Dead, hitInfoVec) };

        if (IS_ALREADY_KILLED == false)
        {
            auto willKill{ false };

            auto const IS_ALREADY_UNCONSCIOUS{ creatureDefendingPtrC->HasCondition(creature::condition::Unconscious) ||
                                               IsConditionContained(creature::condition::Unconscious, hitInfoVec) };

            if ((IS_ALREADY_UNCONSCIOUS) && (TOTAL_DAMAGE > 0))
            {
                willKill = true;
            }
            else if ((creatureDefendingPtrC->IsPlayerCharacter() == false) &&
                     ((TOTAL_DAMAGE > (creatureDefendingPtrC->HealthNormal() * 2) || (TOTAL_DAMAGE > creatureDefendingPtrC->HealthCurrent()))))
            {
                willKill = true;
            }

            if (willKill)
            {
                creatureDefendingPtrC->HealthCurrentSet(0);

                const creature::ConditionSPtr_t CONDITION_DEAD_SPTRC{ creature::condition::ConditionFactory::Make(creature::condition::Dead) };
                creatureDefendingPtrC->ConditionAdd(CONDITION_DEAD_SPTRC);
                conditionsAddedSVec.push_back(CONDITION_DEAD_SPTRC);

                //remove the unconscious condition if already there
                if (IS_ALREADY_UNCONSCIOUS)
                {
                    creatureDefendingPtrC->ConditionRemove(creature::condition::Unconscious);

                    for (auto & nextHitInfo : hitInfoVec)
                        nextHitInfo.RemoveCondition(creature::condition::Unconscious);
                }
            }
            else
            {
                creatureDefendingPtrC->HealthCurrentAdj(TOTAL_DAMAGE * -1);
                if (creatureDefendingPtrC->HealthCurrent() < 0)
                {
                    creatureDefendingPtrC->HealthCurrentSet(0);
                }

                auto const IS_ALREADY_DAZED{ creatureDefendingPtrC->HasCondition(creature::condition::Dazed) ||
                                             IsConditionContained(creature::condition::Dazed, hitInfoVec) };

                if (IS_ALREADY_UNCONSCIOUS == false)
                {
                    if (creatureDefendingPtrC->IsPlayerCharacter() && (creatureDefendingPtrC->HealthCurrent() <= 0))
                    {
                        creatureDefendingPtrC->HealthCurrentSet(1);

                        const creature::ConditionSPtr_t CONDITION_UNCON_SPTR{ creature::condition::ConditionFactory::Make(creature::condition::Unconscious) };
                        creatureDefendingPtrC->ConditionAdd(CONDITION_UNCON_SPTR);
                        conditionsAddedSVec.push_back(CONDITION_UNCON_SPTR);

                        //remove the dazed condition if already there
                        if (IS_ALREADY_DAZED)
                        {
                            creatureDefendingPtrC->ConditionRemove(creature::condition::Dazed);

                            for (auto & nextHitInfo : hitInfoVec)
                                nextHitInfo.RemoveCondition(creature::condition::Dazed);
                        }
                    }
                    else
                    {
                        if (IS_ALREADY_DAZED == false)
                        {
                            auto halfHealthNormal{ creatureDefendingPtrC->HealthNormal() / 2 };
                            if (0 == halfHealthNormal)
                                halfHealthNormal = 1;

                            if (TOTAL_DAMAGE >= halfHealthNormal)
                            {
                                const creature::ConditionSPtr_t CONDITION_DAZED_SPTR{ creature::condition::ConditionFactory::Make(creature::condition::Dazed) };
                                creatureDefendingPtrC->ConditionAdd(CONDITION_DAZED_SPTR);
                                conditionsAddedSVec.push_back(CONDITION_DAZED_SPTR);
                            }
                        }
                    }
                }
            }
        }

        return conditionsAddedSVec;
    }


    const HitInfoVec_t FightClub::AttackWithAllWeapons(creature::CreaturePtrC_t creatureAttackingPtrC,
                                                       creature::CreaturePtrC_t creatureDefendingPtrC,
                                                       const bool               WILL_FORCE_HIT)
    {
        HitInfoVec_t hitInfoVec;

        auto const WEAPONS_SVEC( creatureAttackingPtrC->CurrentWeaponsCopy() );
        for (auto const & NEXT_ITEM_SPTR : WEAPONS_SVEC)
        {
            if (NEXT_ITEM_SPTR->IsWeapon())
            {
                auto const NEXT_HIT_INFO{ AttackWithSingleWeapon(hitInfoVec,
                                                                 NEXT_ITEM_SPTR,
                                                                 creatureAttackingPtrC,
                                                                 creatureDefendingPtrC,
                                                                 WILL_FORCE_HIT) };

                hitInfoVec.push_back(NEXT_HIT_INFO);

                if (NEXT_HIT_INFO.ContainsCondition(creature::condition::Dead) || creatureDefendingPtrC->HasCondition(creature::condition::Dead))
                {
                    break;
                }
            }
        }

        return hitInfoVec;
    }


    const HitInfo FightClub::AttackWithSingleWeapon(HitInfoVec_t &           hitInfoVec,
                                                    const item::ItemSPtr_t & WEAPON_SPTR,
                                                    creature::CreaturePtrC_t creatureAttackingPtrC,
                                                    creature::CreaturePtrC_t creatureDefendingPtrC,
                                                    const bool               WILL_FORCE_HIT)
    {
        auto wasHit{ false };
        auto hitType{ HitType::Count };
        auto dodgeType{ DodgeType::Count };

        //determine if attacking creature's accuracy overcomes the defending creature's speed to see if there was a hit
        auto const ATTACKING_ACCURACY{ creatureAttackingPtrC->Stats().Acc().Current() };
        auto const ATTACKING_ACCURACY_RAND_MIN{ StatReduced(ATTACKING_ACCURACY) };
        auto const ATTACKING_ACCURACY_RAND_MAX{ ATTACKING_ACCURACY };
        auto const ATTACKING_ACCURACY_RAND{ sfml_util::rand::Int(ATTACKING_ACCURACY_RAND_MIN, ATTACKING_ACCURACY_RAND_MAX) };

        auto const DEFENDING_SPEED{ creatureDefendingPtrC->Stats().Spd().Current() };
        auto const DEFENDING_SPEED_RAND_MIN{ StatReduced(DEFENDING_SPEED) };
        auto const DEFENDING_SPEED_RAND_MAX{ DEFENDING_SPEED };
        auto const DEFENDING_SPEED_RAND{ sfml_util::rand::Int(DEFENDING_SPEED_RAND_MIN, DEFENDING_SPEED_RAND_MAX) };

        if ((ATTACKING_ACCURACY_RAND == ATTACKING_ACCURACY_RAND_MAX) && (DEFENDING_SPEED_RAND != DEFENDING_SPEED_RAND_MAX))
        {
            //This case allows any attacker the rare chance to hit, even when the odds are overwhelmingly bad,
            //such as when the attackers accuracy max is less than the defender's speed min
            wasHit = true;
            hitType = HitType::AmazingAccuracy;
        }
        else if ((ATTACKING_ACCURACY_RAND != ATTACKING_ACCURACY_RAND_MAX) && (DEFENDING_SPEED_RAND == DEFENDING_SPEED_RAND_MAX))
        {
            //This case allows any defender the rare chance to dodge even if the odds are overwhelmingly bad,
            //such as when the defender's speed max is less then the attacker's accuracy min.
            wasHit = false;
            dodgeType = DodgeType::AmazingSpeed;
        }
        else if (((ATTACKING_ACCURACY_RAND + static_cast<int>(creatureAttackingPtrC->Rank())) == (DEFENDING_SPEED_RAND + static_cast<int>(creatureDefendingPtrC->Rank()))) ||
                 ((ATTACKING_ACCURACY_RAND == ATTACKING_ACCURACY_RAND_MAX) && (DEFENDING_SPEED_RAND == DEFENDING_SPEED_RAND_MAX)))
        {
            //In this case, either both the attacker and defending did their best and tied for "rolling" their highest possible rand/score or the rand "rolled" scores were equal.
            //So the hit is determined by whose stat is higher.
            if (ATTACKING_ACCURACY > DEFENDING_SPEED)
            {
                wasHit = true;
                hitType = HitType::Accuracy;
            }
            else if (ATTACKING_ACCURACY < DEFENDING_SPEED)
            {
                wasHit = false;
                dodgeType = DodgeType::Speed;
            }
            else
            {
                //In this case both the rand "rolls" and the stat values are equal,
                //so determine if the attack hit or miss based on luck
                auto const ATTACKING_LUCK{ creatureAttackingPtrC->Stats().Lck().Current() };
                auto const ATTACKING_LUCK_RAND_MIN{ StatReduced(ATTACKING_LUCK) };
                auto const ATTACKING_LUCK_RAND_MAX{ ATTACKING_LUCK };
                auto const ATTACKING_LUCK_RAND{ sfml_util::rand::Int(ATTACKING_LUCK_RAND_MIN, ATTACKING_LUCK_RAND_MAX) };

                auto const DEFENDING_LUCK{ creatureDefendingPtrC->Stats().Lck().Current() };
                auto const DEFENDING_LUCK_RAND_MIN{ StatReduced(DEFENDING_LUCK) };
                auto const DEFENDING_LUCK_RAND_MAX{ DEFENDING_LUCK };
                auto const DEFENDING_LUCK_RAND{ sfml_util::rand::Int(DEFENDING_LUCK_RAND_MIN, DEFENDING_LUCK_RAND_MAX) };

                if (ATTACKING_LUCK_RAND > DEFENDING_LUCK_RAND)
                {
                    wasHit = true;
                    hitType = HitType::Luck;
                }
                else if (ATTACKING_LUCK_RAND < DEFENDING_LUCK_RAND)
                {
                    wasHit = false;
                    dodgeType = DodgeType::Luck;
                }
                else
                {
                    //In this case, everything that could be equal WAS equal, so determine hit based on fair coin toss.
                    if (sfml_util::rand::Bool())
                    {
                        wasHit = true;
                        hitType = HitType::AmazingLuck;
                    }
                    else
                    {
                        wasHit = false;
                        dodgeType = DodgeType::AmazingLuck;
                    }
                }
            }
        }
        else
        {
            auto attackingAccuracyToUse{ ATTACKING_ACCURACY_RAND + static_cast<stats::Stat_t>(creatureAttackingPtrC->Rank()) };

            //If the attacking creature is an archer who is using a projectile weapon to attack with,
            //then add a 20% accuracy bonus plus half the attacker's Rank.
            if ((creatureAttackingPtrC->Role().Which() == creature::role::Archer) &&
                (WEAPON_SPTR->WeaponType() & item::weapon_type::Projectile))
            {
                attackingAccuracyToUse += static_cast<stats::Stat_t>(static_cast<float>(ATTACKING_ACCURACY) * 0.2f);
                attackingAccuracyToUse += static_cast<stats::Stat_t>(static_cast<float>(creatureAttackingPtrC->Rank()) / 2.0f);
            }

            auto defendingSpeedToUse{ DEFENDING_SPEED_RAND + static_cast<int>(creatureDefendingPtrC->Rank()) };

            if ((attackingAccuracyToUse > defendingSpeedToUse) &&
                ((creatureDefendingPtrC->Race().Which() != creature::race::Pixie) || (sfml_util::rand::Bool())))
            {
                wasHit = true;
                hitType = HitType::Accuracy;
            }
            else
            {
                wasHit = false;
                dodgeType = DodgeType::Speed;
            }
        }

        //handle forced hit
        if (WILL_FORCE_HIT && (false == wasHit))
        {
            wasHit = true;
            dodgeType = DodgeType::Count;

            if (HitType::Count == hitType)
                hitType = HitType::Accuracy;
        }

        stats::Health_t damage{ 0 };
        auto isPowerHit{ false };
        auto isCriticalHit{ false };
        creature::ConditionSVec_t conditionsSVec;
        if (wasHit)
        {
            damage = DetermineDamage(WEAPON_SPTR, creatureAttackingPtrC, creatureDefendingPtrC, isPowerHit, isCriticalHit);
            conditionsSVec = HandleDamage(creatureDefendingPtrC, hitInfoVec, damage);
        }

        return HitInfo(WEAPON_SPTR, hitType, dodgeType, damage, isCriticalHit, isPowerHit, conditionsSVec, Text::WeaponActionVerb(WEAPON_SPTR, false));
    }


    stats::Health_t FightClub::DetermineDamage(const item::ItemSPtr_t & WEAPON_SPTR,
                                               creature::CreaturePtrC_t creatureAttackingPtrC,
                                               creature::CreaturePtrC_t creatureDefendingPtrC,
                                               bool &                   isPowerHit_OutParam,
                                               bool &                   isCriticalHit_OutParam)
    {
        const stats::Health_t DAMAGE_FROM_WEAPON{ sfml_util::rand::Int(WEAPON_SPTR->DamageMin(), WEAPON_SPTR->DamageMax()) };

        //add extra damage based on rank
        auto const RANK_DIVISOR{ 3 };
        const stats::Health_t DAMAGE_FROM_RANK{ static_cast<stats::Health_t>(creatureAttackingPtrC->Rank()) / RANK_DIVISOR };

        //add extra damage based on the attacking creature's strength
        auto const STAT_FLOOR{ 10 };
        auto strAdj{creatureAttackingPtrC->Stats().Str().Current() - STAT_FLOOR };
        if (strAdj < 0)
            strAdj = 0;
        const stats::Health_t DAMAGE_FROM_STRENGTH{ StatReduced(strAdj) };

        const stats::Health_t DAMAGE_BASE{ DAMAGE_FROM_WEAPON + DAMAGE_FROM_RANK + DAMAGE_FROM_STRENGTH };

        //there is a rare chance of a power hit for players
        auto const STRENGTH{ creatureAttackingPtrC->Stats().Str().Current() };
        auto const STRENGTH_RAND_MIN{ StatReduced(STRENGTH) };
        auto const STRENGTH_RAND_MAX{ STRENGTH };
        auto const STRENGTH_RAND{ sfml_util::rand::Int(STRENGTH_RAND_MIN, STRENGTH_RAND_MAX) };

        isPowerHit_OutParam = ((creatureAttackingPtrC->IsPlayerCharacter()) &&
                               (STRENGTH_RAND == STRENGTH_RAND_MAX) &&
                               ((STRENGTH >= STAT_FLOOR) || (sfml_util::rand::Int(STAT_FLOOR / 2) == 0)));//low str can mean greater chance of power hit so compensate here

        //there is a rare chance of a critical hit for players
        auto const ACCURACY{ creatureAttackingPtrC->Stats().Acc().Current() };
        auto const ACCURACY_RAND_MIN{ StatReduced(ACCURACY) };
        auto const ACCURACY_RAND_MAX{ ACCURACY };
        auto const ACCURACY_RAND{ sfml_util::rand::Int(ACCURACY_RAND_MIN, ACCURACY_RAND_MAX) };

        isCriticalHit_OutParam = ((creatureAttackingPtrC->IsPlayerCharacter()) &&
                                  (ACCURACY_RAND == ACCURACY_RAND_MAX) &&
                                  ((ACCURACY >= STAT_FLOOR) || (sfml_util::rand::Int(STAT_FLOOR / 2) == 0)));//low acc can mean greater chance of critical hit so compensate here

        stats::Health_t damageFinal{ DAMAGE_BASE };

        if (isPowerHit_OutParam)
            damageFinal += DAMAGE_BASE;

        if (isCriticalHit_OutParam)
            damageFinal += DAMAGE_BASE;

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

        return damageFinal;
    }



    const FightResult FightClub::Cast(spell::SpellCPtrC_t,
                                      creature::CreaturePtrC_t,
                                      creature::CreaturePtrC_t)
    {
        //TODO

        //TODO Handle Encounter::Instance()->TurnInfo
        return FightResult();
    }


    const FightResult FightClub::Pounce(creature::CreaturePtrC_t creaturePouncingPtrC,
                                        creature::CreaturePtrC_t creatureDefendingPtrC)
    {
        auto const POUNCER_SPEED{ creaturePouncingPtrC->Stats().Spd().Current() };
        auto const POUNCER_SPEED_RAND_MIN{ StatReduced(POUNCER_SPEED) };
        auto const POUNCER_SPEED_RAND_MAX{ POUNCER_SPEED };
        auto const POUNCER_SPEED_RAND{ sfml_util::rand::Int(POUNCER_SPEED_RAND_MIN, POUNCER_SPEED_RAND_MAX) };

        auto const POUNCER_ACCURACY{ creaturePouncingPtrC->Stats().Acc().Current() };
        auto const POUNCER_ACCURACY_RAND_MIN{ StatReduced(POUNCER_ACCURACY) };
        auto const POUNCER_ACCURACY_RAND_MAX{ POUNCER_ACCURACY };
        auto const POUNCER_ACCURACY_RAND{ sfml_util::rand::Int(POUNCER_ACCURACY_RAND_MIN, POUNCER_ACCURACY_RAND_MAX) };

        auto didPounce{ false };

        if ((POUNCER_SPEED_RAND == POUNCER_SPEED_RAND_MAX) || (POUNCER_ACCURACY_RAND == POUNCER_ACCURACY_RAND_MAX))
        {
            //if either of the pouncer's attribute "rolls" were natural max then the pounce occurs
            didPounce = true;
        }
        else if ((creatureDefendingPtrC->HasConditionNotAThreatTemp() || creatureDefendingPtrC->HasConditionNotAThreatPerm()) &&
                 (POUNCER_ACCURACY_RAND != POUNCER_ACCURACY_RAND_MIN))
        {
            //if the defender is temp disabled then the pounce succeeds unless the accuracy "roll" was natural min
            didPounce = true;
        }
        else
        {
            auto const DEFENDER_SPEED{ creatureDefendingPtrC->Stats().Spd().Current() };
            auto const DEFENDER_SPEED_RAND_MIN{ StatReduced(DEFENDER_SPEED) };
            auto const DEFENDER_SPEED_RAND_MAX{ DEFENDER_SPEED };
            auto const DEFENDER_SPEED_RAND{ sfml_util::rand::Int(DEFENDER_SPEED_RAND_MIN, DEFENDER_SPEED_RAND_MAX) };

            if (((POUNCER_SPEED_RAND + static_cast<int>(creaturePouncingPtrC->Rank())) > (DEFENDER_SPEED_RAND + static_cast<int>(creatureDefendingPtrC->Rank()))) ||
                ((POUNCER_ACCURACY_RAND + static_cast<int>(creaturePouncingPtrC->Rank())) > (DEFENDER_SPEED_RAND + static_cast<int>(creatureDefendingPtrC->Rank()))))
            {
                //if the pouncer's speed or accuracy "rolls" are greater than the defender's speed "roll" then the pouce occurs
                didPounce = true;
            }
        }

        if (didPounce)
        {
            creature::ConditionSVec_t nonHitConditionsSVec;
            if (creatureDefendingPtrC->HasCondition(creature::condition::Tripped) == false)
            {
                auto const CONDITION_TRIPPED{ creature::condition::ConditionFactory::Make(creature::condition::Tripped) };
                creatureDefendingPtrC->ConditionAdd(CONDITION_TRIPPED);
                nonHitConditionsSVec.push_back(CONDITION_TRIPPED);
            }

            auto const HIT_INFO_VEC{ Fight(creaturePouncingPtrC, creatureDefendingPtrC, false).FirstEffect().GetHitInfoVec() };
            return FightResult( CreatureEffect(creatureDefendingPtrC, HIT_INFO_VEC, nullptr, nonHitConditionsSVec, true) );
        }
        else
        {
            return FightResult(creatureDefendingPtrC);
        }
    }


    const FightResult FightClub::Roar(creature::CreaturePtrC_t creatureRoaringPtrC,
                                      CombatDisplayCPtrC_t     COMBAT_DISPLAY_CPTRC)
    {
        auto const LIVING_OPPONENT_CREATURES_PVEC{ creature::Algorithms::FindByAlive(creature::Algorithms::PlayersByType( ! creatureRoaringPtrC->IsPlayerCharacter() )) };

        auto const ROARER_STRENGTH{ creatureRoaringPtrC->Stats().Str().Current() };
        auto const ROARER_STRENGTH_RAND_MIN{ StatReduced(ROARER_STRENGTH) };
        auto const ROARER_STRENGTH_RAND_MAX{ ROARER_STRENGTH };

        auto const ROARER_LUCK{ creatureRoaringPtrC->Stats().Lck().Current() };
        auto const ROARER_LUCK_RAND_MIN_HALF((ROARER_LUCK / 5));
        auto const ROARER_LUCK_RAND_MAX_HALF{ ROARER_LUCK / 2 };

        auto const IS_ROARING_CREATURE_FLYING{ Encounter::Instance()->GetTurnInfoCopy(creatureRoaringPtrC).GetIsFlying() };

        auto const ROARING_FRIGHTEN_ROLL_MIN{ ROARER_STRENGTH_RAND_MIN + ROARER_LUCK_RAND_MIN_HALF };
        auto const ROARING_FRIGHTEN_ROLL_MAX{ ROARER_STRENGTH_RAND_MAX + ROARER_LUCK_RAND_MAX_HALF };
        auto const ROARING_CREATURE_FRIGHTEN_ROLL{ sfml_util::rand::Int(ROARING_FRIGHTEN_ROLL_MIN, ROARING_FRIGHTEN_ROLL_MAX)};

        CreatureEffectVec_t creatureEffectsVec;

        //Give each defending creature a chance to resist being frightened.
        //The farther away each defending creature is the better chance of resisting he/she/it has.
        for (auto const NEXT_DEFENDING_CREATURE_PTR : LIVING_OPPONENT_CREATURES_PVEC)
        {
            if (NEXT_DEFENDING_CREATURE_PTR->HasCondition(creature::condition::Frightened))
                continue;

            auto nextBlockingDisatnce{ std::abs(COMBAT_DISPLAY_CPTRC->GetBlockingDistanceBetween(creatureRoaringPtrC, NEXT_DEFENDING_CREATURE_PTR)) };

            //if flying, then consider it farther away and less likely to be frightened
            if (Encounter::Instance()->GetTurnInfoCopy(NEXT_DEFENDING_CREATURE_PTR).GetIsFlying() &&
                (IS_ROARING_CREATURE_FLYING == false))
                ++nextBlockingDisatnce;

            auto const DISATANCE_OFFSET{ nextBlockingDisatnce * 5 };

            auto const DEFENDER_INTELLIGENCE{ creatureRoaringPtrC->Stats().Int().Current() };
            auto const DEFENDER_INTELLIGENCE_RAND_MIN{ StatReduced(DEFENDER_INTELLIGENCE) };
            auto const DEFENDER_INTELLIGENCE_RAND_MAX{ DEFENDER_INTELLIGENCE };

            auto const DEFENDER_LUCK{ creatureRoaringPtrC->Stats().Lck().Current() };
            auto const DEFENDER_LUCK_RAND_MIN_HALF((DEFENDER_LUCK / 5));
            auto const DEFENDER_LUCK_RAND_MAX_HALF{ DEFENDER_LUCK / 2 };

            auto const DEFENDING_FRIGHTEN_ROLL_MIN{ DEFENDER_INTELLIGENCE_RAND_MIN + DEFENDER_LUCK_RAND_MIN_HALF + DISATANCE_OFFSET };
            auto const DEFENDING_FRIGHTEN_ROLL_MAX{ DEFENDER_INTELLIGENCE_RAND_MAX + DEFENDER_LUCK_RAND_MAX_HALF + DISATANCE_OFFSET };
            auto const DEFENDING_CREATURE_FRIGHTEN_RESIST_ROLL{ sfml_util::rand::Int(DEFENDING_FRIGHTEN_ROLL_MIN, DEFENDING_FRIGHTEN_ROLL_MAX) };

            auto didFrighten{ false };
            if ((ROARING_CREATURE_FRIGHTEN_ROLL == ROARING_FRIGHTEN_ROLL_MAX) && (DEFENDING_CREATURE_FRIGHTEN_RESIST_ROLL != DEFENDING_FRIGHTEN_ROLL_MAX))
            {
                //if the roaring creature's roll was max and the defending creature's was not, then frighten
                didFrighten = true;
            }
            else if ((ROARING_CREATURE_FRIGHTEN_ROLL != ROARING_FRIGHTEN_ROLL_MAX) && (DEFENDING_CREATURE_FRIGHTEN_RESIST_ROLL == DEFENDING_FRIGHTEN_ROLL_MAX))
            {
                //if the defending creature's roll was max and the roaring creature's was not, then don't frighten
                didFrighten = false;
            }
            else if ((ROARING_CREATURE_FRIGHTEN_ROLL + static_cast<int>(creatureRoaringPtrC->Rank())) == (DEFENDING_CREATURE_FRIGHTEN_RESIST_ROLL + static_cast<int>(NEXT_DEFENDING_CREATURE_PTR->Rank())))
            {
                //if rank offset rolls are equal, then decide if frightened by fair coint toss
                didFrighten = sfml_util::rand::Bool();
            }
            else if ((ROARING_CREATURE_FRIGHTEN_ROLL + static_cast<int>(creatureRoaringPtrC->Rank())) > (DEFENDING_CREATURE_FRIGHTEN_RESIST_ROLL + static_cast<int>(NEXT_DEFENDING_CREATURE_PTR->Rank())))
            {
                //if the roaring creature's rolls offset with rank was higher than the defending creature's rank offset roll, then frighten
                didFrighten = true;
            }

            if (didFrighten)
            {
                auto const CONDITION_SVEC{ creature::condition::ConditionFactory::Make(creature::condition::Frightened) };

                NEXT_DEFENDING_CREATURE_PTR->ConditionAdd(CONDITION_SVEC);

                creature::ConditionSVec_t nonHitConditionsSVec;
                nonHitConditionsSVec.push_back(CONDITION_SVEC);

                creatureEffectsVec.push_back( CreatureEffect(NEXT_DEFENDING_CREATURE_PTR, HitInfoVec_t(), nullptr, nonHitConditionsSVec) );
            }
        }

        return FightResult(creatureEffectsVec);
    }


    creature::CreaturePtr_t FightClub::FindNonPlayerCreatureToAttack(creature::CreaturePtrC_t creatureAttackingtrC, CombatDisplayCPtrC_t COMBAT_DISPLAY_CPTRC)
    {
        creature::CreaturePVec_t attackableNonPlayerCreaturesPVec;
        COMBAT_DISPLAY_CPTRC->FindCreaturesThatCanBeAttackedOfType(attackableNonPlayerCreaturesPVec, creatureAttackingtrC, false);

        M_ASSERT_OR_LOGANDTHROW_SS((attackableNonPlayerCreaturesPVec.empty() == false), "heroespath::combat::FightClub::HandleAttack() FindNonPlayerCreatureToAttack() returned no attackable creatures.");

        auto const LIVING_ATTACKABLE_NONPLAYER_CREATURES_PVEC{ creature::Algorithms::FindByAlive(attackableNonPlayerCreaturesPVec) };

        M_ASSERT_OR_LOGANDTHROW_SS((LIVING_ATTACKABLE_NONPLAYER_CREATURES_PVEC.empty() == false), "heroespath::combat::FightClub::HandleAttack() FindNonPlayerCreatureToAttack() returned no LIVING attackable creatures.");

        //attack those with the lowest relative health first, which will correspond to the health bar seen on screen
        auto const LIVING_ATTACKABLE_LOWESTHEALTHRATIO_NONPLAYER_CREATURES_PVEC{ creature::Algorithms::FindLowestHealthRatio(LIVING_ATTACKABLE_NONPLAYER_CREATURES_PVEC) };

        M_ASSERT_OR_LOGANDTHROW_SS((LIVING_ATTACKABLE_LOWESTHEALTHRATIO_NONPLAYER_CREATURES_PVEC.empty() == false), "heroespath::combat::FightClub::HandleAttack() FindNonPlayerCreatureToAttack() returned no LIVING LOWEST HEALTH RATIO attackable creatures.");

        //skip creatures who are not a threat
        auto const LIVING_ATTACKABLE_LOWESTHEALTHRATIO_NONPLAYER_NOTPERMDISABLED_CREATURES_PVEC{ creature::Algorithms::FindByConditionMeaningNotAThreatPermenantly(LIVING_ATTACKABLE_LOWESTHEALTHRATIO_NONPLAYER_CREATURES_PVEC, false) };

        //attack those that are temporarily disabled first
        auto const LIVING_ATTACKABLE_LOWESTHEALTHRATIO_NONPLAYER_NOTPERMDISABLED_TEMPDISABLED_CREATURES_PVEC{ creature::Algorithms::FindByConditionMeaningNotAThreatTemporarily(LIVING_ATTACKABLE_LOWESTHEALTHRATIO_NONPLAYER_NOTPERMDISABLED_CREATURES_PVEC) };

        if (LIVING_ATTACKABLE_LOWESTHEALTHRATIO_NONPLAYER_NOTPERMDISABLED_TEMPDISABLED_CREATURES_PVEC.empty())
        {
            if (LIVING_ATTACKABLE_LOWESTHEALTHRATIO_NONPLAYER_NOTPERMDISABLED_CREATURES_PVEC.empty())
            {
                return sfml_util::rand::SelectRandom(LIVING_ATTACKABLE_LOWESTHEALTHRATIO_NONPLAYER_CREATURES_PVEC);
            }
            else
            {
                return sfml_util::rand::SelectRandom(LIVING_ATTACKABLE_LOWESTHEALTHRATIO_NONPLAYER_NOTPERMDISABLED_CREATURES_PVEC);
            }
        }
        else
        {
            return sfml_util::rand::SelectRandom(LIVING_ATTACKABLE_LOWESTHEALTHRATIO_NONPLAYER_NOTPERMDISABLED_TEMPDISABLED_CREATURES_PVEC);
        }
    }


    bool FightClub::IsConditionContained(const creature::condition::Enum E, const HitInfoVec_t & HIT_INFO_VEC)
    {
        for (auto const & NEXT_HIT_INFO : HIT_INFO_VEC)
            if (NEXT_HIT_INFO.ContainsCondition(E))
                return true;

        return false;
    }

}
}
