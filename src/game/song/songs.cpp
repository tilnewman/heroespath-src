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

#include "misc/random.hpp"


namespace game
{
namespace song
{

    const std::string RallyDrum::EffectCreature(
        creature::CreaturePtr_t creaturePlayingPtr,
        creature::CreaturePtr_t creatureListeningPtr,
        creature::ConditionEnumVec_t & conditionsAddedVec) const
    {
        if (creatureListeningPtr->IsAlive() == false)
        {
            return creatureListeningPtr->NameOrRaceAndRole() + EFFECT_STR_IS_ALREADY_ + "dead.";
        }

        if (creatureListeningPtr->HasCondition(creature::Conditions::Bold))
        {
            return creatureListeningPtr->NameOrRaceAndRole() + EFFECT_STR_IS_ALREADY_ +
                "emboldened.";
        }
        else
        {
            if (creature::Stats::Roll(creaturePlayingPtr, { stats::stat::Intelligence,
                                                            stats::stat::Charm}))
            {
                creatureListeningPtr->ConditionAdd(creature::Conditions::Bold);
                conditionsAddedVec.push_back(creature::Conditions::Bold);
                return Song::EFFECT_STR_SUCCESS_;
            }
            else
            {
                return creatureListeningPtr->NameOrRaceAndRole() + EFFECT_STR_NOT_EFFECTED_;
            }
        }
    }


    const std::string SpiritResonance::EffectCreature(
        creature::CreaturePtr_t creaturePlayingPtr,
        creature::CreaturePtr_t creatureListeningPtr,
        creature::ConditionEnumVec_t &) const
    {
        if (creatureListeningPtr->IsAlive() == false)
        {
            return creatureListeningPtr->NameOrRaceAndRole() + EFFECT_STR_IS_ALREADY_ + "dead.";
        }

        creatureListeningPtr->ManaCurrentAdj(3 + static_cast<stats::Mana_t>(7.0f *
            creature::Stats::Ratio(creaturePlayingPtr,
                { stats::stat::Intelligence, stats::stat::Charm })));

        return Song::EFFECT_STR_SUCCESS_;
    }


    const std::string RousingRhythm::EffectCreature(
        creature::CreaturePtr_t,
        creature::CreaturePtr_t creatureListeningPtr,
        creature::ConditionEnumVec_t &) const
    {
        if (creatureListeningPtr->IsAlive() == false)
        {
            return creatureListeningPtr->NameOrRaceAndRole() + EFFECT_STR_IS_ALREADY_ + "dead.";
        }

        if (creatureListeningPtr->HasCondition(creature::Conditions::AsleepNatural))
        {
            creatureListeningPtr->ConditionRemove(creature::Conditions::AsleepNatural);
        }

        if (creatureListeningPtr->HasCondition(creature::Conditions::AsleepMagical))
        {
            creatureListeningPtr->ConditionRemove(creature::Conditions::AsleepMagical);
        }

        if (creatureListeningPtr->HasCondition(creature::Conditions::Dazed))
        {
            creatureListeningPtr->ConditionRemove(creature::Conditions::Dazed);
        }

        if (creatureListeningPtr->HasCondition(creature::Conditions::Dazed))
        {
            creatureListeningPtr->ConditionRemove(creature::Conditions::Dazed);
        }

        if (creatureListeningPtr->HasCondition(creature::Conditions::Unconscious))
        {
            creatureListeningPtr->ConditionRemove(creature::Conditions::Unconscious);
        }

        return Song::EFFECT_STR_SUCCESS_;
    }


    const std::string TripBeat::EffectCreature(
        creature::CreaturePtr_t creaturePlayingPtr,
        creature::CreaturePtr_t creatureListeningPtr,
        creature::ConditionEnumVec_t & conditionsAddedVec) const
    {
        if (creatureListeningPtr->IsAlive() == false)
        {
            return creatureListeningPtr->NameOrRaceAndRole() + EFFECT_STR_IS_ALREADY_ + "dead.";
        }

        if (creatureListeningPtr->HasCondition(creature::Conditions::Tripped))
        {
            return creatureListeningPtr->NameOrRaceAndRole() + EFFECT_STR_IS_ALREADY_ + "tripped.";
        }
        else
        {
            if (creature::Stats::Versus(
                creaturePlayingPtr,
                { stats::stat::Intelligence, stats::stat::Charm},
                creatureListeningPtr))
            {
                creatureListeningPtr->ConditionAdd(creature::Conditions::Tripped);
                conditionsAddedVec.push_back(creature::Conditions::Tripped);
                return Song::EFFECT_STR_SUCCESS_;
            }
            else
            {
                return creatureListeningPtr->NameOrRaceAndRole() + EFFECT_STR_RESISTED_;
            }
        }
    }


    const std::string PanicStrings::EffectCreature(
        creature::CreaturePtr_t creaturePlayingPtr,
        creature::CreaturePtr_t creatureListeningPtr,
        creature::ConditionEnumVec_t & conditionsAddedVec) const
    {
        if (creatureListeningPtr->IsAlive() == false)
        {
            return creatureListeningPtr->NameOrRaceAndRole() + EFFECT_STR_IS_ALREADY_ + "dead.";
        }

        if (creatureListeningPtr->HasCondition(creature::Conditions::Panic))
        {
            return creatureListeningPtr->NameOrRaceAndRole() + EFFECT_STR_IS_ALREADY_ + "panicked.";
        }
        else
        {
            if (creature::Stats::Versus(
                creaturePlayingPtr,
                { stats::stat::Intelligence, stats::stat::Charm},
                creatureListeningPtr))
            {
                creatureListeningPtr->ConditionAdd(creature::Conditions::Panic);
                conditionsAddedVec.push_back(creature::Conditions::Panic);
                return Song::EFFECT_STR_SUCCESS_;
            }
            else
            {
                return creatureListeningPtr->NameOrRaceAndRole() + EFFECT_STR_RESISTED_;
            }
        }
    }


    const std::string Lullaby::EffectCreature(
        creature::CreaturePtr_t creaturePlayingPtr,
        creature::CreaturePtr_t creatureListeningPtr,
        creature::ConditionEnumVec_t & conditionsAddedVec) const
    {
        if (creatureListeningPtr->IsAlive() == false)
        {
            return creatureListeningPtr->NameOrRaceAndRole() + EFFECT_STR_IS_ALREADY_ + "dead.";
        }

        if (creatureListeningPtr->HasCondition(creature::Conditions::AsleepNatural) ||
            creatureListeningPtr->HasCondition(creature::Conditions::AsleepMagical))
        {
            return creatureListeningPtr->NameOrRaceAndRole() + EFFECT_STR_IS_ALREADY_ + "asleep.";
        }
        else
        {
            if (creature::Stats::Roll(creaturePlayingPtr, { stats::stat::Intelligence,
                                                            stats::stat::Charm}))
            {
                creatureListeningPtr->ConditionAdd(creature::Conditions::AsleepMagical);
                conditionsAddedVec.push_back(creature::Conditions::AsleepMagical);
                return Song::EFFECT_STR_SUCCESS_;
            }
            else
            {
                return creatureListeningPtr->NameOrRaceAndRole() + EFFECT_STR_RESISTED_;
            }
        }
    }

}
}
