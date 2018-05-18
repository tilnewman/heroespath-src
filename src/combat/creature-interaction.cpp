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
#include "game/game-data-file.hpp"
#include "game/game.hpp"
#include "item/item.hpp"
#include "log/log-macros.hpp"
#include "misc/random.hpp"
#include "misc/vectors.hpp"
#include "song/song.hpp"
#include "spell/spell.hpp"
#include "state/game-state.hpp"

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
        auto const HIT_INFO_VEC{ AttackWithAllWeapons(
            CREATURE_ATTACKING_PTR, CREATURE_DEFENDING_PTR, WILL_FORCE_HIT) };

        auto const CREATURE_EFFECT{ CreatureEffect(CREATURE_DEFENDING_PTR, HIT_INFO_VEC) };

        auto turnInfo{ Encounter::Instance()->GetTurnInfoCopy(CREATURE_DEFENDING_PTR) };

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
        using namespace creature;

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
                    { Conditions::Dazed, Conditions::Daunted },
                    CREATURE_DEFENDING_PTR,
                    hitInfoVec,
                    condsRemovedVec);

                if (AreAnyOfCondsContained(
                        { Conditions::Panic }, CREATURE_DEFENDING_PTR, hitInfoVec))
                {
                    RemoveAddedCondition(
                        Conditions::Panic, CREATURE_DEFENDING_PTR, hitInfoVec, condsRemovedVec);

                    CREATURE_DEFENDING_PTR->ConditionAdd(Conditions::Daunted);
                    condsAddedVec.emplace_back(Conditions::Daunted);
                }
            }

            RemoveAddedCondition(
                Conditions::Unconscious, CREATURE_DEFENDING_PTR, hitInfoVec, condsRemovedVec);

            return;
        }

        // check if already dead
        if (AreAnyOfCondsContained({ Conditions::Dead }, CREATURE_DEFENDING_PTR, hitInfoVec))
        {
            return;
        }

        auto const IS_ALREADY_UNCONSCIOUS{ AreAnyOfCondsContained(
            { Conditions::Unconscious }, CREATURE_DEFENDING_PTR, hitInfoVec) };

        // at this point HEALTH_ADJ is negative
        auto const DAMAGE_ABS{ HEALTH_ADJ.Abs() };

        // logic needed to determine if damage will kill
        auto const IS_NONPLAYER_CHARACTER{ CREATURE_DEFENDING_PTR->IsPlayerCharacter() == false };

        auto const IS_DAMAGE_DOUBLE{ (
            DAMAGE_ABS > Health_t::Make(CREATURE_DEFENDING_PTR->HealthNormal() * 2_health)) };

        auto const WILL_DAMAGE_KILL{ (DAMAGE_ABS >= CREATURE_DEFENDING_PTR->HealthCurrent()) };

        auto const NOT_PIXIE{ (CREATURE_DEFENDING_PTR->IsPixie() == false) };

        // check if the damage will kill
        if (IS_ALREADY_UNCONSCIOUS
            || (IS_NONPLAYER_CHARACTER && ((IS_DAMAGE_DOUBLE && NOT_PIXIE) || WILL_DAMAGE_KILL)))
        {
            CREATURE_DEFENDING_PTR->HealthCurrentSet(0_health);
            CREATURE_DEFENDING_PTR->ConditionAdd(Conditions::Dead);
            condsAddedVec.emplace_back(Conditions::Dead);

            auto turnInfo{ combat::Encounter::Instance()->GetTurnInfoCopy(CREATURE_DEFENDING_PTR) };
            turnInfo.SetIsFlying(false);
            turnInfo.SetWasHitLastTurn(true);
            turnInfo.SetTurnActionInfo(TurnActionInfo());
            combat::Encounter::Instance()->SetTurnInfo(CREATURE_DEFENDING_PTR, turnInfo);

            // remove the unconscious condition if already there
            if (IS_ALREADY_UNCONSCIOUS)
            {
                RemoveAddedCondition(
                    Conditions::Unconscious, CREATURE_DEFENDING_PTR, hitInfoVec, condsRemovedVec);
            }

            return;
        }

        if (DAMAGE_ABS > 0_health)
        {
            // wake from natural sleep if hit
            RemoveAddedCondition(
                Conditions::AsleepNatural, CREATURE_DEFENDING_PTR, hitInfoVec, condsRemovedVec);
        }

        CREATURE_DEFENDING_PTR->HealthCurrentAdj(DAMAGE_ABS * Health_t(-1));
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

                CREATURE_DEFENDING_PTR->ConditionAdd(Conditions::Unconscious);
                condsAddedVec.emplace_back(Conditions::Unconscious);

                RemoveAddedConditions(
                    { Conditions::Dazed,
                      Conditions::Daunted,
                      Conditions::AsleepNatural,
                      Conditions::AsleepMagical,
                      Conditions::Bold,
                      Conditions::Heroic,
                      Conditions::Panic,
                      Conditions::Tripped },
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
        auto condsPVec{ CREATURE_PTR->ConditionsPVec() };

        std::sort(condsPVec.begin(), condsPVec.end(), [](auto const A, auto const B) {
            return (A->Severity() > B->Severity());
        });

        auto hasTurnBeenConsumed{ false };
        for (auto const & NEXT_COND_PTR : condsPVec)
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
        using namespace creature;

        creature::CondEnumVec_t condsVecToAdd{
            Conditions::Daunted, Conditions::Dazed, Conditions::Tripped
        };

        misc::Vector::ShuffleVec(condsVecToAdd);

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

        auto const CHANCE_CONDS_ADDED_RATIO{ game::GameDataFile::Instance()->GetCopyFloat(
            "heroespath-fight-chance-conditions-added-from-damage-ratio") };

        for (auto const NEXT_COND_ENUM : condsVecToAdd)
        {
            // only a 50/50 chance of adding conditions
            if (misc::random::Float(1.0f) < CHANCE_CONDS_ADDED_RATIO)
            {
                if ((NEXT_COND_ENUM == Conditions::Dazed)
                    && AreAnyOfCondsContained(
                           { Conditions::AsleepMagical,
                             Conditions::AsleepNatural,
                             Conditions::Dazed,
                             Conditions::Unconscious },
                           CREATURE_DEFENDING_PTR,
                           hitInfoVec))
                {
                    continue;
                }

                if ((NEXT_COND_ENUM == Conditions::Daunted)
                    && AreAnyOfCondsContained(
                           { Conditions::Daunted, Conditions::Unconscious },
                           CREATURE_DEFENDING_PTR,
                           hitInfoVec))
                {
                    continue;
                }

                if ((NEXT_COND_ENUM == Conditions::Tripped)
                    && AreAnyOfCondsContained(
                           { Conditions::Tripped,
                             Conditions::AsleepMagical,
                             Conditions::AsleepNatural,
                             Conditions::Unconscious,
                             Conditions::Pounced },
                           CREATURE_DEFENDING_PTR,
                           hitInfoVec))
                {
                    continue;
                }

                if ((NEXT_COND_ENUM == Conditions::Daunted)
                    && AreAnyOfCondsContained(
                           { Conditions::Heroic, Conditions::Bold },
                           CREATURE_DEFENDING_PTR,
                           hitInfoVec))
                {
                    RemoveAddedConditions(
                        { Conditions::Heroic, Conditions::Bold },
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
        auto wasAnyRemoved{ false };

        for (auto const NEXT_COND_ENUM : CONDS_VEC)
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
        M_ASSERT_OR_LOGANDTHROW_SS(
            (creaturesCastUponPVec.empty() == false),
            "combat::CreatureInteraction::Cast(spell="
                << SPELL_PTR->Name()
                << ", creature_casting=" << CREATURE_CASTING_PTR->NameAndRaceAndRole()
                << ", creatures_cast_upon=empty) was given an empty creaturesCastUponPVec.");

        CREATURE_CASTING_PTR->ManaAdj(SPELL_PTR->ManaCost() * Mana_t(-1));

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
                         misc::Vector::JoinOpt::None,
                         creature::Algorithms::NamesOpt::WithRaceAndRole)
                  << "\") spell target_type=" << TargetType::ToString(SPELL_PTR->Target())
                  << " but there were " << creaturesCastUponPVec.size()
                  << " creatures being cast upon.  There should have been only 1.";

            throw std::runtime_error(ssErr.str());
        }

        // update caster's TurnInfo
        auto casterTurnInfo{ Encounter::Instance()->GetTurnInfoCopy(CREATURE_CASTING_PTR) };
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
                Health_t healthAdj{ 0_health };
                creature::CondEnumVec_t condsAddedVec;
                creature::CondEnumVec_t condsRemovedVec;

                auto const DID_SPELL_SUCCEED{ SPELL_PTR->EffectCreature(
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
                auto turnInfo{ Encounter::Instance()->GetTurnInfoCopy(nextCreatureCastUponPtr) };

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
                        turnInfo.UpdateMostDamagedCreature(healthAdj.Abs(), CREATURE_CASTING_PTR);
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
        M_ASSERT_OR_LOGANDTHROW_SS(
            (CREATURES_LISTENING_PVEC.empty() == false),
            "combat::CreatureInteraction::PlaySong(song="
                << SONG_PTR->Name()
                << ", creature_playing=" << CREATURE_PLAYING_PTR->NameAndRaceAndRole()
                << ", creatures_listening=empty) was given an empty creaturesListeningPVec.");

        CREATURE_PLAYING_PTR->ManaAdj(SONG_PTR->ManaCost() * Mana_t(-1));

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
                         misc::Vector::JoinOpt::None,
                         creature::Algorithms::NamesOpt::WithRaceAndRole)
                  << "\") song target_type=" << TargetType::ToString(SONG_PTR->Target())
                  << " but there were " << CREATURES_LISTENING_PVEC.size()
                  << " creatures listening.  There should have been only 1.";

            throw std::runtime_error(ssErr.str());
        }

        // update player's TurnInfo
        auto playerTurnInfo{ Encounter::Instance()->GetTurnInfoCopy(CREATURE_PLAYING_PTR) };
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
                Health_t healthAdj{ 0_health };
                creature::CondEnumVec_t condsAddedVec;
                creature::CondEnumVec_t condsRemovedVec;

                auto const DID_SONG_SUCCEED{ SONG_PTR->EffectCreature(
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
                auto turnInfo{ Encounter::Instance()->GetTurnInfoCopy(nextCreatureCastUponPtr) };

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
                        turnInfo.UpdateMostDamagedCreature(healthAdj.Abs(), CREATURE_PLAYING_PTR);
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
        using namespace creature;

        // update player's TurnActionInfo
        Encounter::Instance()->SetTurnActionInfo(
            CREATURE_POUNCING_PTR,
            TurnActionInfo(
                ((Encounter::Instance()->GetTurnInfoCopy(CREATURE_POUNCING_PTR).GetIsFlying())
                     ? TurnAction::SkyPounce
                     : TurnAction::LandPounce),
                { CREATURE_DEFENDING_PTR }));

        HitInfo hitInfo;

        auto const DID_ROLL_SUCCEED{ creature::Stats::Versus(
            CREATURE_POUNCING_PTR,
            { stats::Traits::Speed, stats::Traits::Accuracy },
            CREATURE_DEFENDING_PTR,
            { stats::Traits::Speed },
            0,
            0,
            static_cast<creature::Stats::With>(
                creature::Stats::With::Luck | creature::Stats::With::RaceRoleBonus
                | creature::Stats::With::RankBonus | creature::Stats::With::PlayerNaturalWins)) };

        auto const DID_SUCCEED{ (
            DID_ROLL_SUCCEED || CREATURE_DEFENDING_PTR->HasCondition(Conditions::Tripped)
            || CREATURE_DEFENDING_PTR->HasCondition(Conditions::AsleepNatural)
            || CREATURE_DEFENDING_PTR->HasCondition(Conditions::AsleepMagical)
            || CREATURE_DEFENDING_PTR->HasCondition(Conditions::Unconscious)) };

        Health_t healthAdj{ 0_health };

        if (DID_SUCCEED)
        {
            // TODO should pouncing do some damage?
            healthAdj = 0_health;

            HitInfoVec_t hitInfoVec;
            creature::CondEnumVec_t condsAddedVec;
            creature::CondEnumVec_t condsRemovedVec;

            if (CREATURE_DEFENDING_PTR->HasCondition(Conditions::Tripped))
            {
                CREATURE_DEFENDING_PTR->ConditionRemove(Conditions::Tripped);
                condsRemovedVec.emplace_back(Conditions::Tripped);
            }

            HandleDamage(
                CREATURE_DEFENDING_PTR, hitInfoVec, healthAdj, condsAddedVec, condsRemovedVec);

            CREATURE_DEFENDING_PTR->ConditionAdd(Conditions::Pounced);

            const ContentAndNamePos CNP("", " pounces on ", ".", NamePosition::SourceThenTarget);

            hitInfo = HitInfo(
                true,
                HitType::Pounce,
                CNP,
                healthAdj,
                { Conditions::Pounced },
                creature::CondEnumVec_t());
        }
        else
        {
            const ContentAndNamePos CNP(
                "", " tried to pounce on ", "but missed.", NamePosition::SourceThenTarget);

            hitInfo = HitInfo(false, HitType::Pounce, CNP);
        }

        // update defender's TurnInfo
        auto turnInfo{ Encounter::Instance()->GetTurnInfoCopy(CREATURE_DEFENDING_PTR) };

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
            turnInfo.UpdateMostDamagedCreature(healthAdj.Abs(), CREATURE_POUNCING_PTR);
            turnInfo.SetWasHitLastTurn(true);
        }

        Encounter::Instance()->SetTurnInfo(CREATURE_DEFENDING_PTR, turnInfo);

        return FightResult(CreatureEffect(CREATURE_DEFENDING_PTR, HitInfoVec_t(1, hitInfo)));
    }

    const FightResult CreatureInteraction::Roar(
        const creature::CreaturePtr_t CREATURE_ROARING_PTR,
        const CombatDisplayPtr_t COMBAT_DISPLAY_PTR) const
    {
        using namespace creature;

        auto const LISTENING_CREATURES_PVEC{ COMBAT_DISPLAY_PTR->GetCreaturesInRoaringDistance(
            CREATURE_ROARING_PTR) };

        // update player's TurnActionInfo
        Encounter::Instance()->SetTurnActionInfo(
            CREATURE_ROARING_PTR, TurnActionInfo(TurnAction::Roar, LISTENING_CREATURES_PVEC));

        CreatureEffectVec_t creatureEffectsVec;

        auto const IS_ROARING_CREATURE_FLYING{
            combat::Encounter::Instance()->GetTurnInfoCopy(CREATURE_ROARING_PTR).GetIsFlying()
        };

        // Give each defending creature a chance to resist being panicked.
        // The farther away each defending creature is the better chance
        // of resisting he/she/it has.
        for (auto const & NEXT_DEFEND_CREATURE_PTR : LISTENING_CREATURES_PVEC)
        {
            if (NEXT_DEFEND_CREATURE_PTR->HasCondition(Conditions::Panic))
            {
                const ContentAndNamePos CNP(" is already panicked.", NamePosition::TargetBefore);

                const HitInfo HIT_INFO(false, HitType::Roar, CNP);

                creatureEffectsVec.emplace_back(
                    CreatureEffect(NEXT_DEFEND_CREATURE_PTR, HitInfoVec_t(1, HIT_INFO)));

                continue;
            }

            auto nextBlockingDisatnce{ std::abs(COMBAT_DISPLAY_PTR->GetBlockingDistanceBetween(
                CREATURE_ROARING_PTR, NEXT_DEFEND_CREATURE_PTR)) };

            // if flying, then consider it farther away and less likely to be panicked
            if (Encounter::Instance()->GetTurnInfoCopy(NEXT_DEFEND_CREATURE_PTR).GetIsFlying()
                && (IS_ROARING_CREATURE_FLYING == false))
            {
                ++nextBlockingDisatnce;
            }

            const stats::Trait_t DISATANCE_BONUS{ nextBlockingDisatnce * 2 };

            if (creature::Stats::Versus(
                    CREATURE_ROARING_PTR,
                    stats::Traits::Strength,
                    NEXT_DEFEND_CREATURE_PTR,
                    stats::Traits::Intelligence,
                    0,
                    DISATANCE_BONUS,
                    static_cast<creature::Stats::With>(
                        creature::Stats::With::Luck | creature::Stats::With::RaceRoleBonus
                        | creature::Stats::With::RankBonus
                        | creature::Stats::With::PlayerNaturalWins)))
            {
                // no TurnInfo settings need to be made for roar

                // remove the Daunted condition before adding the Panicked condition
                NEXT_DEFEND_CREATURE_PTR->ConditionRemove(Conditions::Daunted);

                NEXT_DEFEND_CREATURE_PTR->ConditionAdd(Conditions::Panic);

                const ContentAndNamePos CNP("'s roar panics ", NamePosition::SourceThenTarget);

                const HitInfo HIT_INFO(
                    true,
                    HitType::Roar,
                    CNP,
                    0_health,
                    creature::CondEnumVec_t(1, Conditions::Panic),
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
        auto const ATTACKABLE_NONPLAYER_CREATURES_PVEC{
            COMBAT_DISPLAY_PTR->FindCreaturesThatCanBeAttackedOfType(CREATURE_ATTACKING_PTR, false)
        };

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ATTACKABLE_NONPLAYER_CREATURES_PVEC.empty() == false),
            "combat::CreatureInteraction::HandleAttack() FindNonPlayerCreatureToAttack() "
                << "returned no attackable creatures.");

        // attack those with the lowest relative health first, which will correspond
        // to the health bar seen on screen
        auto const LIVE_ATTBLE_LOWH_NP_CRTS_PVEC{ creature::Algorithms::FindLowestHealthRatio(
            ATTACKABLE_NONPLAYER_CREATURES_PVEC) };

        M_ASSERT_OR_LOGANDTHROW_SS(
            (LIVE_ATTBLE_LOWH_NP_CRTS_PVEC.empty() == false),
            "combat::CreatureInteraction::HandleAttack() FindNonPlayerCreatureToAttack() returned "
                << "no LIVING LOWEST HEALTH RATIO attackable creatures.");

        // skip creatures who are not a threat
        auto const LIVE_ATTBLE_LOWH_NP_NOTPERMDIS_CRTS_PVEC{
            creature::Algorithms::FindByConditionMeaningNotAThreatPermenantly(
                LIVE_ATTBLE_LOWH_NP_CRTS_PVEC,
                creature::Algorithms::CondSingleOpt::DoesNotHave,
                creature::UnconOpt::Include)
        };

        // attack those that are temporarily disabled first
        auto const LIVE_ATTBLE_LOWH_NP_NOTPERMDIS_TMPDIS_CRTS_PVEC{
            creature::Algorithms::FindByConditionMeaningNotAThreatTemporarily(
                LIVE_ATTBLE_LOWH_NP_NOTPERMDIS_CRTS_PVEC)
        };

        if (LIVE_ATTBLE_LOWH_NP_NOTPERMDIS_TMPDIS_CRTS_PVEC.empty())
        {
            if (LIVE_ATTBLE_LOWH_NP_NOTPERMDIS_CRTS_PVEC.empty())
            {
                return misc::Vector::SelectRandom(COMBAT_DISPLAY_PTR->FindClosestAmongOfType(
                    CREATURE_ATTACKING_PTR, LIVE_ATTBLE_LOWH_NP_CRTS_PVEC, false));
            }
            else
            {
                return misc::Vector::SelectRandom(COMBAT_DISPLAY_PTR->FindClosestAmongOfType(
                    CREATURE_ATTACKING_PTR, LIVE_ATTBLE_LOWH_NP_NOTPERMDIS_CRTS_PVEC, false));
            }
        }
        else
        {
            return misc::Vector::SelectRandom(COMBAT_DISPLAY_PTR->FindClosestAmongOfType(
                CREATURE_ATTACKING_PTR, LIVE_ATTBLE_LOWH_NP_NOTPERMDIS_TMPDIS_CRTS_PVEC, false));
        }
    }

    const FightResult CreatureInteraction::TreasureTrap(
        const Trap & TRAP, const creature::CreaturePtr_t CREATURE_PICKING_THE_LOCK_PTR) const
    {
        auto const HURT_CREATURE_PTRS(
            RandomSelectWhoIsHurtByTrap(TRAP, CREATURE_PICKING_THE_LOCK_PTR));

        CreatureEffectVec_t creatureEffectVec;
        for (auto & nextCreatureHurtPtr : HURT_CREATURE_PTRS)
        {
            auto const HEALTH_ADJ{ TRAP.RandomDamage() * Health_t(-1) };

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
        creature::CreaturePVec_t creaturesHurtPtrs{ CREATURE_PICKING_THE_LOCK_PTR };

        auto allCharacterPtrs{ game::Game::Instance()->State().Party().Characters() };
        misc::Vector::ShuffleVec(allCharacterPtrs);

        auto const NUM_CHARACTERS_TO_ADD{ TRAP.RandomEffectedPlayersCount() - 1 };
        std::size_t numCharactersAdded{ 0 };

        for (std::size_t i(0);
             (i < allCharacterPtrs.size()) && (numCharactersAdded < NUM_CHARACTERS_TO_ADD);
             ++i)
        {
            auto const NEXT_CHARACTER_PTR{ allCharacterPtrs[i] };

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

        for (auto const & NEXT_ITEM_PTR : CREATURE_ATTACKING_PTR->HeldWeapons())
        {
            if (NEXT_ITEM_PTR->IsWeapon())
            {
                auto const NEXT_HIT_INFO{ AttackWithSingleWeapon(
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
        auto hasHitBeenDetermined{ false };
        auto wasHit{ false };

        auto const ATTACK_ACC_RAW{ CREATURE_ATTACKING_PTR->Accuracy().As<int>() };
        auto attackAccToUse{ ATTACK_ACC_RAW };

        // If the attacking creature is an archer who is using a projectile weapon,
        // then add an accuracy bonus.
        if ((CREATURE_ATTACKING_PTR->Role() == creature::role::Archer)
            && (WEAPON_PTR->WeaponType() & item::weapon_type::Projectile))
        {
            auto const ARCHER_ACC_BONUS_RATIO{ game::GameDataFile::Instance()->GetCopyFloat(
                "heroespath-fight-archer-projectile-accuracy-bonus-ratio") };

            attackAccToUse += static_cast<stats::Trait_t>(
                static_cast<float>(ATTACK_ACC_RAW) * ARCHER_ACC_BONUS_RATIO);

            auto const ARCHER_RANK_BONUS_RATIO{ game::GameDataFile::Instance()->GetCopyFloat(
                "heroespath-fight-archer-projectile-rank-bonus-ratio") };

            attackAccToUse += static_cast<int>(
                CREATURE_ATTACKING_PTR->Rank().As<float>() * ARCHER_RANK_BONUS_RATIO);
        }

        auto const ATTACK_ACC_RAND_MIN{ static_cast<stats::Trait_t>(
            static_cast<float>(attackAccToUse) * 0.4f) };

        auto const ATTACK_ACC_RAND_MAX{ std::max(ATTACK_ACC_RAW, attackAccToUse) };
        auto const ATTACK_ACC_RAND{ misc::random::Int(ATTACK_ACC_RAND_MIN, ATTACK_ACC_RAND_MAX) };

        auto const STAT_RATIO_AMAZING{ game::GameDataFile::Instance()->GetCopyFloat(
            "heroespath-fight-stats-amazing-ratio") };

        auto const STAT_HIGHER_THAN_AVERAGE{ game::GameDataFile::Instance()->GetCopyInt(
            "heroespath-fight-stats-base-high-val") };

        auto const IS_ATTACK_AMAZING_ACC{
            (attackAccToUse >= STAT_HIGHER_THAN_AVERAGE)
            && IsValuetHigherThanRatioOfStat(ATTACK_ACC_RAND, attackAccToUse, STAT_RATIO_AMAZING)
        };

        auto const DEFEND_SPD_RAW{ CREATURE_DEFENDING_PTR->Speed().As<int>() };
        auto defendSpdToUse{ DEFEND_SPD_RAW };

        // If the defending creature is a Pixie then add a speed bonus based on rank.
        if (CREATURE_DEFENDING_PTR->IsPixie())
        {
            defendSpdToUse += static_cast<int>(
                CREATURE_DEFENDING_PTR->Rank().As<float>()
                * game::GameDataFile::Instance()->GetCopyFloat(
                      "heroespath-fight-pixie-defend-speed-rank-bonus-ratio"));
        }

        // If the defending creature is Blocking, then add a speed bonus.
        if (combat::Encounter::Instance()
                ->GetTurnInfoCopy(CREATURE_DEFENDING_PTR)
                .GetTurnActionInfo()
                .Action()
            == TurnAction::Block)
        {
            defendSpdToUse += static_cast<int>(
                static_cast<float>(DEFEND_SPD_RAW)
                * game::GameDataFile::Instance()->GetCopyFloat(
                      "heroespath-fight-block-defend-speed-bonus-ratio"));
        }

        auto const DEFEND_SPD_RAND_MIN{ static_cast<stats::Trait_t>(
            static_cast<float>(defendSpdToUse) * 0.4f) };

        auto const DEFEND_SPD_RAND_MAX{ std::max(DEFEND_SPD_RAW, defendSpdToUse) };
        auto const DEFEND_SPD_RAND{ misc::random::Int(DEFEND_SPD_RAND_MIN, DEFEND_SPD_RAND_MAX) };

        auto const IS_DEFENSE_AMAZING_DODGE{
            (defendSpdToUse >= STAT_HIGHER_THAN_AVERAGE)
            && IsValuetHigherThanRatioOfStat(DEFEND_SPD_RAND, defendSpdToUse, STAT_RATIO_AMAZING)
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
            auto const ATTACK_ACC_RANK_ADJ{ ATTACK_ACC_RAND
                                            + CREATURE_ATTACKING_PTR->Rank().As<int>() };

            auto const DEFEND_SPD_RANK_ADJ{ DEFEND_SPD_RAND
                                            + CREATURE_DEFENDING_PTR->Rank().As<int>() };

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
            auto const ATTACKER_LUCK_RAND{ creature::Stats::Roll(
                CREATURE_ATTACKING_PTR, stats::Traits::Luck, creature::Stats::RaceRoleBonus) };

            auto const DEFENDER_LUCK_RAND{ creature::Stats::Roll(
                CREATURE_DEFENDING_PTR, stats::Traits::Luck, creature::Stats::RaceRoleBonus) };

            if (ATTACKER_LUCK_RAND == DEFENDER_LUCK_RAND)
            {
                // In this case, attaker and defender tied on luck rolls,
                // so the hit is determined by who has the greater luck roll + rank.
                auto const ATTACK_LCK_RANK_ADJ{ ATTACKER_LUCK_RAND
                                                + CREATURE_ATTACKING_PTR->Rank().As<int>() };

                auto const DEFEND_LCK_RANK_ADJ{ DEFENDER_LUCK_RAND
                                                + CREATURE_DEFENDING_PTR->Rank().As<int>() };

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
                wasHit = misc::random::Bool();
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

        Health_t damage{ 0_health };
        auto isPowerHit{ false };
        auto isCriticalHit{ false };
        auto didArmorAbsorb{ false };
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
        const Health_t DAMAGE_FROM_WEAPON_RAW{ misc::random::Int(
            WEAPON_PTR->DamageMin().As<int>(), WEAPON_PTR->DamageMax().As<int>()) };

        // If weapon is fist and creature attacking is wearing gauntlets, then triple the damage.
        Health_t extraDamage{ 0_health };
        if (WEAPON_PTR->WeaponInfo().IsFists())
        {
            for (auto const & NEXT_ITEM_PTR : CREATURE_ATTACKING_PTR->Inventory().ItemsEquipped())
            {
                if ((NEXT_ITEM_PTR->ArmorInfo().IsGauntlets())
                    && (NEXT_ITEM_PTR->ArmorInfo().BaseType() != item::armor::base_type::Plain))
                {
                    extraDamage = 2_health * DAMAGE_FROM_WEAPON_RAW;
                    break;
                }
            }
        }

        // If weapon is bite and creature has fangs, then tripple the damage.
        if (WEAPON_PTR->WeaponInfo().IsBite() && (CREATURE_ATTACKING_PTR->Body().HasFangs()))
        {
            extraDamage = DAMAGE_FROM_WEAPON_RAW;
        }

        auto const DAMAGE_FROM_WEAPON{ DAMAGE_FROM_WEAPON_RAW + extraDamage };

        // add extra damage based on rank
        auto const RANK_DAMAGE_BONUS_ADJ_RATIO{ game::GameDataFile::Instance()->GetCopyFloat(
            "heroespath-fight-rank-damage-bonus-ratio") };

        auto const DAMAGE_FROM_RANK{ Health_t::Make(
            CREATURE_ATTACKING_PTR->Rank().As<float>() * RANK_DAMAGE_BONUS_ADJ_RATIO) };

        // If strength stat is at or over the min of STAT_FLOOR,
        // then add a damage bonus based on half a strength ratio "roll".
        const stats::Trait_t STAT_FLOOR{ game::GameDataFile::Instance()->GetCopyInt(
            "heroespath-fight-stats-value-floor") };

        Health_t damageFromStrength{ 0_health };
        auto const STRENGTH_CURRENT{ CREATURE_ATTACKING_PTR->Strength().As<int>() };
        if (STRENGTH_CURRENT > STAT_FLOOR)
        {
            auto const RAND_STR_STAT{ creature::Stats::Roll(
                CREATURE_ATTACKING_PTR, stats::Traits::Strength) };

            auto const STR_BONUS_ADJ_RATIO{ game::GameDataFile::Instance()->GetCopyFloat(
                "heroespath-fight-damage-strength-bonus-ratio") };

            damageFromStrength
                += Health_t::Make(static_cast<float>(RAND_STR_STAT) * STR_BONUS_ADJ_RATIO);

            damageFromStrength -= Health_t(STAT_FLOOR);

            if (damageFromStrength < 0_health)
            {
                damageFromStrength = 0_health;
            }
        }

        const Health_t DAMAGE_BASE{ DAMAGE_FROM_WEAPON + DAMAGE_FROM_RANK + damageFromStrength };

        // there is a rare chance of a power hit for players
        auto const STRENGTH_TEST{ creature::Stats::Test(
            CREATURE_ATTACKING_PTR,
            stats::Traits::Strength,
            static_cast<creature::Stats::With>(
                creature::Stats::With::Luck | creature::Stats::With::RaceRoleBonus
                | creature::Stats::With::RankBonus)) };

        auto const POWER_HIT_CHANCE_RATIO{ game::GameDataFile::Instance()->GetCopyFloat(
            "heroespath-fight-hit-power-chance-ratio") };

        isPowerHit_OutParam
            = (CREATURE_ATTACKING_PTR->IsPlayerCharacter() && STRENGTH_TEST
               && (STRENGTH_CURRENT >= STAT_FLOOR)
               && (misc::random::Float(1.0f) < POWER_HIT_CHANCE_RATIO));

        // there is a rare chance of a critical hit for players and non-players
        auto const ACCURACY_TEST{ creature::Stats::Test(
            CREATURE_ATTACKING_PTR,
            stats::Traits::Accuracy,
            static_cast<creature::Stats::With>(
                creature::Stats::With::Luck | creature::Stats::With::RaceRoleBonus
                | creature::Stats::With::RankBonus)) };

        auto const LUCK_TEST{ creature::Stats::Test(
            CREATURE_ATTACKING_PTR,
            stats::Traits::Luck,
            static_cast<creature::Stats::With>(
                creature::Stats::With::Luck | creature::Stats::With::RaceRoleBonus
                | creature::Stats::With::RankBonus)) };

        auto const CRITICAL_HIT_CHANCE_RATIO{ game::GameDataFile::Instance()->GetCopyFloat(
            "heroespath-fight-hit-critical-chance-ratio") };

        auto const ACCURACY_CURRENT{ CREATURE_ATTACKING_PTR->Accuracy().As<int>() };

        isCriticalHit_OutParam
            = ((CREATURE_ATTACKING_PTR->IsPlayerCharacter() || LUCK_TEST) && ACCURACY_TEST
               && ((ACCURACY_CURRENT >= STAT_FLOOR) || LUCK_TEST)
               && (misc::random::Float(1.0f) < CRITICAL_HIT_CHANCE_RATIO));

        Health_t damageFinal{ DAMAGE_BASE };

        const Health_t SPECIAL_HIT_DAMAGE_MIN{ CREATURE_ATTACKING_PTR->Rank().As<int>()
                                               + game::GameDataFile::Instance()->GetCopyInt(
                                                     "heroespath-fight-hit-special-damage-min") };

        if (isPowerHit_OutParam)
        {
            damageFinal += std::max(
                std::max(damageFromStrength, (damageFinal / SPECIAL_HIT_DAMAGE_MIN)),
                SPECIAL_HIT_DAMAGE_MIN);
        }

        if (isCriticalHit_OutParam)
        {
            damageFinal += std::max(DAMAGE_FROM_WEAPON, SPECIAL_HIT_DAMAGE_MIN);
        }

        auto const DAMAGE_AFTER_SPECIALS{ damageFinal };

        // reduce damage based on the defending creature's armor
        auto armorRatingToUse{ CREATURE_DEFENDING_PTR->ArmorRating() };
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
            damageFinal.As<float>()
            * (armorRatingToUse.As<float>() / armorRatings_.GreaterDiamond().As<float>()));

        // check if armor absorbed all the damage
        if ((DAMAGE_AFTER_SPECIALS > 0_health) && (damageFinal <= 0_health))
        {
            didArmorAbsorb_OutParam = true;
        }

        // pixies are dealt less damage than other creatures because of
        // their small size and speed
        if ((damageFinal > 0_health) && (CREATURE_DEFENDING_PTR->Race() == creature::race::Pixie))
        {
            const Health_t PIXIE_DAMAGE_FLOOR{ game::GameDataFile::Instance()->GetCopyInt(
                "heroespath-fight-pixie-damage-floor") };

            if (damageFinal < PIXIE_DAMAGE_FLOOR)
            {
                damageFinal = 1_health;
            }
            else
            {
                auto const PIXIE_DAMAGE_ADJ_RATIO{ game::GameDataFile::Instance()->GetCopyFloat(
                    "heroespath-fight-pixie-damage-adj-ratio") };

                damageFinal = Health_t::Make(damageFinal.As<float>() * PIXIE_DAMAGE_ADJ_RATIO);
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
                damageFinal += std::max(damageFromStrength, SPECIAL_HIT_DAMAGE_MIN);
            }

            if (isCriticalHit_OutParam)
            {
                damageFinal += std::max(DAMAGE_FROM_WEAPON, SPECIAL_HIT_DAMAGE_MIN);
            }
        }

        // update TurnInfo
        auto turnInfo{ Encounter::Instance()->GetTurnInfoCopy(CREATURE_DEFENDING_PTR) };

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
        turnInfo.UpdateMostDamagedCreature(damageFinal.Abs(), CREATURE_ATTACKING_PTR);
        turnInfo.SetWasHitLastTurn(true);

        Encounter::Instance()->SetTurnInfo(CREATURE_DEFENDING_PTR, turnInfo);

        return damageFinal * Health_t(-1);
    }

    bool CreatureInteraction::AreAnyOfCondsContained(
        const creature::CondEnumVec_t & CONDS_VEC,
        const creature::CreaturePtr_t CREATURE_PTR,
        const HitInfoVec_t & HIT_INFO_VEC) const
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

    stats::Trait_t CreatureInteraction::IsValuetHigherThanRatioOfStat(
        const stats::Trait_t STAT_VALUE, const stats::Trait_t STAT_MAX, const float RATIO) const
    {
        return (STAT_VALUE >= static_cast<stats::Trait_t>(static_cast<float>(STAT_MAX) * RATIO));
    }

} // namespace combat
} // namespace heroespath
