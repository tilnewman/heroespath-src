//
// turn-decider.cpp
//
#include "turn-decider.hpp"

#include "utilz/random.hpp"
#include "utilz/vector.hpp"

#include "game/combat/turn-info.hpp"
#include "game/combat/encounter.hpp"
#include "game/combat/combat-display.hpp"
#include "game/creature/creature.hpp"
#include "game/creature/algorithms.hpp"
#include "game/spell/spell-type-enum.hpp"
#include "game/spell/spell-base.hpp"

#include <vector>
#include <tuple>


namespace game
{
namespace combat
{

    const spell::SpellTypeVec_t TurnDecider::COMBAT_SPELLS_VEC_     { spell::SpellType::Attack, spell::SpellType::EnchantCreatureHelpful, spell::SpellType::EnchantCreatureHarmful, spell::SpellType::MiscCombat };
    const spell::SpellTypeVec_t TurnDecider::AFFLICTION_SPELLS_VEC_ { spell::SpellType::Attack, spell::SpellType::EnchantCreatureHarmful };
    const spell::SpellTypeVec_t TurnDecider::BENEFICIAL_SPELLS_VEC_ { spell::SpellType::Heal, spell::SpellType::EnchantCreatureHelpful };


    const TurnActionInfo TurnDecider::Decide(const creature::CreaturePtrC_t CREATURE_DECIDING_CPTRC,
                                             CombatDisplayCPtrC_t           COMBAT_DISPLAY_CPTRC)
    {
        //only do 'Nothing' if can't take any action
        if (CREATURE_DECIDING_CPTRC->CanTakeAction() == false)
            return TurnActionInfo(TurnAction::Nothing);

        auto const TURN_INFO{ Encounter::Instance()->GetTurnInfoCopy(CREATURE_DECIDING_CPTRC) };

        //decide if flying
        auto const FLY_TURN_ACTION_INFO{ DecideIfFlying(TURN_INFO, CREATURE_DECIDING_CPTRC) };
        if (FLY_TURN_ACTION_INFO.Action() != TurnAction::Count)
            return FLY_TURN_ACTION_INFO;

        creature::CreaturePVec_t playersInMeleeRangePVec;
        COMBAT_DISPLAY_CPTRC->FindCreaturesAllRoundOfType(playersInMeleeRangePVec, CREATURE_DECIDING_CPTRC, true);

        creature::CreaturePVec_t fellowEnemiesInMeleeRangePVec;
        COMBAT_DISPLAY_CPTRC->FindCreaturesAllRoundOfType(fellowEnemiesInMeleeRangePVec, CREATURE_DECIDING_CPTRC, false);

        //decide if running away (retreating), if cowardly or wary and outnumbered
        auto const RETREAT_TURN_ACTION_INFO{ DecideIfRetreating(TURN_INFO,
                                                                CREATURE_DECIDING_CPTRC,
                                                                COMBAT_DISPLAY_CPTRC,
                                                                playersInMeleeRangePVec.size(),
                                                                fellowEnemiesInMeleeRangePVec.size()) };

        if (RETREAT_TURN_ACTION_INFO.Action() != TurnAction::Count)
            return RETREAT_TURN_ACTION_INFO;

        const creature::CreaturePtrC_t MOST_DESIRED_TARGET_CPTRC{ FindMostDesiredTarget(TURN_INFO,
                                                                                        CREATURE_DECIDING_CPTRC,
                                                                                        COMBAT_DISPLAY_CPTRC) };

        //decide if casting a spell
        auto const CASTING_TURN_ACTION_INFO{ DecideIfCasting(TURN_INFO, CREATURE_DECIDING_CPTRC, MOST_DESIRED_TARGET_CPTRC) };
        if ((CASTING_TURN_ACTION_INFO.Action() != TurnAction::Count) && (CASTING_TURN_ACTION_INFO.Spell() != nullptr))
            return CASTING_TURN_ACTION_INFO;

        creature::CreaturePVec_t playersInAttackRangePVec;
        COMBAT_DISPLAY_CPTRC->FindCreaturesThatCanBeAttackedOfType(playersInAttackRangePVec, CREATURE_DECIDING_CPTRC, true);

        auto const MOST_DESIRED_TARGET_DISTANCE{ COMBAT_DISPLAY_CPTRC->GetBlockingDistanceBetween(CREATURE_DECIDING_CPTRC,
                                                                                                  MOST_DESIRED_TARGET_CPTRC) };

        auto const DOES_FLYING_SEPARATE_MOST_DESIRED_TARGET{ (TURN_INFO.GetIsFlying() == false) && Encounter::Instance()->GetTurnInfoCopy(MOST_DESIRED_TARGET_CPTRC).GetIsFlying() };

        auto const CAN_ATTACK_MOST_DESIRED_TARGET_WITH{ CREATURE_DECIDING_CPTRC->HasWeaponsHeld() &&
                                                        ((MOST_DESIRED_TARGET_DISTANCE == 0) || CREATURE_DECIDING_CPTRC->IsHoldingProjectileWeapon()) &&
                                                        ((DOES_FLYING_SEPARATE_MOST_DESIRED_TARGET == false) || (CREATURE_DECIDING_CPTRC->IsHoldingProjectileWeapon())) };

        //decide if moving toward most desired target, which technically could mean Advancing or Retreating...
        auto const MOVE_TOWARD_TURN_ACTION_INFO{ DecideIfMovingTowardsMostDesiredTarget(TURN_INFO,
                                                                                        CREATURE_DECIDING_CPTRC,
                                                                                        COMBAT_DISPLAY_CPTRC,
                                                                                        MOST_DESIRED_TARGET_DISTANCE,
                                                                                        playersInAttackRangePVec.size(),
                                                                                        CAN_ATTACK_MOST_DESIRED_TARGET_WITH) };

        if (MOVE_TOWARD_TURN_ACTION_INFO.Action() != TurnAction::Count)
            return MOVE_TOWARD_TURN_ACTION_INFO;

        //decide if taking beast specific actions (roar, fly, pounce, etc.)
        if (CREATURE_DECIDING_CPTRC->IsBeast())
        {
            auto const BEAST_TURN_ACTION_INFO{ DecideIfDoingBeastAction(TURN_INFO,
                                                                        CREATURE_DECIDING_CPTRC,
                                                                        MOST_DESIRED_TARGET_CPTRC,
                                                                        playersInMeleeRangePVec,
                                                                        MOST_DESIRED_TARGET_DISTANCE) };

            if (BEAST_TURN_ACTION_INFO.Action() != TurnAction::Count)
                return BEAST_TURN_ACTION_INFO;
        }

        //attack most desired target if possible
        if (CAN_ATTACK_MOST_DESIRED_TARGET_WITH)
        {
            return TurnActionInfo(TurnAction::Attack, MOST_DESIRED_TARGET_CPTRC);
        }

        //if no players are in range to attack but there are held weapons ready to attack and other fellow creatures ahead, then move toward the closest player
        if (playersInMeleeRangePVec.empty() &&
            (CREATURE_DECIDING_CPTRC->HasWeaponsHeld()) &&
            (TURN_INFO.GetStrategyInfo().Advance() != strategy::AdvanceType::Cowardly) &&
            (COMBAT_DISPLAY_CPTRC->CanAdvanceOrRetreat(CREATURE_DECIDING_CPTRC, true).empty()))
        {
            creature::CreaturePVec_t creaturesAheadPVec;
            COMBAT_DISPLAY_CPTRC->FindCreaturesAtBlockingPosOfType(creaturesAheadPVec, COMBAT_DISPLAY_CPTRC->FindBlockingPos(CREATURE_DECIDING_CPTRC) - 1, true);

            if (creaturesAheadPVec.empty() == false)
            {
                auto const DISTANCE_TO_CLOSEST_PLAYER{ COMBAT_DISPLAY_CPTRC->GetClosestBlockingDistanceByType(CREATURE_DECIDING_CPTRC, true) };

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

        //cast an attack spell if able
        if (CREATURE_DECIDING_CPTRC->CanCastSpellByType(spell::SpellType::Attack))
        {
            return DecideSpell(CREATURE_DECIDING_CPTRC, MOST_DESIRED_TARGET_CPTRC, { spell::SpellType::Attack });
        }

        //cast any misc spell if able
        if (CREATURE_DECIDING_CPTRC->CanCastSpells())
        {
            auto const SPELL_CAST_TURN_ACTION_INFO{ ForcePickSpellToCast(CREATURE_DECIDING_CPTRC, MOST_DESIRED_TARGET_CPTRC) };
            if ((SPELL_CAST_TURN_ACTION_INFO.Action() != TurnAction::Count) && (SPELL_CAST_TURN_ACTION_INFO.Spell() != nullptr))
                return SPELL_CAST_TURN_ACTION_INFO;
        }

        //change weapons if needed
        if ((playersInAttackRangePVec.empty() == false) &&
            (CREATURE_DECIDING_CPTRC->HasWeaponsHeld() == false) &&
            (CREATURE_DECIDING_CPTRC->HasWeapons() == true))
        {
            return TurnActionInfo(TurnAction::ChangeWeapon);
        }

        //Okay, so we are reaching this end point in the logic tree much more
        //often than anticipated, so try and attacking here if possible.
        if (CREATURE_DECIDING_CPTRC->HasWeaponsHeld() &&
            (playersInAttackRangePVec.empty() == false))
        {
            return TurnActionInfo(TurnAction::Attack, utilz::Vector::SelectRandom(playersInAttackRangePVec));
        }

        //At this point nothing was chosen to be done, so block instead
        return TurnActionInfo(TurnAction::Block);
    }


    creature::CreaturePtr_t TurnDecider::FindMostDesiredTarget(const TurnInfo &               TURN_INFO,
                                                               const creature::CreaturePtrC_t CREATURE_CPTRC,
                                                               CombatDisplayCPtrC_t           COMBAT_DISPLAY_CPTRC)
    {
        //pick (select) favorite targets by type
        const creature::CreaturePVec_t SELECT_TARGETS_PVEC { FindSelectedTargets(TURN_INFO) };

        //eliminate (refine) targets by conditions/AI/etc.
        const creature::CreaturePVec_t REFINED_TARGETS_PVEC{ RefineTargets(SELECT_TARGETS_PVEC, TURN_INFO) };

        if (REFINED_TARGETS_PVEC.empty())
        {
            if (SELECT_TARGETS_PVEC.empty())
            {
                //if no refined or select targets, then choose the closest player, avoiding those permenantly not a threat, at random
                auto const CLOSEST_PLAYERS_PVEC{ COMBAT_DISPLAY_CPTRC->FindClosestLivingByType(CREATURE_CPTRC, true) };

                if (CLOSEST_PLAYERS_PVEC.size() == 1)
                    return * CLOSEST_PLAYERS_PVEC.begin();

                auto const CLOSEST_PLAYERS_STILL_A_THREAT_PVEC{ creature::Algorithms::FindByConditionMeaningNotAThreatPermenantly(CLOSEST_PLAYERS_PVEC, false) };

                auto const CLOSEST_PLAYERS_THREAT_AND_FLYING_MATCH{ ((TURN_INFO.GetIsFlying()) ? CLOSEST_PLAYERS_STILL_A_THREAT_PVEC : creature::Algorithms::FindByFlying(CLOSEST_PLAYERS_STILL_A_THREAT_PVEC, false)) };

                if (CLOSEST_PLAYERS_THREAT_AND_FLYING_MATCH.empty())
                {
                    if (CLOSEST_PLAYERS_STILL_A_THREAT_PVEC.empty())
                    {
                        return utilz::Vector::SelectRandom(CLOSEST_PLAYERS_PVEC);
                    }
                    else
                    {
                        return utilz::Vector::SelectRandom(CLOSEST_PLAYERS_STILL_A_THREAT_PVEC);
                    }
                }
                else
                {
                    return utilz::Vector::SelectRandom(CLOSEST_PLAYERS_THREAT_AND_FLYING_MATCH);
                }
            }
            else
            {
                //select the closest of the select targets at random
                return utilz::Vector::SelectRandom( COMBAT_DISPLAY_CPTRC->FindClosestLiving(CREATURE_CPTRC, SELECT_TARGETS_PVEC) );
            }
        }
        else
        {
            //select the closest of the refined targets at random
            return utilz::Vector::SelectRandom(COMBAT_DISPLAY_CPTRC->FindClosestLiving(CREATURE_CPTRC, REFINED_TARGETS_PVEC));
        }
    }


    creature::CreaturePVec_t TurnDecider::FindSelectedTargets(const TurnInfo & TURN_INFO)
    {
        creature::CreaturePVec_t selectedPlayersPVec;

        auto const SELECT_TYPE_ENUM{ TURN_INFO.GetStrategyInfo().Select() };

        if (SELECT_TYPE_ENUM == strategy::SelectType::None)
            return selectedPlayersPVec;

        auto const ALL_LIVING_PLAYERS_PVEC{ creature::Algorithms::Players(true) };

        auto const SELECTABLE_PLAYERS_PVEC{ ((TURN_INFO.GetIsFlying()) ? ALL_LIVING_PLAYERS_PVEC : creature::Algorithms::FindByFlying(ALL_LIVING_PLAYERS_PVEC, false)) };

        //add target selections by race
        if (SELECT_TYPE_ENUM & strategy::SelectType::Dragon)
            utilz::Vector::Append(creature::Algorithms::FindByRace(SELECTABLE_PLAYERS_PVEC, creature::race::Dragon), selectedPlayersPVec);

        if (SELECT_TYPE_ENUM & strategy::SelectType::Gnome)
            utilz::Vector::Append(creature::Algorithms::FindByRace(SELECTABLE_PLAYERS_PVEC, creature::race::Gnome), selectedPlayersPVec);

        if (SELECT_TYPE_ENUM & strategy::SelectType::Human)
            utilz::Vector::Append(creature::Algorithms::FindByRace(SELECTABLE_PLAYERS_PVEC, creature::race::Human), selectedPlayersPVec);

        if (SELECT_TYPE_ENUM & strategy::SelectType::Pixie)
            utilz::Vector::Append(creature::Algorithms::FindByRace(SELECTABLE_PLAYERS_PVEC, creature::race::Pixie), selectedPlayersPVec);

        if (SELECT_TYPE_ENUM & strategy::SelectType::Wolfen)
            utilz::Vector::Append(creature::Algorithms::FindByRace(SELECTABLE_PLAYERS_PVEC, creature::race::Wolfen), selectedPlayersPVec);

        //add target selections by role
        if (SELECT_TYPE_ENUM & strategy::SelectType::Archer)
            utilz::Vector::Append(creature::Algorithms::FindByRole(SELECTABLE_PLAYERS_PVEC, creature::role::Archer), selectedPlayersPVec);

        if (SELECT_TYPE_ENUM & strategy::SelectType::Bard)
            utilz::Vector::Append(creature::Algorithms::FindByRole(SELECTABLE_PLAYERS_PVEC, creature::role::Bard), selectedPlayersPVec);

        if (SELECT_TYPE_ENUM & strategy::SelectType::Beastmaster)
            utilz::Vector::Append(creature::Algorithms::FindByRole(SELECTABLE_PLAYERS_PVEC, creature::role::Beastmaster), selectedPlayersPVec);

        if (SELECT_TYPE_ENUM & strategy::SelectType::Cleric)
            utilz::Vector::Append(creature::Algorithms::FindByRole(SELECTABLE_PLAYERS_PVEC, creature::role::Cleric), selectedPlayersPVec);

        if (SELECT_TYPE_ENUM & strategy::SelectType::FireBrand)
            utilz::Vector::Append(creature::Algorithms::FindByRole(SELECTABLE_PLAYERS_PVEC, creature::role::Firebrand), selectedPlayersPVec);

        if (SELECT_TYPE_ENUM & strategy::SelectType::Knight)
            utilz::Vector::Append(creature::Algorithms::FindByRole(SELECTABLE_PLAYERS_PVEC, creature::role::Knight), selectedPlayersPVec);

        if (SELECT_TYPE_ENUM & strategy::SelectType::Sorcerer)
            utilz::Vector::Append(creature::Algorithms::FindByRole(SELECTABLE_PLAYERS_PVEC, creature::role::Sorcerer), selectedPlayersPVec);

        if (SELECT_TYPE_ENUM & strategy::SelectType::Sylavin)
            utilz::Vector::Append(creature::Algorithms::FindByRole(SELECTABLE_PLAYERS_PVEC, creature::role::Sylavin), selectedPlayersPVec);

        if (SELECT_TYPE_ENUM & strategy::SelectType::Theif)
            utilz::Vector::Append(creature::Algorithms::FindByRole(SELECTABLE_PLAYERS_PVEC, creature::role::Thief), selectedPlayersPVec);

        //add target selections by misc types
        if (SELECT_TYPE_ENUM & strategy::SelectType::Beast)
            utilz::Vector::Append(creature::Algorithms::FindByBeast(SELECTABLE_PLAYERS_PVEC), selectedPlayersPVec);

        if (SELECT_TYPE_ENUM & strategy::SelectType::NotBeast)
            utilz::Vector::Append(creature::Algorithms::FindByBeast(SELECTABLE_PLAYERS_PVEC, false), selectedPlayersPVec);

        if (SELECT_TYPE_ENUM & strategy::SelectType::CanFly)
            utilz::Vector::Append(creature::Algorithms::FindByCanFly(SELECTABLE_PLAYERS_PVEC), selectedPlayersPVec);

        if (SELECT_TYPE_ENUM & strategy::SelectType::CantFly)
            utilz::Vector::Append(creature::Algorithms::FindByCanFly(SELECTABLE_PLAYERS_PVEC, false), selectedPlayersPVec);

        if (SELECT_TYPE_ENUM & strategy::SelectType::Caster)
            utilz::Vector::Append(creature::Algorithms::FindBySpellCaster(SELECTABLE_PLAYERS_PVEC), selectedPlayersPVec);

        if (SELECT_TYPE_ENUM & strategy::SelectType::Projectile)
            utilz::Vector::Append(creature::Algorithms::FindByProjectileWeapons(SELECTABLE_PLAYERS_PVEC), selectedPlayersPVec);

        if (selectedPlayersPVec.size() > 1)
            std::unique(selectedPlayersPVec.begin(), selectedPlayersPVec.end());

        return creature::Algorithms::FindByConditionMeaningNotAThreatPermenantly(selectedPlayersPVec, false);
    }


    creature::CreaturePVec_t TurnDecider::RefineTargets(const creature::CreaturePVec_t & SELECTED_TARGETS_PVEC, const TurnInfo & TURN_INFO)
    {
        auto const REFINE_TYPE_ENUM{ TURN_INFO.GetStrategyInfo().Refine() };

        if (SELECTED_TARGETS_PVEC.empty() || (REFINE_TYPE_ENUM == strategy::RefineType::None))
            return SELECTED_TARGETS_PVEC;

        creature::CreaturePVec_t refinedTargetsPVec;

        if (REFINE_TYPE_ENUM & strategy::RefineType::Murderer)
            std::copy_if(SELECTED_TARGETS_PVEC.begin(),
                         SELECTED_TARGETS_PVEC.end(),
                         back_inserter(refinedTargetsPVec),
                         [] (const creature::CreaturePtr_t CCPTR) { return CCPTR->HasCondition(creature::condition::Unconscious); });

        if (REFINE_TYPE_ENUM & strategy::RefineType::Bloodthirsty)
        {
            auto const LOWEST_HEALTH_PVEC{ creature::Algorithms::FindLowestHealth(SELECTED_TARGETS_PVEC) };
            std::copy(LOWEST_HEALTH_PVEC.begin(), LOWEST_HEALTH_PVEC.end(), back_inserter(refinedTargetsPVec));
        }

        if (REFINE_TYPE_ENUM & strategy::RefineType::Coward)
        {
            creature::CreaturePVec_t cantTakeActionPVec;

            std::copy_if(SELECTED_TARGETS_PVEC.begin(),
                         SELECTED_TARGETS_PVEC.end(),
                         back_inserter(cantTakeActionPVec),
                         [] (const creature::CreaturePtr_t CCPTR) { return (CCPTR->CanTakeAction() == false); });

            if (cantTakeActionPVec.empty() == false)
            {
                auto const CANT_ACT_BUT_NOT_PERM_PVEC{ creature::Algorithms::FindByConditionMeaningNotAThreatPermenantly(cantTakeActionPVec, false) };

                if (CANT_ACT_BUT_NOT_PERM_PVEC.empty() == false)
                    std::copy(CANT_ACT_BUT_NOT_PERM_PVEC.begin(), CANT_ACT_BUT_NOT_PERM_PVEC.end(), back_inserter(refinedTargetsPVec));
            }
        }

        if (REFINE_TYPE_ENUM & strategy::RefineType::Hit)
        {
            if (REFINE_TYPE_ENUM & strategy::RefineType::LastTo)
            {
                if (TURN_INFO.GetLastHitByCreature() != nullptr)
                    refinedTargetsPVec.push_back(TURN_INFO.GetLastHitByCreature());
            }
            else
            {
                if (TURN_INFO.GetFirstHitByCreature() != nullptr)
                    refinedTargetsPVec.push_back(TURN_INFO.GetFirstHitByCreature());
            }
        }

        if (REFINE_TYPE_ENUM & strategy::RefineType::Attack)
        {
            if (REFINE_TYPE_ENUM & strategy::RefineType::LastTo)
            {
                if (TURN_INFO.GetLastAttackedByCreature() != nullptr)
                    refinedTargetsPVec.push_back(TURN_INFO.GetLastAttackedByCreature());
            }
            else
            {
                if (TURN_INFO.GetFirstAttackedByCreature() != nullptr)
                    refinedTargetsPVec.push_back(TURN_INFO.GetFirstAttackedByCreature());
            }
        }

        if (REFINE_TYPE_ENUM & strategy::RefineType::MusicMaker)
        {
            if (REFINE_TYPE_ENUM & strategy::RefineType::LastTo)
            {
                if (TURN_INFO.GetLastToMakeMusicCreature() != nullptr)
                    refinedTargetsPVec.push_back(TURN_INFO.GetLastToMakeMusicCreature());
            }
            else
            {
                if (TURN_INFO.GetFirstToMakeMusicCreature() != nullptr)
                    refinedTargetsPVec.push_back(TURN_INFO.GetFirstToMakeMusicCreature());
            }
        }

        if (REFINE_TYPE_ENUM & strategy::RefineType::Caster)
        {
            if (REFINE_TYPE_ENUM & strategy::RefineType::LastTo)
            {
                if (TURN_INFO.GetLastToCastCreature() != nullptr)
                    refinedTargetsPVec.push_back(TURN_INFO.GetLastToCastCreature());
            }
            else
            {
                if (TURN_INFO.GetFirstToCastCreature() != nullptr)
                    refinedTargetsPVec.push_back(TURN_INFO.GetFirstToCastCreature());
            }
        }

        if (REFINE_TYPE_ENUM & strategy::RefineType::Enchanted)
            std::copy_if(SELECTED_TARGETS_PVEC.begin(),
                         SELECTED_TARGETS_PVEC.end(),
                         back_inserter(refinedTargetsPVec),
                         [] (const creature::CreaturePtr_t CCPTR) { return CCPTR->HasMagicalCondition(); });

        if (REFINE_TYPE_ENUM & strategy::RefineType::NotEnchanted)
            std::copy_if(SELECTED_TARGETS_PVEC.begin(),
                         SELECTED_TARGETS_PVEC.end(),
                         back_inserter(refinedTargetsPVec),
                         [] (const creature::CreaturePtr_t CCPTR) { return (CCPTR->HasMagicalCondition() == false); });

        if (REFINE_TYPE_ENUM & strategy::RefineType::MostDamage)
            if (TURN_INFO.GetMostDamageCreature().second != nullptr)
                refinedTargetsPVec.push_back(TURN_INFO.GetMostDamageCreature().second);

        if (refinedTargetsPVec.size() > 1)
            std::unique(refinedTargetsPVec.begin(), refinedTargetsPVec.end());

        return refinedTargetsPVec;
    }


    float TurnDecider::ChanceFromFrequency(const strategy::FrequencyType::Enum E)
    {
        if (E == strategy::FrequencyType::Commonly)
            return 0.75f;
        else if (E == strategy::FrequencyType::Neutral)
            return 0.5f;
        else if (E == strategy::FrequencyType::Often)
            return 0.9f;
        else if (E == strategy::FrequencyType::Rarely)
            return 0.1f;
        else if (E == strategy::FrequencyType::Seldom)
            return 0.25f;
        else if (E == strategy::FrequencyType::Always)
            return 1.0f;
        else // Never & everything else
            return 0.0f;
    }


    const TurnActionInfo TurnDecider::DecideIfRetreating(const TurnInfo &               TURN_INFO,
                                                         const creature::CreaturePtrC_t CREATURE_DECIDING_CPTRC,
                                                         CombatDisplayCPtrC_t           COMBAT_DISPLAY_CPTRC,
                                                         const std::size_t              NUM_PLAYERS_IN_MELEE_RANGE,
                                                         const std::size_t              NUM_FELLOWS_IN_MELEE_RANGE)
    {
        if (COMBAT_DISPLAY_CPTRC->CanAdvanceOrRetreat(CREATURE_DECIDING_CPTRC, false).empty())
        {
            if (((TURN_INFO.GetStrategyInfo().Retreat() == strategy::RetreatType::Coward) && (NUM_PLAYERS_IN_MELEE_RANGE > 0)) ||
                ((TURN_INFO.GetStrategyInfo().Retreat() == strategy::RetreatType::Wary) && (NUM_PLAYERS_IN_MELEE_RANGE > NUM_FELLOWS_IN_MELEE_RANGE)) ||
                (NUM_PLAYERS_IN_MELEE_RANGE >= TURN_INFO.GetStrategyInfo().OutnumberedRetreatCount()))
            {
                //fly to evade sometimes
                auto const FLY_TURN_ACTION_INFO{ DecideIfFlying(TURN_INFO, CREATURE_DECIDING_CPTRC) };
                if (FLY_TURN_ACTION_INFO.Action() != TurnAction::Count)
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


    const TurnActionInfo TurnDecider::DecideIfCasting(const TurnInfo &               TURN_INFO,
                                                      const creature::CreaturePtrC_t CREATURE_DECIDING_CPTRC,
                                                      const creature::CreaturePtrC_t MOST_DESIRED_TARGET_CPTRC)
    {
        auto const CAN_CAST_HEALING_SPELLS   { CREATURE_DECIDING_CPTRC->CanCastSpellByType(spell::SpellType::Heal) };
        auto const CAN_CAST_ATTACK_SPELLS    { CREATURE_DECIDING_CPTRC->CanCastSpellByType(spell::SpellType::Attack) };

        //determine if casting a spell
        if ((CAN_CAST_HEALING_SPELLS || CAN_CAST_ATTACK_SPELLS) && (TURN_INFO.GetStrategyInfo().CastFreq() != strategy::FrequencyType::Never))
        {
            //heal self if needed
            if ((CAN_CAST_HEALING_SPELLS) && (CREATURE_DECIDING_CPTRC->HealthRatio() < 0.26f))
                return DecideSpell(CREATURE_DECIDING_CPTRC, CREATURE_DECIDING_CPTRC, { spell::SpellType::Heal });

            std::size_t maxCastCount{ 0 };

            if (TURN_INFO.GetStrategyInfo().CastFreq() == strategy::FrequencyType::Once)
                maxCastCount = 1;
            else if (TURN_INFO.GetStrategyInfo().CastFreq() == strategy::FrequencyType::Twice)
                maxCastCount = 2;
            else if (TURN_INFO.GetStrategyInfo().CastFreq() == strategy::FrequencyType::Thrice)
                maxCastCount = 3;

            //prevent casting spell if already cast too many times
            if ((maxCastCount == 0) || (TURN_INFO.CastCount() < maxCastCount))
            {
                auto const CAST_CHANCE{ ChanceFromFrequency(TURN_INFO.GetStrategyInfo().CastFreq()) };

                //determine if will cast spell at random
                if ((TURN_INFO.GetStrategyInfo().CastFreq() == strategy::FrequencyType::Always) ||
                    (utilz::random::Float(1.0f) < CAST_CHANCE))
                {
                    auto const FELLOWS_WITH_LOWEST_HEALTH_PVEC{ creature::Algorithms::FindLowestHealthRatio(creature::Algorithms::NonPlayers(true)) };

                    //heal fellow if one is injured enough
                    if (FELLOWS_WITH_LOWEST_HEALTH_PVEC.at(0)->HealthRatio() < 0.25f)
                    {
                        return DecideSpell(CREATURE_DECIDING_CPTRC, utilz::Vector::SelectRandom(FELLOWS_WITH_LOWEST_HEALTH_PVEC), { spell::SpellType::Heal });
                    }
                    else
                    {
                        //...otherwise choose a combat spell
                        return DecideSpell(CREATURE_DECIDING_CPTRC, MOST_DESIRED_TARGET_CPTRC, COMBAT_SPELLS_VEC_);
                    }
                }
            }
        }

        return TurnActionInfo();
    }


    const TurnActionInfo TurnDecider::DecideIfMovingTowardsMostDesiredTarget(const TurnInfo &               TURN_INFO,
                                                                             const creature::CreaturePtrC_t CREATURE_DECIDING_CPTRC,
                                                                             CombatDisplayCPtrC_t           COMBAT_DISPLAY_CPTRC,
                                                                             const int                      MOST_DESIRED_TARGET_DISTANCE,
                                                                             const bool                     NUM_PLAYERS_IN_ATTACK_RANGE,
                                                                             const bool                     CAN_ATTACK_MOST_DESIRED_TARGET_WITH)
    {
        if ((TURN_INFO.GetStrategyInfo().Advance() != strategy::AdvanceType::Cowardly) && (MOST_DESIRED_TARGET_DISTANCE != 0))
        {
            if (((TURN_INFO.GetStrategyInfo().Advance() == strategy::AdvanceType::None) && (NUM_PLAYERS_IN_ATTACK_RANGE == 0)) ||
                (TURN_INFO.GetStrategyInfo().Advance() == strategy::AdvanceType::Charger) ||
                ((TURN_INFO.GetStrategyInfo().Advance() == strategy::AdvanceType::Hesitant) && (TURN_INFO.GetWasHitLastTurn() && (CAN_ATTACK_MOST_DESIRED_TARGET_WITH == false))))
            {
                if (MOST_DESIRED_TARGET_DISTANCE < 0)
                {
                    if (COMBAT_DISPLAY_CPTRC->CanAdvanceOrRetreat(CREATURE_DECIDING_CPTRC, true).empty())
                        return TurnActionInfo(TurnAction::Advance);
                }
                else
                {
                    if (COMBAT_DISPLAY_CPTRC->CanAdvanceOrRetreat(CREATURE_DECIDING_CPTRC, false).empty())
                        return TurnActionInfo(TurnAction::Retreat);
                }
            }
        }

        return TurnActionInfo();
    }


    const TurnActionInfo TurnDecider::DecideIfDoingBeastAction(const TurnInfo &               TURN_INFO,
                                                               const creature::CreaturePtrC_t CREATURE_DECIDING_CPTRC,
                                                               const creature::CreaturePtrC_t MOST_DESIRED_TARGET_CPTRC,
                                                               const creature::CreaturePVec_t PLAYERS_IN_MELEE_RANGE_PVEC,
                                                               const int                      MOST_DESIRED_TARGET_CREATURE_DISTANCE)
    {
        if (CREATURE_DECIDING_CPTRC->IsBeast())
        {
            creature::CreaturePtr_t creatureToActOn{ MOST_DESIRED_TARGET_CPTRC };

            //action, chance, rand_value
            using ActionChances_t = std::tuple<TurnAction::Enum, float, float>;
            std::vector<ActionChances_t> actionChancesVec;

            auto const ROAR_CHANCE{ ((PLAYERS_IN_MELEE_RANGE_PVEC.size() > 0) ? ChanceFromFrequency(TURN_INFO.GetStrategyInfo().RoarFreq()) : 0.0f) };
            if (ROAR_CHANCE > 0.0f)
            {
                auto const RAND{ utilz::random::Float(1.0f) };
                if (RAND < ROAR_CHANCE)
                    actionChancesVec.push_back(std::make_tuple(TurnAction::Roar, ROAR_CHANCE, RAND));
            }

            auto const FLY_CHANCE{ ((CREATURE_DECIDING_CPTRC->CanFly() && (TURN_INFO.GetIsFlying() == false)) ? ChanceFromFrequency(TURN_INFO.GetStrategyInfo().FlyFreq()) : 0.0f) };
            if (FLY_CHANCE > 0.0f)
            {
                auto const RAND{ utilz::random::Float(1.0f) };
                if (RAND < FLY_CHANCE)
                    actionChancesVec.push_back(std::make_tuple(TurnAction::Fly, FLY_CHANCE, RAND));
            }

            auto const SKYPOUNCE_CHANCE{ ((TURN_INFO.GetIsFlying()) ? ChanceFromFrequency(TURN_INFO.GetStrategyInfo().FlyPounceFreq()) : 0.0f) };
            if (SKYPOUNCE_CHANCE > 0.0f)
            {
                auto const RAND{ utilz::random::Float(1.0f) };
                if (RAND < FLY_CHANCE)
                    actionChancesVec.push_back(std::make_tuple(TurnAction::SkyPounce, SKYPOUNCE_CHANCE, RAND));
            }

            auto const LANDPOUNCE_CHANCE{ (((TURN_INFO.GetIsFlying() == false) && (PLAYERS_IN_MELEE_RANGE_PVEC.size() > 0)) ? ChanceFromFrequency(TURN_INFO.GetStrategyInfo().StandPounceFreq()) : 0.0f) };
            if (LANDPOUNCE_CHANCE > 0.0f)
            {
                auto const RAND{ utilz::random::Float(1.0f) };
                if (RAND < FLY_CHANCE)
                    actionChancesVec.push_back(std::make_tuple(TurnAction::LandPounce, LANDPOUNCE_CHANCE, RAND));
            }

            if (actionChancesVec.empty() == false)
            {
                if (actionChancesVec.size() > 1)
                    std::sort(actionChancesVec.begin(),
                              actionChancesVec.end(),
                              [] (const ActionChances_t & A, const ActionChances_t & B) { return std::get<2>(A) < std::get<2>(B); });

                auto const DECIDED_ACTION{ std::get<0>(actionChancesVec[0]) };

                if (DECIDED_ACTION == TurnAction::LandPounce)
                {
                    if (MOST_DESIRED_TARGET_CREATURE_DISTANCE == 0)
                    {
                        creatureToActOn = MOST_DESIRED_TARGET_CPTRC;
                    }
                    else
                    {
                        creatureToActOn = utilz::Vector::SelectRandom(PLAYERS_IN_MELEE_RANGE_PVEC);
                    }
                }

                return TurnActionInfo(DECIDED_ACTION, creatureToActOn);
            }
        }

        return TurnActionInfo();
    }


    const TurnActionInfo TurnDecider::ForcePickSpellToCast(const creature::CreaturePtrC_t CREATURE_DECIDING_CPTRC,
                                                           const creature::CreaturePtrC_t MOST_DESIRED_TARGET_CPTRC)
    {
        auto const ALL_SPELLS_PVEC{ CREATURE_DECIDING_CPTRC->Spells() };

        spell::SpellPVec_t combatSpellsPVec;

        std::copy_if(ALL_SPELLS_PVEC.begin(),
                     ALL_SPELLS_PVEC.end(),
                     back_inserter(combatSpellsPVec),
                     [] (const spell::SpellPtr_t S_PTR) { return ((S_PTR->Type() != spell::SpellType::EnchantItemHelpful) &&
                                                                  (S_PTR->Type() != spell::SpellType::EnchantItemHarmful) &&
                                                                  (S_PTR->Type() != spell::SpellType::MiscNonCombat)); });

        if (combatSpellsPVec.empty())
        {
            return TurnActionInfo();
        }

        auto const RAND_FELLOW_WITH_LOWEST_HEALTH_PTR{ utilz::Vector::SelectRandom(creature::Algorithms::FindLowestHealthRatio(creature::Algorithms::NonPlayers(true))) };
        auto const CAN_CAST_HEAL_SPELLS{ (RAND_FELLOW_WITH_LOWEST_HEALTH_PTR->HealthRatio() < 1.0f) };

        spell::SpellPVec_t finalSpellPVec;

        if (CAN_CAST_HEAL_SPELLS)
        {
            finalSpellPVec = combatSpellsPVec;
        }
        else
        {
            std::copy_if(combatSpellsPVec.begin(),
                         combatSpellsPVec.end(),
                         back_inserter(finalSpellPVec),
                         [] (const spell::SpellPtr_t S_PTR) { return (S_PTR->Type() != spell::SpellType::Heal); });
        }

        spell::SpellPtr_t spellToCastPtr{ nullptr };

        if (finalSpellPVec.empty())
        {
            return TurnActionInfo();
        }
        else if (finalSpellPVec.size() == 1)
        {
            spellToCastPtr = finalSpellPVec.at(0);
        }
        else
        {
            spellToCastPtr = finalSpellPVec.at(utilz::random::Int(finalSpellPVec.size() - 1));
        }

        return DecideSpell(CREATURE_DECIDING_CPTRC, MOST_DESIRED_TARGET_CPTRC, { spellToCastPtr->Type() });
    }


    const TurnActionInfo TurnDecider::DecideSpell(const creature::CreaturePtrC_t CREATURE_DECIDING_CPTRC,
                                                  const creature::CreaturePtrC_t MOST_DESIRED_TARGET_CPTRC,
                                                  const spell::SpellTypeVec_t    SPELL_TYPES_VEC)
    {
        return TurnActionInfo(TurnAction::Cast, MOST_DESIRED_TARGET_CPTRC, PickSpell(CREATURE_DECIDING_CPTRC, SPELL_TYPES_VEC));
    }


    spell::SpellPtr_t TurnDecider::PickSpell(const creature::CreaturePtrC_t CREATURE_DECIDING_CPTRC,
                                             const spell::SpellType::Enum   SPELL_TYPE)
    {
        const spell::SpellTypeVec_t SPELL_TYPE_VEC{ SPELL_TYPE };
        return PickSpell(CREATURE_DECIDING_CPTRC, SPELL_TYPE_VEC);
    }


    spell::SpellPtr_t TurnDecider::PickSpell(const creature::CreaturePtrC_t CREATURE_DECIDING_CPTRC,
                                             const spell::SpellTypeVec_t    SPELL_TYPES_VEC)
    {
        auto const ALL_SPELLS_PVEC{ CREATURE_DECIDING_CPTRC->Spells() };

        if (ALL_SPELLS_PVEC.empty())
            return nullptr;

        spell::SpellPVec_t spellsOfTypePVec;

        std::copy_if(ALL_SPELLS_PVEC.begin(),
                     ALL_SPELLS_PVEC.end(),
                     back_inserter(spellsOfTypePVec),
                     [& SPELL_TYPES_VEC]
                     (const spell::SpellPtr_t S_PTR)
                     {
                        for (auto const NEXT_SPELL_TYPE : SPELL_TYPES_VEC)
                            if (NEXT_SPELL_TYPE == S_PTR->Type())
                                return true;

                        return false;
                     });

        auto const SPELL_COUNT{ spellsOfTypePVec.size() };

        if (SPELL_COUNT == 0)
        {
            return nullptr;
        }
        else if (SPELL_COUNT == 1)
        {
            return spellsOfTypePVec.at(0);
        }
        else
        {
            return spellsOfTypePVec.at( utilz::random::Int(spellsOfTypePVec.size() - 1) );
        }
    }


    const TurnActionInfo TurnDecider::DecideIfFlying(const TurnInfo &               TURN_INFO,
                                                     const creature::CreaturePtrC_t CREATURE_DECIDING_CPTRC)
    {
        if ((TURN_INFO.GetIsFlying() == false) && (CREATURE_DECIDING_CPTRC->CanFly()))
        {
            if (utilz::random::Float(1.0f) < ChanceFromFrequency(TURN_INFO.GetStrategyInfo().FlyFreq()))
            {
                return TurnActionInfo(TurnAction::Fly);
            }
        }

        return TurnActionInfo();
    }

}
}
