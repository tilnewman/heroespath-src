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

#include "misc/random.hpp"


namespace game
{
namespace song
{

    const std::string SleepMelody::ActionPhrase(creature::CreaturePtr_t creaturePlayingPtr,
                                                creature::CreaturePtr_t) const
    {
        return ActionPhraseBeginning(creaturePlayingPtr);
    }


    const std::string SleepMelody::EffectCreature(
        creature::CreaturePtr_t creaturePlayingPtr,
        creature::CreaturePtr_t creatureListeningPtr,
        creature::ConditionEnumVec_t & conditionsAddedVec) const
    {
        if (creatureListeningPtr->IsAlive() == false)
        {
            return creatureListeningPtr->NameOrRaceAndRole() + EFFECT_STR_IS_ALREADY_ + "dead";
        }

        if (creatureListeningPtr->HasCondition(creature::Conditions::AsleepNatural) ||
            creatureListeningPtr->HasCondition(creature::Conditions::AsleepMagical))
        {
            return creatureListeningPtr->NameOrRaceAndRole() + EFFECT_STR_IS_ALREADY_ + "asleep";
        }
        else
        {
            auto const RAND_INT_PLAYER{ misc::random::Int(
                creaturePlayingPtr->Stats().Int().CurrentReduced(),
                creaturePlayingPtr->Stats().Int().Current()) };

            auto const RAND_CHA_PLAYER{ misc::random::Int(
                creaturePlayingPtr->Stats().Cha().CurrentReduced(),
                creaturePlayingPtr->Stats().Cha().Current()) };

            auto const RAND_PLAYER{ (RAND_INT_PLAYER + RAND_CHA_PLAYER) / 2};

            //check for player character 'natural' success
            if (creaturePlayingPtr->IsPlayerCharacter() &&
                (RAND_PLAYER >= (creaturePlayingPtr->Stats().Int().Normal() +
                    creaturePlayingPtr->Stats().Cha().Normal())))
            {
                creatureListeningPtr->ConditionAdd(creature::Conditions::AsleepMagical);
                conditionsAddedVec.push_back(creature::Conditions::AsleepMagical);
                return Song::EFFECT_STR_SUCCESS_;
            }

            //check for 'normal' success
            auto const RAND_LISTENER{ misc::random::Int(
                creatureListeningPtr->Stats().Int().CurrentReduced(),
                creatureListeningPtr->Stats().Int().Current()) };

            auto const CHANCE_VAL_PLAYER{ RAND_PLAYER +
                static_cast<int>(creaturePlayingPtr->Rank()) };

            auto const CHANCE_VAL_LISTENER{ RAND_LISTENER +
                static_cast<int>(creatureListeningPtr->Rank()) };

            if (CHANCE_VAL_PLAYER >= CHANCE_VAL_LISTENER)
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
