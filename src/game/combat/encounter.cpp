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
// encounter.cpp
//
#include "encounter.hpp"

#include "sfml-util/sound-manager.hpp"
#include "sfml-util/music-enum.hpp"

#include "game/game.hpp"
#include "game/log-macros.hpp"
#include "game/player/party.hpp"
#include "game/player/character.hpp"
#include "game/non-player/party.hpp"
#include "game/non-player/character.hpp"
#include "game/combat/strategy-details.hpp"
#include "game/combat/party-factory.hpp"
#include "game/state/game-state.hpp"
#include "game/state/world-state.hpp"
#include "misc/assertlogandthrow.hpp"
#include "game/creature/algorithms.hpp"

#include "misc/vectors.hpp"

#include <sstream>
#include <exception>
#include <algorithm>


namespace game
{
namespace combat
{

    std::unique_ptr<Encounter> Encounter::instanceUPtr_{ nullptr };


    Encounter::Encounter()
    :
        enemyPartyUPtr_    (),
        deadEnemyPartyUPtr_(std::make_unique<non_player::Party>()),
        roundCounter_      (0),
        hasStarted_        (false),
        turnOverPVec_      (),
        turnIndex_         (0),
        turnInfoMap_       (),
        turnCreaturePtr_   ()
    {
        M_HP_LOG_DBG("Singleton Construction: Encounter");
    }


    Encounter::~Encounter()
    {
        M_HP_LOG_DBG("Singleton Destruction: Encounter");
    }


    Encounter * Encounter::Instance()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            M_HP_LOG_WRN("Singleton Instance() before Acquire(): Encounter");
            Acquire();
        }

        return instanceUPtr_.get();
    }


    void Encounter::Acquire()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            instanceUPtr_.reset(new Encounter);
        }
        else
        {
            M_HP_LOG_WRN("Singleton Acquire() after Construction: Encounter");
        }
    }


    void Encounter::Release()
    {
        M_ASSERT_OR_LOGANDTHROW_SS((instanceUPtr_.get() != nullptr),
            "game::combat::Encounter::Release() found instanceUPtr that was null.");

        instanceUPtr_.reset();
    }


    non_player::Party & Encounter::NonPlayerParty()
    {
        if (enemyPartyUPtr_.get() == nullptr)
        {
            std::ostringstream ss;
            ss << "game::combat::Encounter::NonPlayerParty() called when ptr was null.";
            throw std::runtime_error(ss.str());
        }
        else
        {
            return * enemyPartyUPtr_;
        }
    }


    non_player::Party & Encounter::DeadNonPlayerParty()
    {
        if (deadEnemyPartyUPtr_.get() == nullptr)
        {
            std::ostringstream ss;
            ss << "game::combat::Encounter::DeadNonPlayerParty() called when ptr was null.";
            throw std::runtime_error(ss.str());
        }
        else
        {
            return * deadEnemyPartyUPtr_;
        }
    }


    void Encounter::Setup_First()
    {
        enemyPartyUPtr_.reset( PartyFactory::Instance()->MakeParty_FirstEncounter() );
    }


    const TurnInfo Encounter::GetTurnInfoCopy(const creature::CreaturePtr_t P) const
    {
        M_ASSERT_OR_LOGANDTHROW_SS((P != nullptr),
            "game::combat::Encounter::GetTurnInfoCopy() was given a nullptr"
            << " creature::CreaturePtr_t.");

        auto const FOUND_ITER{ turnInfoMap_.find(P) };

        M_ASSERT_OR_LOGANDTHROW_SS((FOUND_ITER != turnInfoMap_.end()),
            "game::combat::Encounter::GetTurnInfoCopy(creature::CreaturePtr_t=" << P->Name()
            << ") was not found in the turnInfoMap_.");

        return FOUND_ITER->second;
    }


    void Encounter::StartTasks()
    {
        roundCounter_ = 0;
        Game::Instance()->State().World().EncoundterCountInc();

        sfml_util::SoundManager::Instance()->MusicStart(sfml_util::music::CombatIntro);

        if (Game::Instance()->State().IsNewGame())
        {
            GenerateFirstEncounter();
        }

        PopulateTurnInfoMap();
        SortAndSetTurnCreature();
        hasStarted_ = true;
    }


    void Encounter::EndTasks()
    {
        turnOverPVec_.clear();
        turnInfoMap_.clear();
        enemyPartyUPtr_.reset();
        roundCounter_ = 0;
        hasStarted_ = false;
        turnIndex_ = 0;
    }


    void Encounter::HandleKilledCreature(creature::CreatureCPtrC_t CREATURE_CPTRC)
    {
        //make sure no TurnInfo objects refer to a killed creature
        for (auto & nextCreatureTurnInfoPair : turnInfoMap_)
        {
            nextCreatureTurnInfoPair.second.RemoveDeadCreatureTasks(CREATURE_CPTRC);
        }

        //no need to remove from turnOverPVec_

        //no need to null out turnCreaturePtr_

        //move from the enemyParty to the deadEnemyParty for later loot collection
        auto killedCharacterPtr{ enemyPartyUPtr_->FindByCreaturePtr(CREATURE_CPTRC) };
        enemyPartyUPtr_->Remove(killedCharacterPtr, false);
        deadEnemyPartyUPtr_->Add(killedCharacterPtr, false);
    }


    void Encounter::IncrementTurn()
    {
        ++roundCounter_;

        if (turnCreaturePtr_ != nullptr)
        {
            turnOverPVec_.push_back(turnCreaturePtr_);
        }

        //re-sort by speed every turn because spells/conditions/etc could have changed
        //creature speed during any given turn
        SortAndSetTurnCreature();
    }


    void Encounter::GenerateFirstEncounter()
    {
        combat::Encounter::Instance()->Setup_First();
    }


    void Encounter::PopulateTurnInfoMap()
    {
        turnInfoMap_.clear();

        for (auto const NEXT_CHAR_PTR : enemyPartyUPtr_->Characters())
        {
            //enemy creatures need a real populated strategy info object
            TurnInfo turnInfo;
            turnInfo.SetStrategyInfo( strategy::ChanceFactory::Instance()->Get(
                NEXT_CHAR_PTR->Race().Which(), NEXT_CHAR_PTR->Role().Which()).Make() );

            turnInfoMap_[NEXT_CHAR_PTR] = turnInfo;
        }

        for (auto const NEXT_CHAR_PTR : Game::Instance()->State().Party().Characters())
        {
            turnInfoMap_[NEXT_CHAR_PTR] = TurnInfo();
        }
    }


    void Encounter::SortAndSetTurnCreature()
    {
        creature::CreaturePVec_t allLivingCreaturesPVec;
        misc::Vector::Append(creature::Algorithms::Players(true), allLivingCreaturesPVec);
        misc::Vector::Append(creature::Algorithms::NonPlayers(true), allLivingCreaturesPVec);

        if (turnOverPVec_.size() >= allLivingCreaturesPVec.size())
        {
            turnOverPVec_.clear();
        }

        creature::CreaturePVec_t creaturesThatHaveNotTakenTurnYetPVec(
            misc::Vector::Exclude(allLivingCreaturesPVec, turnOverPVec_));

        M_ASSERT_OR_LOGANDTHROW_SS((creaturesThatHaveNotTakenTurnYetPVec.empty() == false),
            "game::combat::Encounter::SortAndSetTurnCreature("
            << ((turnCreaturePtr_ == nullptr) ? "nullptr" : turnCreaturePtr_->Name())
            << ") resulted in an empty creaturesThatHaveNotTakenTurnYetPVec.");

        if (creaturesThatHaveNotTakenTurnYetPVec.size() > 1)
        {
            std::sort(creaturesThatHaveNotTakenTurnYetPVec.begin(),
                      creaturesThatHaveNotTakenTurnYetPVec.end(),
                      []
                      (const creature::CreaturePtr_t A, const creature::CreaturePtr_t B)
                      { return A->Stats().Spd().Current() > B->Stats().Spd().Current(); });
        }

        turnCreaturePtr_ = creaturesThatHaveNotTakenTurnYetPVec.at(0);

        M_ASSERT_OR_LOGANDTHROW_SS((turnCreaturePtr_ != nullptr),
            "game::combat::Encounter::SortAndSetTurnCreature("
            << ((turnCreaturePtr_ == nullptr) ? "nullptr" : turnCreaturePtr_->Name())
            << ") resulted in a nullptr turnCreaturePtr_.");
    }

}
}
