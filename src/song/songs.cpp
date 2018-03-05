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
// songs.cpp
//
#include "songs.hpp"

#include "combat/encounter.hpp"
#include "creature/creature.hpp"
#include "creature/stats.hpp"

#include "misc/random.hpp"

namespace heroespath
{
namespace song
{

    bool RallyDrum::EffectCreature(
        creature::CreaturePtr_t creaturePlayingPtr,
        creature::CreaturePtr_t creatureListeningPtr,
        Health_t &,
        creature::CondEnumVec_t & condsAddedVec,
        creature::CondEnumVec_t &,
        combat::ContentAndNamePos & actionPhraseCNP) const
    {
        if (creatureListeningPtr->HasCondition(creature::Conditions::Bold))
        {
            actionPhraseCNP = combat::ContentAndNamePos(
                "",
                "'s " + TypeToVerb() + Song::FAILED_STR_,
                " because " + creature::sex::HeSheIt(creatureListeningPtr->Sex(), false)
                    + " is already emboldened.",
                combat::NamePosition::SourceThenTarget);

            return false;
        }
        else
        {
            auto const DID_STAT_ROLL_SUCCEED{ creature::Stats::Test(
                creaturePlayingPtr,
                { stats::Traits::Intelligence, stats::Traits::Charm },
                static_cast<creature::Stats::With>(
                    creature::Stats::With::Luck | creature::Stats::With::RaceRoleBonus)) };

            auto const DID_MAGIC_CAST_TRAIT_BONUS_SUCCEED{ creaturePlayingPtr->TraitBonusTest(
                stats::Traits::MagicCast) };

            if (DID_STAT_ROLL_SUCCEED || DID_MAGIC_CAST_TRAIT_BONUS_SUCCEED)
            {
                creatureListeningPtr->ConditionAdd(creature::Conditions::Bold);
                condsAddedVec.push_back(creature::Conditions::Bold);

                actionPhraseCNP = combat::ContentAndNamePos(
                    "",
                    "'s magical " + TypeToVerb() + " emboldens ",
                    ".",
                    combat::NamePosition::SourceThenTarget);

                return true;
            }
            else
            {
                actionPhraseCNP = combat::ContentAndNamePos(
                    "",
                    "'s " + TypeToVerb() + Song::FAILED_STR_,
                    " because " + creature::sex::HeSheIt(creatureListeningPtr->Sex(), false)
                        + Song::RESISTED_STR_,
                    combat::NamePosition::SourceThenTarget);

                return false;
            }
        }
    }

    bool SpiritResonance::EffectCreature(
        creature::CreaturePtr_t creaturePlayingPtr,
        creature::CreaturePtr_t creatureListeningPtr,
        Health_t &,
        creature::CondEnumVec_t &,
        creature::CondEnumVec_t &,
        combat::ContentAndNamePos & actionPhraseCNP) const
    {
        auto const RATIO_FROM_STATS{ creature::Stats::Ratio(
            creaturePlayingPtr,
            { stats::Traits::Intelligence, stats::Traits::Charm },
            (creature::Stats::Luck | creature::Stats::RaceRoleBonus)) };

        auto const RATIO_FROM_TRAIT_BONUS{ static_cast<float>(creaturePlayingPtr->TraitBonusCurrent(
                                               stats::Traits::MagicEffect))
                                           / 100.0f };

        auto const MANA_GAIN_ORIG{ Mana_t(
            static_cast<Mana_t::type>(10.0f * (RATIO_FROM_STATS + RATIO_FROM_TRAIT_BONUS))) };

        auto const MANA_GAIN_MAX{ creatureListeningPtr->ManaMissing() };

        auto const MANA_GAIN_FINAL{ (
            (MANA_GAIN_ORIG > MANA_GAIN_MAX) ? MANA_GAIN_MAX : MANA_GAIN_ORIG) };

        if (MANA_GAIN_FINAL > 0_mana)
        {
            creatureListeningPtr->ManaAdj(MANA_GAIN_FINAL);

            std::ostringstream ss;
            ss << "'s mana for " << MANA_GAIN_FINAL << ".";

            actionPhraseCNP = combat::ContentAndNamePos(
                "",
                "'s " + TypeToVerb() + " recharges ",
                ss.str(),
                combat::NamePosition::SourceThenTarget);

            return true;
        }
        else
        {
            actionPhraseCNP = combat::ContentAndNamePos(
                "",
                "'s " + TypeToVerb() + Song::FAILED_STR_,
                " because " + creature::sex::HeSheIt(creatureListeningPtr->Sex(), false)
                    + " already has full mana.",
                combat::NamePosition::SourceThenTarget);

            return false;
        }
    }

    bool RousingRhythm::EffectCreature(
        creature::CreaturePtr_t,
        creature::CreaturePtr_t creatureListeningPtr,
        Health_t &,
        creature::CondEnumVec_t &,
        creature::CondEnumVec_t & condsRemovedVec,
        combat::ContentAndNamePos & actionPhraseCNP) const
    {
        const creature::CondEnumVec_t CONDS_TO_REMOVE_VEC{ creature::Conditions::AsleepNatural,
                                                           creature::Conditions::AsleepMagical,
                                                           creature::Conditions::Dazed,
                                                           creature::Conditions::Unconscious };

        creature::CondEnumVec_t condsToRemoveVec;
        for (auto const NEXT_COND_TO_REMOVE_ENUM : CONDS_TO_REMOVE_VEC)
        {
            if (creatureListeningPtr->HasCondition(NEXT_COND_TO_REMOVE_ENUM))
            {
                creatureListeningPtr->ConditionRemove(NEXT_COND_TO_REMOVE_ENUM);
                condsToRemoveVec.push_back(NEXT_COND_TO_REMOVE_ENUM);
            }
        }

        if (condsToRemoveVec.empty())
        {
            actionPhraseCNP = combat::ContentAndNamePos(
                "",
                "'s " + TypeToVerb() + Song::FAILED_STR_,
                " because " + creature::sex::HeSheIt(creatureListeningPtr->Sex(), false)
                    + " did not need rousing.",
                combat::NamePosition::SourceThenTarget);

            return false;
        }
        else
        {
            condsRemovedVec.insert(
                condsRemovedVec.end(), condsToRemoveVec.begin(), condsToRemoveVec.end());

            actionPhraseCNP = combat::ContentAndNamePos(
                "", "'s " + TypeToVerb() + " rouses ", ".", combat::NamePosition::SourceThenTarget);

            return true;
        }
    }

    bool TripBeat::EffectCreature(
        creature::CreaturePtr_t creaturePlayingPtr,
        creature::CreaturePtr_t creatureListeningPtr,
        Health_t &,
        creature::CondEnumVec_t & condsAddedVec,
        creature::CondEnumVec_t &,
        combat::ContentAndNamePos & actionPhraseCNP) const
    {
        if (creatureListeningPtr->HasCondition(creature::Conditions::Tripped))
        {
            actionPhraseCNP = combat::ContentAndNamePos(
                "",
                "'s " + TypeToVerb() + Song::FAILED_STR_,
                " because " + creature::sex::HeSheIt(creatureListeningPtr->Sex(), false)
                    + " is already tripped.",
                combat::NamePosition::SourceThenTarget);

            return false;
        }
        else if (combat::Encounter::Instance()->GetTurnInfoCopy(creatureListeningPtr).GetIsFlying())
        {
            actionPhraseCNP = combat::ContentAndNamePos(
                "",
                "'s " + TypeToVerb() + Song::FAILED_STR_,
                " because " + creature::sex::HeSheIt(creatureListeningPtr->Sex(), false)
                    + " is flying.",
                combat::NamePosition::SourceThenTarget);

            return false;
        }
        else
        {
            auto const DID_STAT_ROLL_SUCCEED{ creature::Stats::Versus(
                creaturePlayingPtr,
                { stats::Traits::Intelligence, stats::Traits::Charm },
                creatureListeningPtr,
                {},
                0,
                0,
                static_cast<creature::Stats::With>(
                    creature::Stats::With::Luck | creature::Stats::With::RaceRoleBonus
                    | creature::Stats::With::RankBonus
                    | creature::Stats::With::PlayerNaturalWins)) };

            auto const DID_MAGIC_CAST_TRAIT_BONUS_SUCCEED{ creaturePlayingPtr->TraitBonusTest(
                stats::Traits::MagicCast) };

            if (DID_STAT_ROLL_SUCCEED || DID_MAGIC_CAST_TRAIT_BONUS_SUCCEED)
            {
                creatureListeningPtr->ConditionAdd(creature::Conditions::Tripped);
                condsAddedVec.push_back(creature::Conditions::Tripped);

                actionPhraseCNP = combat::ContentAndNamePos(
                    "",
                    "'s " + TypeToVerb() + " trips ",
                    ".",
                    combat::NamePosition::SourceThenTarget);

                return true;
            }
            else
            {
                actionPhraseCNP = combat::ContentAndNamePos(
                    "",
                    "'s " + TypeToVerb() + Song::FAILED_STR_,
                    " because " + creature::sex::HeSheIt(creatureListeningPtr->Sex(), false)
                        + Song::RESISTED_STR_,
                    combat::NamePosition::SourceThenTarget);

                return false;
            }
        }
    }

    bool PanicStrings::EffectCreature(
        creature::CreaturePtr_t creaturePlayingPtr,
        creature::CreaturePtr_t creatureListeningPtr,
        Health_t &,
        creature::CondEnumVec_t & condsAddedVec,
        creature::CondEnumVec_t &,
        combat::ContentAndNamePos & actionPhraseCNP) const
    {
        if (creatureListeningPtr->HasCondition(creature::Conditions::Panic))
        {
            actionPhraseCNP = combat::ContentAndNamePos(
                "",
                "'s " + TypeToVerb() + Song::FAILED_STR_,
                " because " + creature::sex::HeSheIt(creatureListeningPtr->Sex(), false)
                    + " is already panicked.",
                combat::NamePosition::SourceThenTarget);

            return false;
        }
        else
        {
            auto const DID_STAT_ROLL_SUCCEED{ creature::Stats::Versus(
                creaturePlayingPtr,
                { stats::Traits::Intelligence, stats::Traits::Charm },
                creatureListeningPtr,
                {},
                0,
                0,
                static_cast<creature::Stats::With>(
                    creature::Stats::With::Luck | creature::Stats::With::RaceRoleBonus
                    | creature::Stats::With::RankBonus
                    | creature::Stats::With::PlayerNaturalWins)) };

            auto const DID_MAGIC_CAST_TRAIT_BONUS_SUCCEED{ creaturePlayingPtr->TraitBonusTest(
                stats::Traits::MagicCast) };

            if (DID_STAT_ROLL_SUCCEED || DID_MAGIC_CAST_TRAIT_BONUS_SUCCEED)
            {
                creatureListeningPtr->ConditionAdd(creature::Conditions::Panic);
                condsAddedVec.push_back(creature::Conditions::Panic);

                actionPhraseCNP = combat::ContentAndNamePos(
                    "",
                    "'s " + TypeToVerb() + " panics ",
                    ".",
                    combat::NamePosition::SourceThenTarget);

                return true;
            }
            else
            {
                actionPhraseCNP = combat::ContentAndNamePos(
                    "",
                    "'s " + TypeToVerb() + Song::FAILED_STR_,
                    " because " + creature::sex::HeSheIt(creatureListeningPtr->Sex(), false)
                        + Song::RESISTED_STR_,
                    combat::NamePosition::SourceThenTarget);

                return false;
            }
        }
    }

    bool Lullaby::EffectCreature(
        creature::CreaturePtr_t creaturePlayingPtr,
        creature::CreaturePtr_t creatureListeningPtr,
        Health_t &,
        creature::CondEnumVec_t & condsAddedVec,
        creature::CondEnumVec_t &,
        combat::ContentAndNamePos & actionPhraseCNP) const
    {
        if ((creatureListeningPtr->HasCondition(creature::Conditions::AsleepNatural))
            && (creatureListeningPtr->HasCondition(creature::Conditions::AsleepMagical)))
        {
            actionPhraseCNP = combat::ContentAndNamePos(
                "",
                "'s " + TypeToVerb() + Song::FAILED_STR_,
                " because " + creature::sex::HeSheIt(creatureListeningPtr->Sex(), false)
                    + " is already sleeping.",
                combat::NamePosition::SourceThenTarget);

            return false;
        }
        else
        {
            auto const DID_STAT_ROLL_SUCCEED{ creature::Stats::Test(
                creaturePlayingPtr,
                { stats::Traits::Intelligence, stats::Traits::Charm },
                static_cast<creature::Stats::With>(
                    creature::Stats::With::Luck | creature::Stats::With::RaceRoleBonus)) };

            auto const DID_MAGIC_CAST_TRAIT_BONUS_SUCCEED{ creaturePlayingPtr->TraitBonusTest(
                stats::Traits::MagicCast) };

            if (DID_STAT_ROLL_SUCCEED || DID_MAGIC_CAST_TRAIT_BONUS_SUCCEED)
            {
                creatureListeningPtr->ConditionAdd(creature::Conditions::AsleepNatural);
                condsAddedVec.push_back(creature::Conditions::AsleepNatural);

                actionPhraseCNP = combat::ContentAndNamePos(
                    "",
                    "'s " + TypeToVerb() + " puts ",
                    " into a peaceful slumber.",
                    combat::NamePosition::SourceThenTarget);

                combat::Encounter::Instance()->SetIsFlying(creatureListeningPtr, false);
                return true;
            }
            else
            {
                actionPhraseCNP = combat::ContentAndNamePos(
                    "",
                    "'s " + TypeToVerb() + Song::FAILED_STR_,
                    " because " + creature::sex::HeSheIt(creatureListeningPtr->Sex(), false)
                        + Song::RESISTED_STR_,
                    combat::NamePosition::SourceThenTarget);

                return false;
            }
        }
    }
} // namespace song
} // namespace heroespath
