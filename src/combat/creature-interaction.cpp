// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// creature-interaction.cpp
//
#include "creature-interaction.hpp"

#include "combat/combat-display.hpp"
#include "combat/combat-text.hpp"
#include "combat/encounter.hpp"
#include "combat/name-position-enum.hpp"
#include "combat/trap.hpp"
#include "creature/algorithms.hpp"
#include "creature/condition.hpp"
#include "creature/creature.hpp"
#include "creature/player-party.hpp"
#include "creature/stats.hpp"
#include "game/game-state.hpp"
#include "game/game.hpp"
#include "item/item.hpp"
#include "misc/config-file.hpp"
#include "misc/log-macros.hpp"
#include "misc/random.hpp"
#include "misc/strings.hpp"
#include "misc/vectors.hpp"
#include "song/song.hpp"
#include "spell/spell.hpp"

#include <algorithm>
#include <cmath>
#include <string>

namespace heroespath
{
namespace combat
{

    CreatureInteraction::CreatureInteraction()
        : armorRatings_()
    {}

    const FightResult CreatureInteraction::Fight(
        const creature::CreaturePtr_t CREATURE_ATTACKING_PTR,
        const creature::CreaturePtr_t CREATURE_DEFENDING_PTR,
        const bool WILL_FORCE_HIT) const
    {
        const auto HIT_INFO_VEC { AttackWithAllWeapons(
            CREATURE_ATTACKING_PTR, CREATURE_DEFENDING_PTR, WILL_FORCE_HIT) };

        const auto CREATURE_EFFECT { CreatureEffect(CREATURE_DEFENDING_PTR, HIT_INFO_VEC) };

        auto turnInfo { Encounter::Instance()->GetTurnInfoCopy(CREATURE_DEFENDING_PTR) };

        if (!turnInfo.GetFirstAttackedByCreaturePtrOpt())
        {
            turnInfo.SetFirstAttackedByCreature(CREATURE_ATTACKING_PTR);
        }

        turnInfo.SetLastAttackedByCreature(CREATURE_ATTACKING_PTR);

        if (CREATURE_EFFECT.GetWasHit())
        {
            turnInfo.SetWasHitLastTurn(true);

            if (!turnInfo.GetFirstHitByCreaturePtrOpt())
            {
                turnInfo.SetFirstHitByCreature(CREATURE_ATTACKING_PTR);
            }

            if (!turnInfo.GetLastHitByCreaturePtrOpt())
            {
                turnInfo.SetLastHitByCreature(CREATURE_ATTACKING_PTR);
            }
        }

        turnInfo.UpdateMostDamagedCreature(
            CREATURE_EFFECT.GetDamageTotal(), CREATURE_ATTACKING_PTR);

        Encounter::Instance()->SetTurnInfo(CREATURE_DEFENDING_PTR, turnInfo);

        return FightResult(CREATURE_EFFECT);
    }

    void CreatureInteraction::HandleDamage(
        const creature::CreaturePtr_t CREATURE_DEFENDING_PTR,
        HitInfoVec_t & hitInfoVec,
        const Health_t & HEALTH_ADJ,
        creature::CondEnumVec_t & condsAddedVec,
        creature::CondEnumVec_t & condsRemovedVec,
        const bool CAN_ADD_CONDITIONS) const
    {
        if (HEALTH_ADJ.IsZero())
        {
            return;
        }

        if (HEALTH_ADJ > 0_health)
        {
            CREATURE_DEFENDING_PTR->HealthCurrentAdj(HEALTH_ADJ);

            if (CREATURE_DEFENDING_PTR->HealthCurrent() == CREATURE_DEFENDING_PTR->HealthNormal())
            {
                RemoveAddedConditions(
                    { creature::Conditions::Dazed, creature::Conditions::Daunted },
                    CREATURE_DEFENDING_PTR,
                    hitInfoVec,
                    condsRemovedVec);

                if (AreAnyOfCondsContained(
                        { creature::Conditions::Panic }, CREATURE_DEFENDING_PTR, hitInfoVec))
                {
                    RemoveAddedCondition(
                        creature::Conditions::Panic,
                        CREATURE_DEFENDING_PTR,
                        hitInfoVec,
                        condsRemovedVec);

                    CREATURE_DEFENDING_PTR->ConditionAdd(creature::Conditions::Daunted);
                    condsAddedVec.emplace_back(creature::Conditions::Daunted);
                }
            }

            RemoveAddedCondition(
                creature::Conditions::Unconscious,
                CREATURE_DEFENDING_PTR,
                hitInfoVec,
                condsRemovedVec);

            return;
        }

        // check if already dead
        if (AreAnyOfCondsContained(
                { creature::Conditions::Dead }, CREATURE_DEFENDING_PTR, hitInfoVec))
        {
            return;
        }

        const auto IS_ALREADY_UNCONSCIOUS { AreAnyOfCondsContained(
            { creature::Conditions::Unconscious }, CREATURE_DEFENDING_PTR, hitInfoVec) };

        // at this point HEALTH_ADJ is negative
        const auto DAMAGE_ABS { HEALTH_ADJ.MakePositiveCopy() };

        // logic needed to determine if damage will kill
        const auto IS_NONPLAYER_CHARACTER { CREATURE_DEFENDING_PTR->IsPlayerCharacter() == false };

        const auto IS_DAMAGE_DOUBLE { (
            DAMAGE_ABS > (CREATURE_DEFENDING_PTR->HealthNormal() * 2_health)) };

        const auto WILL_DAMAGE_KILL { (DAMAGE_ABS >= CREATURE_DEFENDING_PTR->HealthCurrent()) };

        const auto NOT_PIXIE { (CREATURE_DEFENDING_PTR->IsPixie() == false) };

        // check if the damage will kill
        if (IS_ALREADY_UNCONSCIOUS
            || (IS_NONPLAYER_CHARACTER && ((IS_DAMAGE_DOUBLE && NOT_PIXIE) || WILL_DAMAGE_KILL)))
        {
            CREATURE_DEFENDING_PTR->HealthCurrentSet(0_health);
            CREATURE_DEFENDING_PTR->ConditionAdd(creature::Conditions::Dead);
            condsAddedVec.emplace_back(creature::Conditions::Dead);

            auto turnInfo { combat::Encounter::Instance()->GetTurnInfoCopy(
                CREATURE_DEFENDING_PTR) };
            turnInfo.SetIsFlying(false);
            turnInfo.SetWasHitLastTurn(true);
            turnInfo.SetTurnActionInfo(TurnActionInfo());
            combat::Encounter::Instance()->SetTurnInfo(CREATURE_DEFENDING_PTR, turnInfo);

            // remove the unconscious condition if already there
            if (IS_ALREADY_UNCONSCIOUS)
            {
                RemoveAddedCondition(
                    creature::Conditions::Unconscious,
                    CREATURE_DEFENDING_PTR,
                    hitInfoVec,
                    condsRemovedVec);
            }

            return;
        }

        if (DAMAGE_ABS > 0_health)
        {
            // wake from natural sleep if hit
            RemoveAddedCondition(
                creature::Conditions::AsleepNatural,
                CREATURE_DEFENDING_PTR,
                hitInfoVec,
                condsRemovedVec);
        }

        CREATURE_DEFENDING_PTR->HealthCurrentAdj(-DAMAGE_ABS);
        if (CREATURE_DEFENDING_PTR->HealthCurrent() < 0_health)
        {
            CREATURE_DEFENDING_PTR->HealthCurrentSet(0_health);
        }

        if (IS_ALREADY_UNCONSCIOUS == false)
        {
            if (CREATURE_DEFENDING_PTR->IsPlayerCharacter()
                && (CREATURE_DEFENDING_PTR->HealthCurrent() <= 0_health))
            {
                CREATURE_DEFENDING_PTR->HealthCurrentSet(1_health);

                CREATURE_DEFENDING_PTR->ConditionAdd(creature::Conditions::Unconscious);
                condsAddedVec.emplace_back(creature::Conditions::Unconscious);

                RemoveAddedConditions(
                    { creature::Conditions::Dazed,
                      creature::Conditions::Daunted,
                      creature::Conditions::AsleepNatural,
                      creature::Conditions::AsleepMagical,
                      creature::Conditions::Bold,
                      creature::Conditions::Heroic,
                      creature::Conditions::Panic,
                      creature::Conditions::Tripped },
                    CREATURE_DEFENDING_PTR,
                    hitInfoVec,
                    condsRemovedVec);

                Encounter::Instance()->SetIsFlying(CREATURE_DEFENDING_PTR, false);
            }
            else
            {
                if (CAN_ADD_CONDITIONS)
                {
                    AddConditionsBasedOnDamage(
                        CREATURE_DEFENDING_PTR,
                        DAMAGE_ABS,
                        condsAddedVec,
                        condsRemovedVec,
                        hitInfoVec);
                }
            }
        }
    }

    bool CreatureInteraction::ProcessConditionEffects(
        const game::Phase::Enum,
        const creature::CreaturePtr_t CREATURE_PTR,
        HitInfoVec_t & hitInfoVec_OuParam) const
    {
        auto condsPVec { CREATURE_PTR->ConditionsPVec() };

        std::sort(condsPVec.begin(), condsPVec.end(), [](const auto A, const auto B) {
            return (A->Severity() > B->Severity());
        });

        auto hasTurnBeenConsumed { false };
        for (const auto & NEXT_COND_PTR : condsPVec)
        {
            NEXT_COND_PTR->PerTurnEffect(
                *this, CREATURE_PTR, hitInfoVec_OuParam, hasTurnBeenConsumed);
        }

        return hasTurnBeenConsumed;
    }

    void CreatureInteraction::AddConditionsBasedOnDamage(
        const creature::CreaturePtr_t CREATURE_DEFENDING_PTR,
        const Health_t & DAMAGE_ABS,
        creature::CondEnumVec_t & condsAddedVecParam,
        creature::CondEnumVec_t & condsRemovedVecParam,
        HitInfoVec_t & hitInfoVec) const
    {
        creature::CondEnumVec_t condsVecToAdd { creature::Conditions::Daunted,
                                                creature::Conditions::Dazed,
                                                creature::Conditions::Tripped };

        misc::RandomShuffle(condsVecToAdd);

        if (DAMAGE_ABS > (CREATURE_DEFENDING_PTR->HealthNormal()
                          - (CREATURE_DEFENDING_PTR->HealthNormal() / 8_health)))
        {
            // leave condsVec as is, with all three conditions contained
        }
        else if (
            DAMAGE_ABS > (CREATURE_DEFENDING_PTR->HealthNormal()
                          - (CREATURE_DEFENDING_PTR->HealthNormal() / 4_health)))
        {
            condsVecToAdd.resize(2);
        }
        else if (
            DAMAGE_ABS > (CREATURE_DEFENDING_PTR->HealthNormal()
                          - (CREATURE_DEFENDING_PTR->HealthNormal() / 2_health)))
        {
            condsVecToAdd.resize(1);
        }
        else
        {
            return;
        }

        const auto CHANCE_CONDS_ADDED_RATIO { misc::ConfigFile::Instance()->ValueOrDefault<float>(
            "fight-chance-conditions-added-from-damage-ratio") };

        for (const auto NEXT_COND_ENUM : condsVecToAdd)
        {
            // only a 50/50 chance of adding conditions
            if (misc::Random(1.0f) < CHANCE_CONDS_ADDED_RATIO)
            {
                if ((NEXT_COND_ENUM == creature::Conditions::Dazed)
                    && AreAnyOfCondsContained(
                           { creature::Conditions::AsleepMagical,
                             creature::Conditions::AsleepNatural,
                             creature::Conditions::Dazed,
                             creature::Conditions::Unconscious },
                           CREATURE_DEFENDING_PTR,
                           hitInfoVec))
                {
                    continue;
                }

                if ((NEXT_COND_ENUM == creature::Conditions::Daunted)
                    && AreAnyOfCondsContained(
                           { creature::Conditions::Daunted, creature::Conditions::Unconscious },
                           CREATURE_DEFENDING_PTR,
                           hitInfoVec))
                {
                    continue;
                }

                if ((NEXT_COND_ENUM == creature::Conditions::Tripped)
                    && AreAnyOfCondsContained(
                           { creature::Conditions::Tripped,
                             creature::Conditions::AsleepMagical,
                             creature::Conditions::AsleepNatural,
                             creature::Conditions::Unconscious,
                             creature::Conditions::Pounced },
                           CREATURE_DEFENDING_PTR,
                           hitInfoVec))
                {
                    continue;
                }

                if ((NEXT_COND_ENUM == creature::Conditions::Daunted)
                    && AreAnyOfCondsContained(
                           { creature::Conditions::Heroic, creature::Conditions::Bold },
                           CREATURE_DEFENDING_PTR,
                           hitInfoVec))
                {
                    RemoveAddedConditions(
                        { creature::Conditions::Heroic, creature::Conditions::Bold },
                        CREATURE_DEFENDING_PTR,
                        hitInfoVec,
                        condsRemovedVecParam);
                    continue;
                }

                CREATURE_DEFENDING_PTR->ConditionAdd(NEXT_COND_ENUM);
                condsAddedVecParam.emplace_back(NEXT_COND_ENUM);
            }
        }
    }

    bool CreatureInteraction::RemoveAddedConditions(
        const creature::CondEnumVec_t & CONDS_VEC,
        const creature::CreaturePtr_t CREATURE_PTR,
        HitInfoVec_t & hitInfoVec,
        creature::CondEnumVec_t & condsRemovedVec) const
    {
        auto wasAnyRemoved { false };

        for (const auto NEXT_COND_ENUM : CONDS_VEC)
        {
            if (RemoveAddedCondition(NEXT_COND_ENUM, CREATURE_PTR, hitInfoVec, condsRemovedVec))
            {
                wasAnyRemoved = true;
            }
        }

        return wasAnyRemoved;
    }

    bool CreatureInteraction::RemoveAddedCondition(
        const creature::Conditions::Enum COND_ENUM,
        const creature::CreaturePtr_t CREATURE_PTR,
        HitInfoVec_t & hitInfoVec,
        creature::CondEnumVec_t & condsRemovedVec) const
    {
        for (auto & nextHitInfo : hitInfoVec)
        {
            nextHitInfo.CondsAddedRemove(COND_ENUM);
        }

        if (CREATURE_PTR->HasCondition(COND_ENUM))
        {
            CREATURE_PTR->ConditionRemove(COND_ENUM);
            condsRemovedVec.emplace_back(COND_ENUM);
            return true;
        }
        else
        {
            return false;
        }
    }

    const FightResult CreatureInteraction::Cast(
        const spell::SpellPtr_t SPELL_PTR,
        const creature::CreaturePtr_t CREATURE_CASTING_PTR,
        const creature::CreaturePVec_t & creaturesCastUponPVec) const
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (creaturesCastUponPVec.empty() == false),
            "combat::CreatureInteraction::Cast(spell="
                << SPELL_PTR->Name()
                << ", creature_casting=" << CREATURE_CASTING_PTR->NameAndRaceAndRole()
                << ", creatures_cast_upon=empty) was given an empty creaturesCastUponPVec.");

        CREATURE_CASTING_PTR->ManaAdj(-SPELL_PTR->ManaCost());

        if (((SPELL_PTR->Target() == TargetType::SingleCompanion)
             || (SPELL_PTR->Target() == TargetType::SingleOpponent))
            && (creaturesCastUponPVec.size() > 1))
        {
            std::ostringstream ssErr;
            ssErr << "combat::CreatureInteraction::Cast(spell=" << SPELL_PTR->Name()
                  << ", creature_casting=" << CREATURE_CASTING_PTR->NameAndRaceAndRole()
                  << ", creatures_cast_upon=\""
                  << creature::Algorithms::Names(
                         creaturesCastUponPVec,
                         0,
                         misc::JoinOpt::None,
                         creature::Algorithms::NamesOpt::WithRaceAndRole)
                  << "\") spell target_type=" << TargetType::ToString(SPELL_PTR->Target())
                  << " but there were " << creaturesCastUponPVec.size()
                  << " creatures being cast upon.  There should have been only 1.";

            throw std::runtime_error(ssErr.str());
        }

        // update caster's TurnInfo
        auto casterTurnInfo { Encounter::Instance()->GetTurnInfoCopy(CREATURE_CASTING_PTR) };
        casterTurnInfo.CastCountIncrement();
        Encounter::Instance()->SetTurnInfo(CREATURE_CASTING_PTR, casterTurnInfo);

        // update caster's TurnActionInfo
        Encounter::Instance()->SetTurnActionInfo(
            CREATURE_CASTING_PTR, TurnActionInfo(SPELL_PTR, creaturesCastUponPVec));

        // attempt to have spell effect each target creature
        CreatureEffectVec_t creatureEffectVec;
        for (auto & nextCreatureCastUponPtr : creaturesCastUponPVec)
        {
            if (nextCreatureCastUponPtr->IsAlive() == false)
            {
                const ContentAndNamePos CNP(" is dead.", NamePosition::TargetBefore);

                const HitInfo HIT_INFO(false, SPELL_PTR, CNP);

                creatureEffectVec.emplace_back(
                    CreatureEffect(nextCreatureCastUponPtr, HitInfoVec_t(1, HIT_INFO)));
            }
            else
            {
                ContentAndNamePos actionPhraseCNP;
                Health_t healthAdj { 0_health };
                creature::CondEnumVec_t condsAddedVec;
                creature::CondEnumVec_t condsRemovedVec;

                const auto DID_SPELL_SUCCEED { SPELL_PTR->EffectCreature(
                    CREATURE_CASTING_PTR,
                    nextCreatureCastUponPtr,
                    healthAdj,
                    condsAddedVec,
                    condsRemovedVec,
                    actionPhraseCNP) };

                HitInfoVec_t hitInfoVec;

                if (DID_SPELL_SUCCEED)
                {
                    HandleDamage(
                        nextCreatureCastUponPtr,
                        hitInfoVec,
                        healthAdj,
                        condsAddedVec,
                        condsRemovedVec);
                }

                hitInfoVec.emplace_back(HitInfo(
                    DID_SPELL_SUCCEED,
                    SPELL_PTR,
                    actionPhraseCNP,
                    healthAdj,
                    condsAddedVec,
                    condsRemovedVec));

                creatureEffectVec.emplace_back(CreatureEffect(nextCreatureCastUponPtr, hitInfoVec));

                // update target's TurnInfo
                auto turnInfo { Encounter::Instance()->GetTurnInfoCopy(nextCreatureCastUponPtr) };

                turnInfo.SetWasHitLastTurn(false);

                if ((SPELL_PTR->Effect() == combat::EffectType::CreatureHarmDamage)
                    || (SPELL_PTR->Effect() == combat::EffectType::CreatureHarmMisc))
                {
                    if (!turnInfo.GetFirstAttackedByCreaturePtrOpt())
                    {
                        turnInfo.SetFirstAttackedByCreature(CREATURE_CASTING_PTR);
                    }

                    turnInfo.SetLastAttackedByCreature(CREATURE_CASTING_PTR);

                    if (DID_SPELL_SUCCEED)
                    {
                        if (!turnInfo.GetFirstHitByCreaturePtrOpt())
                        {
                            turnInfo.SetFirstHitByCreature(CREATURE_CASTING_PTR);
                        }

                        turnInfo.SetLastHitByCreature(CREATURE_CASTING_PTR);

                        turnInfo.UpdateMostDamagedCreature(
                            healthAdj.MakePositiveCopy(), CREATURE_CASTING_PTR);

                        turnInfo.SetWasHitLastTurn(true);
                    }
                }

                turnInfo.SetLastToCastCreature(CREATURE_CASTING_PTR);

                if (DID_SPELL_SUCCEED && (!turnInfo.GetFirstToCastCreaturePtrOpt()))
                {
                    turnInfo.SetFirstToCastCreature(CREATURE_CASTING_PTR);
                }

                Encounter::Instance()->SetTurnInfo(CREATURE_CASTING_PTR, turnInfo);
            }
        }

        return FightResult(creatureEffectVec);
    }

    const FightResult CreatureInteraction::PlaySong(
        const song::SongPtr_t SONG_PTR,
        const creature::CreaturePtr_t CREATURE_PLAYING_PTR,
        const creature::CreaturePVec_t & CREATURES_LISTENING_PVEC) const
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (CREATURES_LISTENING_PVEC.empty() == false),
            "combat::CreatureInteraction::PlaySong(song="
                << SONG_PTR->Name()
                << ", creature_playing=" << CREATURE_PLAYING_PTR->NameAndRaceAndRole()
                << ", creatures_listening=empty) was given an empty creaturesListeningPVec.");

        CREATURE_PLAYING_PTR->ManaAdj(-SONG_PTR->ManaCost());

        if (((SONG_PTR->Target() == TargetType::SingleCompanion)
             || (SONG_PTR->Target() == TargetType::SingleOpponent))
            && (CREATURES_LISTENING_PVEC.size() > 1))
        {
            std::ostringstream ssErr;
            ssErr << "combat::CreatureInteraction::PlaySong(song=" << SONG_PTR->Name()
                  << ", creature_playing=" << CREATURE_PLAYING_PTR->NameAndRaceAndRole()
                  << ", creatures_listening=\""
                  << creature::Algorithms::Names(
                         CREATURES_LISTENING_PVEC,
                         0,
                         misc::JoinOpt::None,
                         creature::Algorithms::NamesOpt::WithRaceAndRole)
                  << "\") song target_type=" << TargetType::ToString(SONG_PTR->Target())
                  << " but there were " << CREATURES_LISTENING_PVEC.size()
                  << " creatures listening.  There should have been only 1.";

            throw std::runtime_error(ssErr.str());
        }

        // update player's TurnInfo
        auto playerTurnInfo { Encounter::Instance()->GetTurnInfoCopy(CREATURE_PLAYING_PTR) };
        playerTurnInfo.SongCountIncrement();
        Encounter::Instance()->SetTurnInfo(CREATURE_PLAYING_PTR, playerTurnInfo);

        // update player's TurnActionInfo
        Encounter::Instance()->SetTurnActionInfo(
            CREATURE_PLAYING_PTR, TurnActionInfo(SONG_PTR, CREATURES_LISTENING_PVEC));

        // attempt to have spell effect each target creature
        CreatureEffectVec_t creatureEffectVec;
        for (auto & nextCreatureCastUponPtr : CREATURES_LISTENING_PVEC)
        {
            if (nextCreatureCastUponPtr->IsAlive() == false)
            {
                const ContentAndNamePos CNP(" is dead.", NamePosition::TargetBefore);

                const HitInfo HIT_INFO(false, SONG_PTR, CNP);

                creatureEffectVec.emplace_back(
                    CreatureEffect(nextCreatureCastUponPtr, HitInfoVec_t(1, HIT_INFO)));
            }
            else
            {
                ContentAndNamePos actionPhraseCNP;
                Health_t healthAdj { 0_health };
                creature::CondEnumVec_t condsAddedVec;
                creature::CondEnumVec_t condsRemovedVec;

                const auto DID_SONG_SUCCEED { SONG_PTR->EffectCreature(
                    CREATURE_PLAYING_PTR,
                    nextCreatureCastUponPtr,
                    healthAdj,
                    condsAddedVec,
                    condsRemovedVec,
                    actionPhraseCNP) };

                HitInfoVec_t hitInfoVec;

                if (DID_SONG_SUCCEED)
                {
                    HandleDamage(
                        nextCreatureCastUponPtr,
                        hitInfoVec,
                        healthAdj,
                        condsAddedVec,
                        condsRemovedVec);
                }

                hitInfoVec.emplace_back(HitInfo(
                    DID_SONG_SUCCEED,
                    SONG_PTR,
                    actionPhraseCNP,
                    healthAdj,
                    condsAddedVec,
                    condsRemovedVec));

                creatureEffectVec.emplace_back(CreatureEffect(nextCreatureCastUponPtr, hitInfoVec));

                // handle TurnInfo
                auto turnInfo { Encounter::Instance()->GetTurnInfoCopy(nextCreatureCastUponPtr) };

                if (!turnInfo.GetFirstToMakeMusicCreaturePtrOpt())
                {
                    turnInfo.SetFirstToMakeMusicCreature(CREATURE_PLAYING_PTR);
                }

                turnInfo.SetLastToMakeMusicCreature(CREATURE_PLAYING_PTR);

                turnInfo.SetWasHitLastTurn(false);

                if ((SONG_PTR->Effect() == combat::EffectType::CreatureHarmDamage)
                    || (SONG_PTR->Effect() == combat::EffectType::CreatureHarmMisc))
                {
                    if (!turnInfo.GetFirstAttackedByCreaturePtrOpt())
                    {
                        turnInfo.SetFirstAttackedByCreature(CREATURE_PLAYING_PTR);
                    }

                    if (DID_SONG_SUCCEED)
                    {
                        turnInfo.SetFirstHitByCreature(CREATURE_PLAYING_PTR);
                        turnInfo.SetLastAttackedByCreature(CREATURE_PLAYING_PTR);
                        turnInfo.SetLastHitByCreature(CREATURE_PLAYING_PTR);
                        turnInfo.UpdateMostDamagedCreature(
                            healthAdj.MakePositiveCopy(), CREATURE_PLAYING_PTR);
                        turnInfo.SetWasHitLastTurn(true);
                    }
                }

                Encounter::Instance()->SetTurnInfo(nextCreatureCastUponPtr, turnInfo);
            }
        }

        return FightResult(creatureEffectVec);
    }

    const FightResult CreatureInteraction::Pounce(
        const creature::CreaturePtr_t CREATURE_POUNCING_PTR,
        const creature::CreaturePtr_t CREATURE_DEFENDING_PTR) const
    {
        // update player's TurnActionInfo
        Encounter::Instance()->SetTurnActionInfo(
            CREATURE_POUNCING_PTR,
            TurnActionInfo(
                ((Encounter::Instance()->GetTurnInfoCopy(CREATURE_POUNCING_PTR).GetIsFlying())
                     ? TurnAction::SkyPounce
                     : TurnAction::LandPounce),
                { CREATURE_DEFENDING_PTR }));

        HitInfo hitInfo;

        const auto DID_ROLL_SUCCEED { creature::Stats::Versus(
            CREATURE_POUNCING_PTR,
            { creature::Traits::Speed, creature::Traits::Accuracy },
            CREATURE_DEFENDING_PTR,
            { creature::Traits::Speed },
            0,
            0,
            static_cast<creature::Stats::With>(
                creature::Stats::With::Luck | creature::Stats::With::RaceRoleBonus
                | creature::Stats::With::RankBonus | creature::Stats::With::PlayerNaturalWins)) };

        const auto DID_SUCCEED { (
            DID_ROLL_SUCCEED || CREATURE_DEFENDING_PTR->HasCondition(creature::Conditions::Tripped)
            || CREATURE_DEFENDING_PTR->HasCondition(creature::Conditions::AsleepNatural)
            || CREATURE_DEFENDING_PTR->HasCondition(creature::Conditions::AsleepMagical)
            || CREATURE_DEFENDING_PTR->HasCondition(creature::Conditions::Unconscious)) };

        Health_t healthAdj { 0_health };

        if (DID_SUCCEED)
        {
            // TODO should pouncing do some damage?
            healthAdj = 0_health;

            HitInfoVec_t hitInfoVec;
            creature::CondEnumVec_t condsAddedVec;
            creature::CondEnumVec_t condsRemovedVec;

            if (CREATURE_DEFENDING_PTR->HasCondition(creature::Conditions::Tripped))
            {
                CREATURE_DEFENDING_PTR->ConditionRemove(creature::Conditions::Tripped);
                condsRemovedVec.emplace_back(creature::Conditions::Tripped);
            }

            HandleDamage(
                CREATURE_DEFENDING_PTR, hitInfoVec, healthAdj, condsAddedVec, condsRemovedVec);

            CREATURE_DEFENDING_PTR->ConditionAdd(creature::Conditions::Pounced);

            const ContentAndNamePos CNP("", " pounces on ", ".", NamePosition::SourceThenTarget);

            hitInfo = HitInfo(
                true,
                HitType::Pounce,
                CNP,
                healthAdj,
                { creature::Conditions::Pounced },
                creature::CondEnumVec_t());
        }
        else
        {
            const ContentAndNamePos CNP(
                "", " tried to pounce on ", "but missed.", NamePosition::SourceThenTarget);

            hitInfo = HitInfo(false, HitType::Pounce, CNP);
        }

        // update defender's TurnInfo
        auto turnInfo { Encounter::Instance()->GetTurnInfoCopy(CREATURE_DEFENDING_PTR) };

        if (!turnInfo.GetFirstAttackedByCreaturePtrOpt())
        {
            turnInfo.SetFirstAttackedByCreature(CREATURE_POUNCING_PTR);
        }

        turnInfo.SetLastAttackedByCreature(CREATURE_POUNCING_PTR);

        turnInfo.SetWasHitLastTurn(false);

        if (DID_SUCCEED)
        {
            if (!turnInfo.GetFirstHitByCreaturePtrOpt())
            {
                turnInfo.SetFirstHitByCreature(CREATURE_POUNCING_PTR);
            }

            turnInfo.SetLastHitByCreature(CREATURE_POUNCING_PTR);
            turnInfo.UpdateMostDamagedCreature(healthAdj.MakePositiveCopy(), CREATURE_POUNCING_PTR);
            turnInfo.SetWasHitLastTurn(true);
        }

        Encounter::Instance()->SetTurnInfo(CREATURE_DEFENDING_PTR, turnInfo);

        return FightResult(CreatureEffect(CREATURE_DEFENDING_PTR, HitInfoVec_t(1, hitInfo)));
    }

    const FightResult CreatureInteraction::Roar(
        const creature::CreaturePtr_t CREATURE_ROARING_PTR,
        const CombatDisplayPtr_t COMBAT_DISPLAY_PTR) const
    {
        const auto LISTENING_CREATURES_PVEC { COMBAT_DISPLAY_PTR->GetCreaturesInRoaringDistance(
            CREATURE_ROARING_PTR) };

        // update player's TurnActionInfo
        Encounter::Instance()->SetTurnActionInfo(
            CREATURE_ROARING_PTR, TurnActionInfo(TurnAction::Roar, LISTENING_CREATURES_PVEC));

        CreatureEffectVec_t creatureEffectsVec;

        const auto IS_ROARING_CREATURE_FLYING {
            combat::Encounter::Instance()->GetTurnInfoCopy(CREATURE_ROARING_PTR).GetIsFlying()
        };

        // Give each defending creature a chance to resist being panicked.
        // The farther away each defending creature is the better chance
        // of resisting he/she/it has.
        for (const auto & NEXT_DEFEND_CREATURE_PTR : LISTENING_CREATURES_PVEC)
        {
            if (NEXT_DEFEND_CREATURE_PTR->HasCondition(creature::Conditions::Panic))
            {
                const ContentAndNamePos CNP(" is already panicked.", NamePosition::TargetBefore);

                const HitInfo HIT_INFO(false, HitType::Roar, CNP);

                creatureEffectsVec.emplace_back(
                    CreatureEffect(NEXT_DEFEND_CREATURE_PTR, HitInfoVec_t(1, HIT_INFO)));

                continue;
            }

            auto nextBlockingDisatnce { misc::Abs(COMBAT_DISPLAY_PTR->GetBlockingDistanceBetween(
                CREATURE_ROARING_PTR, NEXT_DEFEND_CREATURE_PTR)) };

            // if flying, then consider it farther away and less likely to be panicked
            if (Encounter::Instance()->GetTurnInfoCopy(NEXT_DEFEND_CREATURE_PTR).GetIsFlying()
                && (IS_ROARING_CREATURE_FLYING == false))
            {
                ++nextBlockingDisatnce;
            }

            const creature::Trait_t DISATANCE_BONUS { nextBlockingDisatnce * 2 };

            if (creature::Stats::Versus(
                    CREATURE_ROARING_PTR,
                    creature::Traits::Strength,
                    NEXT_DEFEND_CREATURE_PTR,
                    creature::Traits::Intelligence,
                    0,
                    DISATANCE_BONUS,
                    static_cast<creature::Stats::With>(
                        creature::Stats::With::Luck | creature::Stats::With::RaceRoleBonus
                        | creature::Stats::With::RankBonus
                        | creature::Stats::With::PlayerNaturalWins)))
            {
                // no TurnInfo settings need to be made for roar

                // remove the Daunted condition before adding the Panicked condition
                NEXT_DEFEND_CREATURE_PTR->ConditionRemove(creature::Conditions::Daunted);

                NEXT_DEFEND_CREATURE_PTR->ConditionAdd(creature::Conditions::Panic);

                const ContentAndNamePos CNP("'s roar panics ", NamePosition::SourceThenTarget);

                const HitInfo HIT_INFO(
                    true,
                    HitType::Roar,
                    CNP,
                    0_health,
                    creature::CondEnumVec_t(1, creature::Conditions::Panic),
                    creature::CondEnumVec_t());

                creatureEffectsVec.emplace_back(
                    CreatureEffect(NEXT_DEFEND_CREATURE_PTR, HitInfoVec_t(1, HIT_INFO)));
            }
            else
            {
                const ContentAndNamePos CNP(
                    "", " resisted the fear of ", "'s roar.", NamePosition::TargetThenSource);

                const HitInfo HIT_INFO(false, HitType::Roar, CNP);

                creatureEffectsVec.emplace_back(
                    CreatureEffect(NEXT_DEFEND_CREATURE_PTR, HitInfoVec_t(1, HIT_INFO)));
            }
        }

        return FightResult(creatureEffectsVec);
    }

    const creature::CreaturePtr_t CreatureInteraction::FindNonPlayerCreatureToAttack(
        const creature::CreaturePtr_t CREATURE_ATTACKING_PTR,
        const CombatDisplayPtr_t COMBAT_DISPLAY_PTR) const
    {
        const auto ATTACKABLE_NONPLAYER_CREATURES_PVEC {
            COMBAT_DISPLAY_PTR->FindCreaturesThatCanBeAttackedOfType(CREATURE_ATTACKING_PTR, false)
        };

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (ATTACKABLE_NONPLAYER_CREATURES_PVEC.empty() == false),
            "combat::CreatureInteraction::HandleAttack() FindNonPlayerCreatureToAttack() "
                << "returned no attackable creatures.");

        // attack those with the lowest relative health first, which will correspond
        // to the health bar seen on screen
        const auto LIVE_ATTBLE_LOWH_NP_CRTS_PVEC { creature::Algorithms::FindLowestHealthRatio(
            ATTACKABLE_NONPLAYER_CREATURES_PVEC) };

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (LIVE_ATTBLE_LOWH_NP_CRTS_PVEC.empty() == false),
            "combat::CreatureInteraction::HandleAttack() FindNonPlayerCreatureToAttack() "
            "returned "
                << "no LIVING LOWEST HEALTH RATIO attackable creatures.");

        // skip creatures who are not a threat
        const auto LIVE_ATTBLE_LOWH_NP_NOTPERMDIS_CRTS_PVEC {
            creature::Algorithms::FindByConditionMeaningNotAThreatPermenantly(
                LIVE_ATTBLE_LOWH_NP_CRTS_PVEC,
                creature::Algorithms::CondSingleOpt::DoesNotHave,
                creature::UnconOpt::Include)
        };

        // attack those that are temporarily disabled first
        const auto LIVE_ATTBLE_LOWH_NP_NOTPERMDIS_TMPDIS_CRTS_PVEC {
            creature::Algorithms::FindByConditionMeaningNotAThreatTemporarily(
                LIVE_ATTBLE_LOWH_NP_NOTPERMDIS_CRTS_PVEC)
        };

        if (LIVE_ATTBLE_LOWH_NP_NOTPERMDIS_TMPDIS_CRTS_PVEC.empty())
        {
            if (LIVE_ATTBLE_LOWH_NP_NOTPERMDIS_CRTS_PVEC.empty())
            {
                return misc::RandomSelect(COMBAT_DISPLAY_PTR->FindClosestAmongOfType(
                    CREATURE_ATTACKING_PTR, LIVE_ATTBLE_LOWH_NP_CRTS_PVEC, false));
            }
            else
            {
                return misc::RandomSelect(COMBAT_DISPLAY_PTR->FindClosestAmongOfType(
                    CREATURE_ATTACKING_PTR, LIVE_ATTBLE_LOWH_NP_NOTPERMDIS_CRTS_PVEC, false));
            }
        }
        else
        {
            return misc::RandomSelect(COMBAT_DISPLAY_PTR->FindClosestAmongOfType(
                CREATURE_ATTACKING_PTR, LIVE_ATTBLE_LOWH_NP_NOTPERMDIS_TMPDIS_CRTS_PVEC, false));
        }
    }

    const FightResult CreatureInteraction::TreasureTrap(
        const Trap & TRAP, const creature::CreaturePtr_t CREATURE_PICKING_THE_LOCK_PTR) const
    {
        const auto HURT_CREATURE_PTRS(
            RandomSelectWhoIsHurtByTrap(TRAP, CREATURE_PICKING_THE_LOCK_PTR));

        CreatureEffectVec_t creatureEffectVec;
        for (auto & nextCreatureHurtPtr : HURT_CREATURE_PTRS)
        {
            const auto HEALTH_ADJ { -TRAP.RandomDamage() };

            creature::CondEnumVec_t condsAddedVec;
            creature::CondEnumVec_t condsRemovedVec;

            HitInfoVec_t hitInfoVec;

            HandleDamage(
                nextCreatureHurtPtr, hitInfoVec, HEALTH_ADJ, condsAddedVec, condsRemovedVec);

            hitInfoVec.emplace_back(
                HitInfo(HEALTH_ADJ, TRAP.HitVerb(), condsAddedVec, condsRemovedVec));

            creatureEffectVec.emplace_back(CreatureEffect(nextCreatureHurtPtr, hitInfoVec));
        }

        return FightResult(creatureEffectVec);
    }

    const creature::CreaturePVec_t CreatureInteraction::RandomSelectWhoIsHurtByTrap(
        const Trap & TRAP, const creature::CreaturePtr_t CREATURE_PICKING_THE_LOCK_PTR) const
    {
        creature::CreaturePVec_t creaturesHurtPtrs { CREATURE_PICKING_THE_LOCK_PTR };

        auto allCharacterPtrs { game::Game::Instance()->State().Party().Characters() };
        misc::RandomShuffle(allCharacterPtrs);

        const auto NUM_CHARACTERS_TO_ADD { TRAP.RandomEffectedPlayersCount() - 1 };
        std::size_t numCharactersAdded { 0 };

        for (std::size_t i(0);
             (i < allCharacterPtrs.size()) && (numCharactersAdded < NUM_CHARACTERS_TO_ADD);
             ++i)
        {
            const auto NEXT_CHARACTER_PTR { allCharacterPtrs[i] };

            // only living characters will be hurt by traps
            if (NEXT_CHARACTER_PTR->IsAlive()
                && (NEXT_CHARACTER_PTR != CREATURE_PICKING_THE_LOCK_PTR))
            {
                creaturesHurtPtrs.emplace_back(NEXT_CHARACTER_PTR);
                ++numCharactersAdded;
            }
        }

        return creaturesHurtPtrs;
    }

    const HitInfoVec_t CreatureInteraction::AttackWithAllWeapons(
        const creature::CreaturePtr_t CREATURE_ATTACKING_PTR,
        const creature::CreaturePtr_t CREATURE_DEFENDING_PTR,
        const bool WILL_FORCE_HIT) const
    {
        HitInfoVec_t hitInfoVec;

        for (const auto & NEXT_ITEM_PTR : CREATURE_ATTACKING_PTR->HeldWeapons())
        {
            if (NEXT_ITEM_PTR->IsWeapon())
            {
                const auto NEXT_HIT_INFO { AttackWithSingleWeapon(
                    hitInfoVec,
                    NEXT_ITEM_PTR,
                    CREATURE_ATTACKING_PTR,
                    CREATURE_DEFENDING_PTR,
                    WILL_FORCE_HIT) };

                hitInfoVec.emplace_back(NEXT_HIT_INFO);

                if (NEXT_HIT_INFO.CondsAddedContains(creature::Conditions::Dead)
                    || CREATURE_DEFENDING_PTR->HasCondition(creature::Conditions::Dead))
                {
                    break;
                }
            }
        }

        return hitInfoVec;
    }

    // Determine if attacking creature's accuracy overcomes the defending
    // creature's speed to see if there was a hit.
    const HitInfo CreatureInteraction::AttackWithSingleWeapon(
        HitInfoVec_t & hitInfoVec,
        const item::ItemPtr_t WEAPON_PTR,
        const creature::CreaturePtr_t CREATURE_ATTACKING_PTR,
        const creature::CreaturePtr_t CREATURE_DEFENDING_PTR,
        const bool WILL_FORCE_HIT) const
    {
        auto hasHitBeenDetermined { false };
        auto wasHit { false };

        const auto ATTACK_ACC_RAW { CREATURE_ATTACKING_PTR->Accuracy() };
        auto attackAccToUse { ATTACK_ACC_RAW };

        // If the attacking creature is an archer who is using a projectile weapon,
        // then add an accuracy bonus.
        if ((CREATURE_ATTACKING_PTR->Role() == creature::role::Archer)
            && (WEAPON_PTR->WeaponType() & item::weapon_type::Projectile))
        {
            const auto ARCHER_ACC_BONUS_RATIO { misc::ConfigFile::Instance()->ValueOrDefault<float>(
                "fight-archer-projectile-accuracy-bonus-ratio") };

            attackAccToUse
                += Accuracy_t::Make(ATTACK_ACC_RAW.GetAs<float>() * ARCHER_ACC_BONUS_RATIO);

            const auto ARCHER_RANK_BONUS_RATIO {
                misc::ConfigFile::Instance()->ValueOrDefault<float>(
                    "fight-archer-projectile-rank-bonus-ratio")
            };

            attackAccToUse += Accuracy_t::Make(
                CREATURE_ATTACKING_PTR->Rank().GetAs<float>() * ARCHER_RANK_BONUS_RATIO);
        }

        const auto ATTACK_ACC_RAND_MIN { Accuracy_t::Make(attackAccToUse.GetAs<float>() * 0.4f) };
        const auto ATTACK_ACC_RAND_MAX { misc::Max(ATTACK_ACC_RAW, attackAccToUse) };

        const auto ATTACK_ACC_RAND { misc::Random(ATTACK_ACC_RAND_MIN, ATTACK_ACC_RAND_MAX) };

        const auto STAT_RATIO_AMAZING { misc::ConfigFile::Instance()->ValueOrDefault<float>(
            "fight-stats-amazing-ratio") };

        const auto STAT_HIGHER_THAN_AVERAGE {
            misc::ConfigFile::Instance()->ValueOrDefault<creature::Trait_t>(
                "fight-stats-base-high-val")
        };

        const auto IS_ATTACK_AMAZING_ACC {
            (attackAccToUse.Get() >= STAT_HIGHER_THAN_AVERAGE)
            && IsValuetHigherThanRatioOfStat(
                   ATTACK_ACC_RAND.Get(), attackAccToUse.Get(), STAT_RATIO_AMAZING)
        };

        const auto DEFEND_SPD_RAW { CREATURE_DEFENDING_PTR->Speed() };
        auto defendSpdToUse { DEFEND_SPD_RAW };

        // If the defending creature is a Pixie then add a speed bonus based on rank.
        if (CREATURE_DEFENDING_PTR->IsPixie())
        {
            defendSpdToUse += Speed_t::Make(
                CREATURE_DEFENDING_PTR->Rank().GetAs<float>()
                * misc::ConfigFile::Instance()->ValueOrDefault<float>(
                      "fight-pixie-defend-speed-rank-bonus-ratio"));
        }

        // If the defending creature is Blocking, then add a speed bonus.
        if (combat::Encounter::Instance()
                ->GetTurnInfoCopy(CREATURE_DEFENDING_PTR)
                .GetTurnActionInfo()
                .Action()
            == TurnAction::Block)
        {
            defendSpdToUse += Speed_t::Make(
                DEFEND_SPD_RAW.GetAs<float>()
                * misc::ConfigFile::Instance()->ValueOrDefault<float>(
                      "fight-block-defend-speed-bonus-ratio"));
        }

        const auto DEFEND_SPD_RAND_MIN { Speed_t::Make(defendSpdToUse.GetAs<float>() * 0.4f) };
        const auto DEFEND_SPD_RAND_MAX { misc::Max(DEFEND_SPD_RAW, defendSpdToUse) };
        const auto DEFEND_SPD_RAND { misc::Random(DEFEND_SPD_RAND_MIN, DEFEND_SPD_RAND_MAX) };

        const auto IS_DEFENSE_AMAZING_DODGE {
            (defendSpdToUse.Get() >= STAT_HIGHER_THAN_AVERAGE)
            && IsValuetHigherThanRatioOfStat(
                   DEFEND_SPD_RAND.Get(), defendSpdToUse.Get(), STAT_RATIO_AMAZING)
        };

        if (IS_ATTACK_AMAZING_ACC && (IS_DEFENSE_AMAZING_DODGE == false))
        {
            // This case allows any attacker the rare chance to hit, even when the odds are
            // overwhelmingly bad, such as when the attackers accuracy max is less than the
            // defender's speed min.
            wasHit = true;
            hasHitBeenDetermined = true;
        }
        else if ((IS_ATTACK_AMAZING_ACC == false) && IS_DEFENSE_AMAZING_DODGE)
        {
            // This case allows any defender the rare chance to dodge even if the odds are
            // overwhelmingly bad, such as when the defender's speed max is less then the
            // attacker's accuracy min.
            wasHit = false;
            hasHitBeenDetermined = true;
        }

        // In this case, either both the attacker and defending did their best and tied for
        //"rolling" their highest possible rand/score or the rand "rolled" scores were equal.
        // So the hit is determined by whose stat+rank is higher.
        // This is the case most likely to decide if hit or miss.
        if (false == hasHitBeenDetermined)
        {
            const auto ATTACK_ACC_RANK_ADJ { Rank_t::Make(
                ATTACK_ACC_RAND.Get() + CREATURE_ATTACKING_PTR->Rank().Get()) };

            const auto DEFEND_SPD_RANK_ADJ { Rank_t::Make(
                DEFEND_SPD_RAND.Get() + CREATURE_DEFENDING_PTR->Rank().Get()) };

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

        // In this case both the rand "rolls" and the stat+rank are equal,
        // so determine if the attack hit or miss based on luck.
        if (false == hasHitBeenDetermined)
        {
            const auto ATTACKER_LUCK_RAND { creature::Stats::Roll(
                CREATURE_ATTACKING_PTR, creature::Traits::Luck, creature::Stats::RaceRoleBonus) };

            const auto DEFENDER_LUCK_RAND { creature::Stats::Roll(
                CREATURE_DEFENDING_PTR, creature::Traits::Luck, creature::Stats::RaceRoleBonus) };

            if (ATTACKER_LUCK_RAND == DEFENDER_LUCK_RAND)
            {
                // In this case, attacker and defender tied on luck rolls,
                // so the hit is determined by who has the greater luck roll + rank.
                const auto ATTACK_LCK_RANK_ADJ { ATTACKER_LUCK_RAND
                                                 + CREATURE_ATTACKING_PTR->Rank().GetAs<int>() };

                const auto DEFEND_LCK_RANK_ADJ { DEFENDER_LUCK_RAND
                                                 + CREATURE_DEFENDING_PTR->Rank().GetAs<int>() };

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

        // In this case, everything that could be equal WAS equal, so let the player win.
        if (false == hasHitBeenDetermined)
        {
            // hasHitBeenDetermined = true;

            if (CREATURE_ATTACKING_PTR->IsPlayerCharacter()
                && CREATURE_DEFENDING_PTR->IsPlayerCharacter())
            {
                // In this case, both attacker and defender are players,
                // so let fair coin toss determine the hit.
                wasHit = misc::RandomBool();
            }
            else
            {
                wasHit = CREATURE_ATTACKING_PTR->IsPlayerCharacter();
            }
        }

        // handle forced hits
        if (WILL_FORCE_HIT && (false == wasHit))
        {
            wasHit = true;
            // hasHitBeenDetermined = true;
        }

        Health_t damage { 0_health };
        auto isPowerHit { false };
        auto isCriticalHit { false };
        auto didArmorAbsorb { false };
        creature::CondEnumVec_t condsAddedVec;
        creature::CondEnumVec_t condsRemovedVec;
        if (wasHit)
        {
            damage = DetermineDamage(
                WEAPON_PTR,
                CREATURE_ATTACKING_PTR,
                CREATURE_DEFENDING_PTR,
                isPowerHit,
                isCriticalHit,
                didArmorAbsorb);

            HandleDamage(
                CREATURE_DEFENDING_PTR, hitInfoVec, damage, condsAddedVec, condsRemovedVec);
        }

        combat::Text combatText;

        return HitInfo(
            wasHit,
            WEAPON_PTR,
            damage,
            isCriticalHit,
            isPowerHit,
            didArmorAbsorb,
            condsAddedVec,
            condsRemovedVec,
            combatText.WeaponActionVerb(WEAPON_PTR, false));
    }

    Health_t CreatureInteraction::DetermineDamage(
        const item::ItemPtr_t WEAPON_PTR,
        const creature::CreaturePtr_t CREATURE_ATTACKING_PTR,
        const creature::CreaturePtr_t CREATURE_DEFENDING_PTR,
        bool & isPowerHit_OutParam,
        bool & isCriticalHit_OutParam,
        bool & didArmorAbsorb_OutParam) const
    {
        const Health_t DAMAGE_FROM_WEAPON_RAW { misc::Random(
            WEAPON_PTR->DamageMin(), WEAPON_PTR->DamageMax()) };

        // If weapon is fist and creature attacking is wearing gauntlets, then triple the
        // damage.
        Health_t extraDamage { 0_health };
        if (WEAPON_PTR->WeaponInfo().IsFists())
        {
            for (const auto & NEXT_ITEM_PTR : CREATURE_ATTACKING_PTR->Inventory().ItemsEquipped())
            {
                if ((NEXT_ITEM_PTR->ArmorInfo().IsGauntlets())
                    && (NEXT_ITEM_PTR->ArmorInfo().BaseType() != item::armor::base_type::Plain))
                {
                    extraDamage = 2_health * DAMAGE_FROM_WEAPON_RAW;
                    break;
                }
            }
        }

        // If weapon is bite and creature has fangs, then triple the damage.
        if (WEAPON_PTR->WeaponInfo().IsBite() && (CREATURE_ATTACKING_PTR->Body().HasFangs()))
        {
            extraDamage = DAMAGE_FROM_WEAPON_RAW;
        }

        const auto DAMAGE_FROM_WEAPON { DAMAGE_FROM_WEAPON_RAW + extraDamage };

        // add extra damage based on rank
        const auto RANK_DAMAGE_BONUS_ADJ_RATIO {
            misc::ConfigFile::Instance()->ValueOrDefault<float>("fight-rank-damage-bonus-ratio")
        };

        const auto DAMAGE_FROM_RANK { Health_t::Make(
            CREATURE_ATTACKING_PTR->Rank().GetAs<float>() * RANK_DAMAGE_BONUS_ADJ_RATIO) };

        // If strength stat is at or over the min of STAT_FLOOR,
        // then add a damage bonus based on half a strength ratio "roll".
        const creature::Trait_t STAT_FLOOR { misc::ConfigFile::Instance()->ValueOrDefault<int>(
            "fight-stats-value-floor") };

        Health_t damageFromStrength { 0_health };
        const auto STRENGTH_CURRENT { CREATURE_ATTACKING_PTR->Strength().GetAs<int>() };
        if (STRENGTH_CURRENT > STAT_FLOOR)
        {
            const auto RAND_STR_STAT { creature::Stats::Roll(
                CREATURE_ATTACKING_PTR, creature::Traits::Strength) };

            const auto STR_BONUS_ADJ_RATIO { misc::ConfigFile::Instance()->ValueOrDefault<float>(
                "fight-damage-strength-bonus-ratio") };

            damageFromStrength
                += Health_t::Make(static_cast<float>(RAND_STR_STAT) * STR_BONUS_ADJ_RATIO);

            damageFromStrength -= Health_t::Make(STAT_FLOOR);

            if (damageFromStrength < 0_health)
            {
                damageFromStrength = 0_health;
            }
        }

        const Health_t DAMAGE_BASE { DAMAGE_FROM_WEAPON + DAMAGE_FROM_RANK + damageFromStrength };

        // there is a rare chance of a power hit for players
        const auto STRENGTH_TEST { creature::Stats::Test(
            CREATURE_ATTACKING_PTR,
            creature::Traits::Strength,
            static_cast<creature::Stats::With>(
                creature::Stats::With::Luck | creature::Stats::With::RaceRoleBonus
                | creature::Stats::With::RankBonus)) };

        const auto POWER_HIT_CHANCE_RATIO { misc::ConfigFile::Instance()->ValueOrDefault<float>(
            "fight-hit-power-chance-ratio") };

        isPowerHit_OutParam
            = (CREATURE_ATTACKING_PTR->IsPlayerCharacter() && STRENGTH_TEST
               && (STRENGTH_CURRENT >= STAT_FLOOR)
               && (misc::Random(1.0f) < POWER_HIT_CHANCE_RATIO));

        // there is a rare chance of a critical hit for players and non-players
        const auto ACCURACY_TEST { creature::Stats::Test(
            CREATURE_ATTACKING_PTR,
            creature::Traits::Accuracy,
            static_cast<creature::Stats::With>(
                creature::Stats::With::Luck | creature::Stats::With::RaceRoleBonus
                | creature::Stats::With::RankBonus)) };

        const auto LUCK_TEST { creature::Stats::Test(
            CREATURE_ATTACKING_PTR,
            creature::Traits::Luck,
            static_cast<creature::Stats::With>(
                creature::Stats::With::Luck | creature::Stats::With::RaceRoleBonus
                | creature::Stats::With::RankBonus)) };

        const auto CRITICAL_HIT_CHANCE_RATIO { misc::ConfigFile::Instance()->ValueOrDefault<float>(
            "fight-hit-critical-chance-ratio") };

        const auto ACCURACY_CURRENT { CREATURE_ATTACKING_PTR->Accuracy().GetAsTrait() };

        isCriticalHit_OutParam
            = ((CREATURE_ATTACKING_PTR->IsPlayerCharacter() || LUCK_TEST) && ACCURACY_TEST
               && ((ACCURACY_CURRENT >= STAT_FLOOR) || LUCK_TEST)
               && (misc::Random(1.0f) < CRITICAL_HIT_CHANCE_RATIO));

        Health_t damageFinal { DAMAGE_BASE };

        const auto SPECIAL_HIT_DAMAGE_MIN = Health_t::Make(
            CREATURE_ATTACKING_PTR->Rank().Get()
            + misc::ConfigFile::Instance()->ValueOrDefault<Health_t::value_type>(
                  "fight-hit-special-damage-min"));

        if (isPowerHit_OutParam)
        {
            damageFinal += misc::Max(
                damageFromStrength, (damageFinal / SPECIAL_HIT_DAMAGE_MIN), SPECIAL_HIT_DAMAGE_MIN);
        }

        if (isCriticalHit_OutParam)
        {
            damageFinal += misc::Max(DAMAGE_FROM_WEAPON, SPECIAL_HIT_DAMAGE_MIN);
        }

        const auto DAMAGE_AFTER_SPECIALS { damageFinal };

        // reduce damage based on the defending creature's armor
        auto armorRatingToUse { CREATURE_DEFENDING_PTR->ArmorRating() };
        if (CREATURE_DEFENDING_PTR->HasCondition(creature::Conditions::Tripped))
        {
            armorRatingToUse
                -= (armorRatings_.LesserSteel() + armorRatings_.GreaterSteel()) / 4_armor;

            if (armorRatingToUse < 0_armor)
            {
                armorRatingToUse = 0_armor;
            }
        }

        damageFinal -= Health_t::Make(
            damageFinal.GetAs<float>()
            * (armorRatingToUse.GetAs<float>() / armorRatings_.GreaterDiamond().GetAs<float>()));

        // check if armor absorbed all the damage
        if ((DAMAGE_AFTER_SPECIALS > 0_health) && (damageFinal <= 0_health))
        {
            didArmorAbsorb_OutParam = true;
        }

        // pixies are dealt less damage than other creatures because of
        // their small size and speed
        if ((damageFinal > 0_health) && (CREATURE_DEFENDING_PTR->Race() == creature::race::Pixie))
        {
            const Health_t PIXIE_DAMAGE_FLOOR
                = misc::ConfigFile::Instance()->ValueOrDefault<Health_t>(
                    "fight-pixie-damage-floor");

            if (damageFinal < PIXIE_DAMAGE_FLOOR)
            {
                damageFinal = 1_health;
            }
            else
            {
                const auto PIXIE_DAMAGE_ADJ_RATIO {
                    misc::ConfigFile::Instance()->ValueOrDefault<float>(
                        "fight-pixie-damage-adj-ratio")
                };

                damageFinal = Health_t::Make(damageFinal.GetAs<float>() * PIXIE_DAMAGE_ADJ_RATIO);
            }
        }

        // prevent negative damage
        if (damageFinal < 0_health)
        {
            damageFinal = 0_health;
        }

        // if power or critical hit, then assure there is some damage dealt
        if ((isPowerHit_OutParam || isCriticalHit_OutParam)
            && (damageFinal < SPECIAL_HIT_DAMAGE_MIN))
        {
            didArmorAbsorb_OutParam = false;

            if (isPowerHit_OutParam)
            {
                damageFinal += misc::Max(damageFromStrength, SPECIAL_HIT_DAMAGE_MIN);
            }

            if (isCriticalHit_OutParam)
            {
                damageFinal += misc::Max(DAMAGE_FROM_WEAPON, SPECIAL_HIT_DAMAGE_MIN);
            }
        }

        // update TurnInfo
        auto turnInfo { Encounter::Instance()->GetTurnInfoCopy(CREATURE_DEFENDING_PTR) };

        if (!turnInfo.GetFirstAttackedByCreaturePtrOpt())
        {
            turnInfo.SetFirstAttackedByCreature(CREATURE_ATTACKING_PTR);
        }

        if (!turnInfo.GetFirstHitByCreaturePtrOpt())
        {
            turnInfo.SetFirstHitByCreature(CREATURE_ATTACKING_PTR);
        }

        turnInfo.SetLastAttackedByCreature(CREATURE_ATTACKING_PTR);
        turnInfo.SetLastHitByCreature(CREATURE_ATTACKING_PTR);
        turnInfo.UpdateMostDamagedCreature(damageFinal.MakePositiveCopy(), CREATURE_ATTACKING_PTR);
        turnInfo.SetWasHitLastTurn(true);

        Encounter::Instance()->SetTurnInfo(CREATURE_DEFENDING_PTR, turnInfo);

        return -damageFinal;
    }

    bool CreatureInteraction::AreAnyOfCondsContained(
        const creature::CondEnumVec_t & CONDS_VEC,
        const creature::CreaturePtr_t CREATURE_PTR,
        const HitInfoVec_t & HIT_INFO_VEC) const
    {
        for (const auto NEXT_COND_ENUM : CONDS_VEC)
        {
            if (CREATURE_PTR->HasCondition(NEXT_COND_ENUM))
            {
                return true;
            }

            for (const auto & NEXT_HIT_INFO : HIT_INFO_VEC)
            {
                if (NEXT_HIT_INFO.CondsAddedContains(NEXT_COND_ENUM))
                {
                    return true;
                }
            }
        }

        return false;
    }

    creature::Trait_t CreatureInteraction::IsValuetHigherThanRatioOfStat(
        const creature::Trait_t STAT_VALUE,
        const creature::Trait_t STAT_MAX,
        const float RATIO) const
    {
        return (STAT_VALUE >= static_cast<creature::Trait_t>(static_cast<float>(STAT_MAX) * RATIO));
    }

} // namespace combat
} // namespace heroespath
