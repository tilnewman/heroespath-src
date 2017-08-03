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
#include "game/creature/algorithms.hpp"
#include "game/item/item.hpp"
#include "game/item/item-warehouse.hpp"

#include "misc/vectors.hpp"
#include "misc/assertlogandthrow.hpp"

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
        nonPlayerPartyUPtr_       (),
        deadNonPlayerPartyUPtr_   (),
        runawayNonPlayerPartyUPtr_(),
        runawayPlayersVec_        (),
        roundCounter_             (0),
        hasStarted_               (false),
        turnOverPVec_             (),
        turnIndex_                (0),
        turnInfoMap_              (),
        turnCreaturePtr_          (),
        deadNonPlayerItemsHeld_   (),
        deadNonPlayerItemsCached_ ()
    {
        M_HP_LOG_DBG("Singleton Construction: Encounter");
        FreeThenResetLivingNonPlayerParty();
        FreeThenResetDeadNonPlayerParty();
        FreeThenResetRunawayNonPlayerParty();
        runawayPlayersVec_.clear();
    }


    Encounter::~Encounter()
    {
        M_HP_LOG_DBG("Singleton Destruction: Encounter");
        FreeThenResetLivingNonPlayerParty();
        FreeThenResetDeadNonPlayerParty();
        FreeThenResetRunawayNonPlayerParty();
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


    non_player::Party & Encounter::LivingNonPlayerParty()
    {
        if (nonPlayerPartyUPtr_.get() == nullptr)
        {
            std::ostringstream ss;
            ss << "game::combat::Encounter::LivingNonPlayerParty() called when ptr was null.";
            throw std::runtime_error(ss.str());
        }
        else
        {
            return * nonPlayerPartyUPtr_;
        }
    }


    non_player::Party & Encounter::DeadNonPlayerParty()
    {
        if (deadNonPlayerPartyUPtr_.get() == nullptr)
        {
            std::ostringstream ss;
            ss << "game::combat::Encounter::DeadNonPlayerParty() called when ptr was null.";
            throw std::runtime_error(ss.str());
        }
        else
        {
            return * deadNonPlayerPartyUPtr_;
        }
    }


    non_player::Party & Encounter::RunawayNonPlayerParty()
    {
        if (runawayNonPlayerPartyUPtr_.get() == nullptr)
        {
            std::ostringstream ss;
            ss << "game::combat::Encounter::RunawayNonPlayerParty() called when ptr was null.";
            throw std::runtime_error(ss.str());
        }
        else
        {
            return * runawayNonPlayerPartyUPtr_;
        }
    }


    bool Encounter::IsRunaway(const creature::CreaturePtr_t CREATURE_PTR) const
    {
        M_ASSERT_OR_LOGANDTHROW_SS((CREATURE_PTR != nullptr),
            "game::combat::Encounter::IsRunaway() given a nullptr.");

        return ((std::find(runawayPlayersVec_.begin(), runawayPlayersVec_.end(), CREATURE_PTR) !=
            runawayPlayersVec_.end()) ||
                (runawayNonPlayerPartyUPtr_->FindByCreaturePtr(CREATURE_PTR) != nullptr));
    }


    void Encounter::Runaway(const creature::CreaturePtr_t CREATURE_PTR)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((CREATURE_PTR != nullptr),
            "game::combat::Encounter::Runaway() given a nullptr.");

        M_ASSERT_OR_LOGANDTHROW_SS((IsRunaway(CREATURE_PTR) == false),
            "game::combat::Encounter::IsRunaway() given a creature that already ran away.");

        if (CREATURE_PTR->IsPlayerCharacter())
        {
            runawayPlayersVec_.push_back(CREATURE_PTR);
        }
        else
        {
            auto const CHARACTER_PTR{ nonPlayerPartyUPtr_->FindByCreaturePtr(CREATURE_PTR) };

            M_ASSERT_OR_LOGANDTHROW_SS((CHARACTER_PTR != nullptr),
                "game::combat::Encounter::IsRunaway() given a non-player creature "
                << "that was not in nonPlayerPartyUPtr_.");

            nonPlayerPartyUPtr_->Remove(CHARACTER_PTR, false);
            runawayNonPlayerPartyUPtr_->Add(CHARACTER_PTR, false);
        }
    }


    void Encounter::Setup_First()
    {
        FreeThenResetLivingNonPlayerParty();
        nonPlayerPartyUPtr_.reset( PartyFactory::Instance()->MakeParty_FirstEncounter() );
    }


    const TurnInfo Encounter::GetTurnInfoCopy(const creature::CreaturePtrC_t PTR) const
    {
        M_ASSERT_OR_LOGANDTHROW_SS((PTR != nullptr),
            "game::combat::Encounter::GetTurnInfoCopy() was given a nullptr"
            << " creature::CreaturePtr_t.");

        auto const FOUND_ITER{ turnInfoMap_.find(PTR) };

        M_ASSERT_OR_LOGANDTHROW_SS((FOUND_ITER != turnInfoMap_.end()),
            "game::combat::Encounter::GetTurnInfoCopy(creature::CreaturePtr_t=" << PTR->Name()
            << ") was not found in the turnInfoMap_.");

        return FOUND_ITER->second;
    }


    void Encounter::BeginCombatTasks()
    {
        //nonPlayerPartyUPtr_ must be left alone because it will have already been populated
        
        roundCounter_ = 0;
        Game::Instance()->State().World().EncoundterCountInc();

        //TODO move this to the adventure stage
        //TODO Encounter will need a function that plays combat music and that takes a bool
        //     for whether or not to play the same music or to pick new music.
        sfml_util::SoundManager::Instance()->MusicStart(sfml_util::music::CombatIntro);

        //TODO This will need to be triggered by the Adventure stage and moved to it's own
        //     object responsible for creating random sets of creatures for each fight.
        if (Game::Instance()->State().IsNewGame())
        {
            GenerateFirstEncounter();
        }

        PopulateTurnInfoMap();
        SortAndSetTurnCreature();
        hasStarted_ = true;
    }


    void Encounter::EndCombatTasks()
    {
        //At the end of this function:
        //  - runawayPlayersVec_ should be empty
        //  - nonPlayerPartyUPtr_ should be emptied into deadNonPlayerPartyUPtr_
        //  - deadNonPlayerPartyUPtr_ owns all enemy pointers for looting in Treasure Stage
        //  - runawayNonPlayerPartyUPtr_ should be empty, having all it's pointers
        //    free'd because they will not contribute to looting during Treasure Stage.
        //  - So all should be empty except deadNonPlayerPartyUPtr_ unless all enemies ran away.
        
        //Move all remaining enemy creatures (stoned, incapacitated, etc) in the party to
        //deadNonPlayerPartyUPtr_ so that they are all available for looting in Treasure Stage.
        auto const ENEMY_CHARATER_PVEC{ nonPlayerPartyUPtr_->Characters() };
        for (auto const NEXT_ENEMY_CHARACTER_PTR : ENEMY_CHARATER_PVEC)
        {
            HandleKilledCreature(NEXT_ENEMY_CHARACTER_PTR);
        }

        //null runaway player character pointer copies
        runawayPlayersVec_.clear();

        //Free runaway non-player character pointers so they will NOT
        //contribute to looting in Treasure Stage.
        FreeThenResetRunawayNonPlayerParty();
        
        turnCreaturePtr_ = nullptr;
        turnOverPVec_.clear();
        turnInfoMap_.clear();
        roundCounter_ = 0;
        hasStarted_ = false;
        turnIndex_ = 0;
    }


    void Encounter::BeginTreasureStageTasks()
    {
        //At the end of this function:
        //  - All (non-bodypart) item pointers in deadNonPlayerPartyUPtr_ will be moved
        //    to deadNonPlayerItemsHeld_.
        //
        //  - All generated item pointers for the treasure owned by deadNonPlayerPartyUPtr_
        //    will be in deadNonPlayerItemsCached_.
        //

        //move non-bodypart item pointers into deadNonPlayerItemsHeld_
        auto const DEAD_NONPLAYER_CHARACTER_PVEC{ deadNonPlayerPartyUPtr_->Characters() };
        for (auto const NEXT_CHARACTER_PTR : DEAD_NONPLAYER_CHARACTER_PVEC)
        {
            //unequip non-bodypart items
            auto const ITEMS_EQUIPPED_PVEC{ NEXT_CHARACTER_PTR->Inventory().ItemsEquipped() };
            for (auto const NEXT_EQUIPPED_ITEM_PTR : ITEMS_EQUIPPED_PVEC)
            {
                if (NEXT_EQUIPPED_ITEM_PTR->IsBodypart() == false)
                {
                    NEXT_CHARACTER_PTR->ItemUnEquip(NEXT_EQUIPPED_ITEM_PTR);
                }
            }

            //move non-bodypart item pointers into deadNonPlayerItemsHeld_
            auto const ITEMS_UNEQUIPPED_PVEC{ NEXT_CHARACTER_PTR->Inventory().Items() };
            for (auto const NEXT_UNEQUIPPED_ITEM_PTR : ITEMS_UNEQUIPPED_PVEC)
            {
                if (NEXT_UNEQUIPPED_ITEM_PTR->IsBodypart() == false)
                {
                    NEXT_CHARACTER_PTR->ItemRemove(NEXT_UNEQUIPPED_ITEM_PTR);
                    deadNonPlayerItemsHeld_.items_pvec.push_back(NEXT_UNEQUIPPED_ITEM_PTR);
                }
            }
        }

        //create cached treasure
        TreasureFactory::Make(deadNonPlayerPartyUPtr_->Characters(), deadNonPlayerItemsCached_);
    }


    void Encounter::EndTreasureStageTasks()
    {
        //At the end of this function:
        //  - All item pointers in deadNonPlayerItemsHeld_ and deadNonPlayerItemsCached_
        //    will be free'd by the ItemWarehouse.
        //  - Everything in deadNonPlayerItemsHeld_ and deadNonPlayerItemsCached_ will be
        //    zero'd.
        //  - deadNonPlayerPartyUPtr_ will be empty and all it's held pointers free'd.
        //

        for (auto nextItemPtr : deadNonPlayerItemsHeld_.items_pvec)
        {
            item::ItemWarehouse::Instance()->Free(nextItemPtr);
        }
        
        deadNonPlayerItemsHeld_.items_pvec.clear();
        deadNonPlayerItemsHeld_.coins = 0;
        deadNonPlayerItemsHeld_.gems = 0;
        deadNonPlayerItemsHeld_.meteor_shards = 0;

        for (auto nextItemPtr : deadNonPlayerItemsCached_.items_pvec)
        {
            item::ItemWarehouse::Instance()->Free(nextItemPtr);
        }

        deadNonPlayerItemsCached_.items_pvec.clear();
        deadNonPlayerItemsCached_.coins = 0;
        deadNonPlayerItemsCached_.gems = 0;
        deadNonPlayerItemsCached_.meteor_shards = 0;

        FreeThenResetDeadNonPlayerParty();
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

        //move from the enemyParty to the deadNonPlayerParty for later use by the Treasure Stage
        if (CREATURE_CPTRC->IsPlayerCharacter() == false)
        {
            auto killedCharacterPtr{ nonPlayerPartyUPtr_->FindByCreaturePtr(CREATURE_CPTRC) };
            nonPlayerPartyUPtr_->Remove(killedCharacterPtr, false);
            deadNonPlayerPartyUPtr_->Add(killedCharacterPtr, false);
        }
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

        for (auto const NEXT_CHAR_PTR : nonPlayerPartyUPtr_->Characters())
        {
            //enemy creatures need a real populated strategy info object
            TurnInfo turnInfo;
            turnInfo.SetStrategyInfo( strategy::ChanceFactory::Instance()->Get(
                NEXT_CHAR_PTR->Race(), NEXT_CHAR_PTR->Role()).Make() );

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
                      { return A->Speed() > B->Speed(); });
        }

        turnCreaturePtr_ = creaturesThatHaveNotTakenTurnYetPVec.at(0);

        M_ASSERT_OR_LOGANDTHROW_SS((turnCreaturePtr_ != nullptr),
            "game::combat::Encounter::SortAndSetTurnCreature("
            << ((turnCreaturePtr_ == nullptr) ? "nullptr" : turnCreaturePtr_->Name())
            << ") resulted in a nullptr turnCreaturePtr_.");
    }


    void Encounter::FreeThenResetLivingNonPlayerParty()
    {
        FreeThenReset(nonPlayerPartyUPtr_);
    }


    void Encounter::FreeThenResetDeadNonPlayerParty()
    {
        FreeThenReset(deadNonPlayerPartyUPtr_);
    }


    void Encounter::FreeThenResetRunawayNonPlayerParty()
    {
        FreeThenReset(runawayNonPlayerPartyUPtr_);
    }


    void Encounter::FreeThenReset(non_player::PartyUPtr_t & nonPlayerPartyUPtr)
    {
        if (nonPlayerPartyUPtr.get() != nullptr)
        {
            auto const CHARATER_PVEC{ nonPlayerPartyUPtr->Characters() };
            for (auto const NEXT_CHARACTER_PTR : CHARATER_PVEC)
            {
                nonPlayerPartyUPtr->Remove(NEXT_CHARACTER_PTR, true);
            }
        }

        nonPlayerPartyUPtr = std::make_unique<non_player::Party>();
    }

}
}
