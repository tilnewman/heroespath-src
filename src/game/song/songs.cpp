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

#include "game/creature/creature.hpp"
#include "game/creature/stats.hpp"
#include "game/combat/encounter.hpp"

#include "misc/random.hpp"


namespace game
{
namespace song
{

    bool RallyDrum::EffectCreature(
        creature::CreaturePtr_t   creaturePlayingPtr,
        creature::CreaturePtr_t   creatureListeningPtr,
        stats::Health_t &,
        creature::CondEnumVec_t & condsAddedVec,
        creature::CondEnumVec_t &,
        ContentAndNamePos &       actionPhraseCNP) const
    {
        if (creatureListeningPtr->HasCondition(creature::Conditions::Bold))
        {
            actionPhraseCNP = ContentAndNamePos(
                "",
                "'s " + TypeToVerb() + Song::FAILED_STR_,
                " because " + creature::sex::HeSheIt(creatureListeningPtr->Sex(), false) +
                    " is already emboldened.",
                NamePosition::SourceThenTarget);

            return false;
        }
        else
        {
            if (creature::Stats::Test(creaturePlayingPtr,
                                      { stats::stat::Intelligence, stats::stat::Charm},
                                      0.5f,
                                      true,
                                      true))
            {
                creatureListeningPtr->ConditionAdd(creature::Conditions::Bold);
                condsAddedVec.push_back(creature::Conditions::Bold);

                actionPhraseCNP = ContentAndNamePos(
                    "",
                    "'s magical " + TypeToVerb() + " emboldens ",
                    ".",
                    NamePosition::SourceThenTarget);

                return true;
            }
            else
            {
                actionPhraseCNP = ContentAndNamePos(
                    "",
                    "'s " + TypeToVerb() + Song::FAILED_STR_,
                    " because " + creature::sex::HeSheIt(creatureListeningPtr->Sex(), false) +
                        Song::RESISTED_STR_,
                    NamePosition::SourceThenTarget);

                return false;
            }
        }
    }


    bool SpiritResonance::EffectCreature(
        creature::CreaturePtr_t   creaturePlayingPtr,
        creature::CreaturePtr_t   creatureListeningPtr,
        stats::Health_t &,
        creature::CondEnumVec_t &,
        creature::CondEnumVec_t &,
        ContentAndNamePos &       actionPhraseCNP) const
    {
        auto const RATIO{ creature::Stats::Ratio(
            creaturePlayingPtr,
            { stats::stat::Intelligence, stats::stat::Charm },
            false,
            true,
            true) };

        auto const MANA_GAIN_ORIG{ static_cast<stats::Mana_t>(10.0f * RATIO) };

        auto const MANA_GAIN_MAX{ creatureListeningPtr->ManaMissing() };

        auto const MANA_GAIN_FINAL{ ((MANA_GAIN_ORIG > MANA_GAIN_MAX) ?
            MANA_GAIN_MAX : MANA_GAIN_ORIG) };

        if (MANA_GAIN_FINAL > 0)
        {
            creatureListeningPtr->ManaCurrentAdj(MANA_GAIN_FINAL);

            std::ostringstream ss;
            ss << "'s mana for " << MANA_GAIN_FINAL << ".";

            actionPhraseCNP = ContentAndNamePos("",
                                                "'s " + TypeToVerb() + " recharges ",
                                                ss.str(),
                                                NamePosition::SourceThenTarget);

            return true;
        }
        else
        {
            actionPhraseCNP = ContentAndNamePos(
                "",
                "'s " + TypeToVerb() + Song::FAILED_STR_,
                " because " + creature::sex::HeSheIt(creatureListeningPtr->Sex(), false) +
                    " already has full mana.",
                NamePosition::SourceThenTarget);

            return false;
        }
    }


    bool RousingRhythm::EffectCreature(
        creature::CreaturePtr_t,
        creature::CreaturePtr_t   creatureListeningPtr,
        stats::Health_t &,
        creature::CondEnumVec_t &,
        creature::CondEnumVec_t & condsRemovedVec,
        ContentAndNamePos &       actionPhraseCNP) const
    {
        const creature::CondEnumVec_t CONDS_TO_REMOVE_VEC{
            creature::Conditions::AsleepNatural,
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
            actionPhraseCNP = ContentAndNamePos(
                "",
                "'s " + TypeToVerb() + Song::FAILED_STR_,
                " because " + creature::sex::HeSheIt(creatureListeningPtr->Sex(), false) +
                    " did not need rousing.",
                NamePosition::SourceThenTarget);

            return false;
        }
        else
        {
            condsRemovedVec.insert(condsRemovedVec.end(),
                                   condsToRemoveVec.begin(),
                                   condsToRemoveVec.end());

            actionPhraseCNP = ContentAndNamePos("",
                                                "'s " + TypeToVerb() + " rouses ",
                                                ".",
                                                NamePosition::SourceThenTarget);

            return true;
        }
    }


    bool TripBeat::EffectCreature(
        creature::CreaturePtr_t   creaturePlayingPtr,
        creature::CreaturePtr_t   creatureListeningPtr,
        stats::Health_t &,
        creature::CondEnumVec_t & condsAddedVec,
        creature::CondEnumVec_t &,
        ContentAndNamePos &       actionPhraseCNP) const
    {
        if (creatureListeningPtr->HasCondition(creature::Conditions::Tripped))
        {
            actionPhraseCNP = ContentAndNamePos(
                "",
                "'s " + TypeToVerb() + Song::FAILED_STR_,
                " because " + creature::sex::HeSheIt(creatureListeningPtr->Sex(), false) +
                    " is already tripped.",
                NamePosition::SourceThenTarget);

            return false;
        }
        else if (combat::Encounter::Instance()->GetTurnInfoCopy(creatureListeningPtr).GetIsFlying())
        {
            actionPhraseCNP = ContentAndNamePos(
                "",
                "'s " + TypeToVerb() + Song::FAILED_STR_,
                " because " + creature::sex::HeSheIt(creatureListeningPtr->Sex(), false) +
                    " is flying.",
                NamePosition::SourceThenTarget);

            return false;
        }
        else
        {
            if (creature::Stats::Versus(creaturePlayingPtr,
                                        { stats::stat::Intelligence, stats::stat::Charm },
                                        creatureListeningPtr,
                                        {},
                                        0,
                                        0,
                                        false,
                                        true,
                                        true,
                                        true,
                                        true))
            {
                creatureListeningPtr->ConditionAdd(creature::Conditions::Tripped);
                condsAddedVec.push_back(creature::Conditions::Tripped);

                actionPhraseCNP = ContentAndNamePos(
                    "",
                    "'s " + TypeToVerb() + " trips ",
                    ".",
                    NamePosition::SourceThenTarget);

                return true;
            }
            else
            {
                actionPhraseCNP = ContentAndNamePos(
                    "",
                    "'s " + TypeToVerb() + Song::FAILED_STR_,
                    " because " + creature::sex::HeSheIt(creatureListeningPtr->Sex(), false) +
                        Song::RESISTED_STR_,
                    NamePosition::SourceThenTarget);

                return false;
            }
        }
    }


    bool PanicStrings::EffectCreature(
        creature::CreaturePtr_t   creaturePlayingPtr,
        creature::CreaturePtr_t   creatureListeningPtr,
        stats::Health_t &,
        creature::CondEnumVec_t & condsAddedVec,
        creature::CondEnumVec_t &,
        ContentAndNamePos &       actionPhraseCNP) const
    {
        if (creatureListeningPtr->HasCondition(creature::Conditions::Panic))
        {
            actionPhraseCNP = ContentAndNamePos(
                "",
                "'s " + TypeToVerb() + Song::FAILED_STR_,
                " because " + creature::sex::HeSheIt(creatureListeningPtr->Sex(), false) +
                    " is already panicked.",
                NamePosition::SourceThenTarget);

            return false;
        }
        else
        {
            if (creature::Stats::Versus(creaturePlayingPtr,
                                        { stats::stat::Intelligence, stats::stat::Charm},
                                        creatureListeningPtr,
                                        {},
                                        0,
                                        0,
                                        false,
                                        true,
                                        true,
                                        true,
                                        true))
            {
                creatureListeningPtr->ConditionAdd(creature::Conditions::Panic);
                condsAddedVec.push_back(creature::Conditions::Panic);

                actionPhraseCNP = ContentAndNamePos(
                    "",
                    "'s " + TypeToVerb() + " panics ",
                    ".",
                    NamePosition::SourceThenTarget);

                return true;
            }
            else
            {
                actionPhraseCNP = ContentAndNamePos(
                    "",
                    "'s " + TypeToVerb() + Song::FAILED_STR_,
                    " because " + creature::sex::HeSheIt(creatureListeningPtr->Sex(), false) +
                        Song::RESISTED_STR_,
                    NamePosition::SourceThenTarget);

                return false;
            }
        }
    }


    bool Lullaby::EffectCreature(
        creature::CreaturePtr_t   creaturePlayingPtr,
        creature::CreaturePtr_t   creatureListeningPtr,
        stats::Health_t &,
        creature::CondEnumVec_t & condsAddedVec,
        creature::CondEnumVec_t &,
        ContentAndNamePos &       actionPhraseCNP) const
    {
        if ((creatureListeningPtr->HasCondition(creature::Conditions::AsleepNatural)) &&
            (creatureListeningPtr->HasCondition(creature::Conditions::AsleepMagical)))
        {
            actionPhraseCNP = ContentAndNamePos(
                "",
                "'s " + TypeToVerb() + Song::FAILED_STR_,
                " because " + creature::sex::HeSheIt(creatureListeningPtr->Sex(), false) +
                    " is already sleeping.",
                NamePosition::SourceThenTarget);

            return false;
        }
        else
        {
            if (creature::Stats::Test(creaturePlayingPtr,
                                      { stats::stat::Intelligence, stats::stat::Charm},
                                      0.5f,
                                      true,
                                      true))
            {
                creatureListeningPtr->ConditionAdd(creature::Conditions::AsleepNatural);
                condsAddedVec.push_back(creature::Conditions::AsleepNatural);

                actionPhraseCNP = ContentAndNamePos(
                    "",
                    "'s " + TypeToVerb() + " puts ",
                    " into a peaceful slumber.",
                    NamePosition::SourceThenTarget);

                combat::Encounter::Instance()->SetIsFlying(creatureListeningPtr, false);
                return true;
            }
            else
            {
                actionPhraseCNP = ContentAndNamePos(
                    "",
                    "'s " + TypeToVerb() + Song::FAILED_STR_,
                    " because " + creature::sex::HeSheIt(creatureListeningPtr->Sex(), false) +
                        Song::RESISTED_STR_,
                    NamePosition::SourceThenTarget);

                return false;
            }
        }
    }

}
}
