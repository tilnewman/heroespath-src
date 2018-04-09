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

#include "combat/strategy-details.hpp"
#include "creature/algorithms.hpp"
#include "creature/creature-factory.hpp"
#include "creature/creature-warehouse.hpp"
#include "creature/creature.hpp"
#include "game/game.hpp"
#include "game/loop-manager.hpp"
#include "item/item-warehouse.hpp"
#include "item/item.hpp"
#include "item/treasure.hpp"
#include "log/log-macros.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/vectors.hpp"
#include "player/party.hpp"
#include "sfml-util/music-enum.hpp"
#include "sfml-util/sound-manager.hpp"
#include "state/game-state.hpp"
#include "state/world.hpp"

#include <algorithm>
#include <exception>
#include <sstream>

namespace heroespath
{
namespace combat
{

    std::unique_ptr<Encounter> Encounter::instanceUPtr_;

    Encounter::Encounter()
        : creatureFactory_()
        , nonPlayerPartyPVec_()
        , deadNonPlayerPartyPVec_()
        , runawayNonPlayerPartyPVec_()
        , runawayPlayersVec_()
        , roundCounter_(0)
        , hasStarted_(false)
        , turnOverPVec_()
        , turnIndex_(0)
        , turnInfoMap_()
        , turnCreaturePtrOpt_(boost::none)
        , deadNonPlayerItemsHeld_()
        , deadNonPlayerItemsLockbox_()
        , lockPickCreaturePtrOpt_(boost::none)
    {
        M_HP_LOG_DBG("Singleton Construction: Encounter");
    }

    Encounter::~Encounter()
    {
        M_HP_LOG_DBG("Singleton Destruction: Encounter");
        creature::CreatureWarehouse::Access().Free(nonPlayerPartyPVec_);
        creature::CreatureWarehouse::Access().Free(deadNonPlayerPartyPVec_);
        creature::CreatureWarehouse::Access().Free(runawayNonPlayerPartyPVec_);
    }

    misc::NotNull<Encounter *> Encounter::Instance()
    {
        if (!instanceUPtr_)
        {
            M_HP_LOG_ERR("Singleton Instance() before Acquire(): Encounter");
            Acquire();
        }

        return instanceUPtr_.get();
    }

    void Encounter::Acquire()
    {
        if (!instanceUPtr_)
        {
            instanceUPtr_ = std::make_unique<Encounter>();
        }
        else
        {
            M_HP_LOG_ERR("Singleton Acquire() after Construction: Encounter");
        }
    }

    void Encounter::Release()
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            (instanceUPtr_), "combat::Encounter::Release() found instanceUPtr that was null.");

        instanceUPtr_.reset();
    }

    bool Encounter::IsRunaway(const creature::CreaturePtr_t CREATURE_PTR) const
    {
        auto const IS_RUNAWAY_PLAYER{
            std::find(runawayPlayersVec_.begin(), runawayPlayersVec_.end(), CREATURE_PTR)
            != runawayPlayersVec_.end()
        };

        auto const IS_RUNAWAY_NONPLAYER{ std::find(
                                             std::begin(runawayNonPlayerPartyPVec_),
                                             std::end(runawayNonPlayerPartyPVec_),
                                             CREATURE_PTR)
                                         != std::end(runawayNonPlayerPartyPVec_) };

        return (IS_RUNAWAY_PLAYER || IS_RUNAWAY_NONPLAYER);
    }

    void Encounter::Runaway(const creature::CreaturePtr_t CREATURE_PTR)
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            (IsRunaway(CREATURE_PTR) == false),
            "combat::Encounter::IsRunaway() given a creature that already ran away.");

        if (CREATURE_PTR->IsPlayerCharacter())
        {
            runawayPlayersVec_.emplace_back(CREATURE_PTR);
        }
        else
        {
            RemoveCreatureFromPVec(CREATURE_PTR, nonPlayerPartyPVec_);
            runawayNonPlayerPartyPVec_.emplace_back(CREATURE_PTR);
        }
    }

    void Encounter::Setup_First()
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            (nonPlayerPartyPVec_.empty()),
            "combat::Encounter::Setup_First() called when nonPlayerPartyPVec_ was NOT empty.");

        nonPlayerPartyPVec_ = creatureFactory_.Make_FirstEncounter();
    }

    const TurnInfo Encounter::GetTurnInfoCopy(const creature::CreaturePtr_t CREATURE_PTR) const
    {
        auto const FOUND_ITER{ turnInfoMap_.Find(CREATURE_PTR) };

        M_ASSERT_OR_LOGANDTHROW_SS(
            (FOUND_ITER != turnInfoMap_.end()),
            "combat::Encounter::GetTurnInfoCopy(creature={"
                << CREATURE_PTR->ToString() << "}) was not found in the turnInfoMap_.");

        return FOUND_ITER->second;
    }

    void Encounter::HandleKilledCreature(const creature::CreaturePtr_t CREATURE_PTR)
    {
        // make sure no TurnInfo objects refer to a killed creature
        for (auto & nextCreatureTurnInfoPair : turnInfoMap_)
        {
            nextCreatureTurnInfoPair.second.RemoveDeadCreatureTasks(CREATURE_PTR);
        }

        // no need to remove from turnOverPVec_

        // no need to null out turnCreaturePtr_

        // move from the enemyParty to the deadNonPlayerParty for later use by the Treasure Stage
        if (CREATURE_PTR->IsPlayerCharacter() == false)
        {
            RemoveCreatureFromPVec(CREATURE_PTR, nonPlayerPartyPVec_);
            deadNonPlayerPartyPVec_.emplace_back(CREATURE_PTR);
        }
    }

    void Encounter::IncrementTurn()
    {
        ++roundCounter_;

        if (turnCreaturePtrOpt_)
        {
            turnOverPVec_.emplace_back(turnCreaturePtrOpt_.value());
        }

        // re-sort by speed every turn because spells/conditions/etc could have changed
        // creature speed during any given turn
        SortAndSetTurnCreature();
    }

    void Encounter::BeginCombatTasks()
    {
        // nonPlayerPartyPvec_ must be left alone because it will have already been populated
        // Okay, wait, that is the responsibility of the Adventure Stage, and it hasn't been
        // implemented yet... SO TODO This will need to be triggered by the Adventure stage and
        // moved to it's own object responsible for creating random sets of creatures for each
        // fight.
        if (game::Game::Instance()->State().IsNewGame())
        {
            GenerateFirstEncounter();
        }

        // TODO move this to the Adventure Stage as well
        // TODO Encounter will need a function that plays combat music and that takes a bool
        //     for whether or not to play the same music or to pick new music.
        if (game::LoopManager::Instance()->GetPhase() == game::Phase::Combat)
        {
            sfml_util::SoundManager::Instance()->MusicStart(sfml_util::music::CombatIntro);
        }

        M_ASSERT_OR_LOGANDTHROW_SS(
            (nonPlayerPartyPVec_.empty() == false),
            "combat::Encounter::BeginCombatTasks() was called when nonPlayerPartyPvec_ was empty.");

        roundCounter_ = 0;
        game::Game::Instance()->State().GetWorld().EncounterCountInc();

        PopulateTurnInfoMap();
        SortAndSetTurnCreature();
        hasStarted_ = true;
    }

    void Encounter::EndCombatTasks()
    {
        // At the end of this function:
        //  - All turn info is cleared/reset: turnCreaturePtrOpt_, turnOverPVec_, turnInfoMap_
        //  - runawayPlayersVec_ should be empty
        //  - nonPlayerPartyUPtr_ should be emptied into deadNonPlayerPartyUPtr_
        //  - deadNonPlayerPartyUPtr_ retains all enemy CreaturePtr_ts for looting in Treasure Stage
        //  - runawayNonPlayerPartyUPtr_ should be empty, having all it's pointers
        //    free'd because they will not contribute to looting during Treasure Stage and are no
        //    longer needed.
        //  - So all should be empty except deadNonPlayerPartyUPtr_ unless all enemies ran away.

        // Move all remaining enemy creatures (stoned, incapacitated, etc) in the party to
        // deadNonPlayerPartyUPtr_ so that they are all available for looting in Treasure Stage.
        for (auto const & CREATURE_PTR : nonPlayerPartyPVec_)
        {
            RemoveCreatureFromPVec(CREATURE_PTR, nonPlayerPartyPVec_);
            deadNonPlayerPartyPVec_.emplace_back(CREATURE_PTR);
        }

        // Free runaway non-player character pointers so they will NOT
        // contribute to looting in Treasure Stage.
        creature::CreatureWarehouse::Access().Free(runawayNonPlayerPartyPVec_);

        // get rid of runaway player character pointer copies
        runawayPlayersVec_.clear();

        turnCreaturePtrOpt_ = boost::none;
        turnOverPVec_.clear();
        turnInfoMap_.Clear();
        roundCounter_ = 0;
        hasStarted_ = false;
        turnIndex_ = 0;
    }

    item::TreasureImage::Enum Encounter::BeginTreasureStageTasks()
    {
        // At the end of this function:
        //  - All (non-bodypart) item pointers in deadNonPlayerPartyPVec_ will be moved
        //    to deadNonPlayerItemsHeld_.
        //
        //  - All generated item pointers for the treasure owned by deadNonPlayerPartyUPtr_
        //    will be in deadNonPlayerItemsLockbox_.
        //

        // move non-bodypart item pointers into deadNonPlayerItemsHeld_
        for (auto const & CREATURE_PTR : deadNonPlayerPartyPVec_)
        {
            // unequip non-bodypart items
            for (auto const & EQUIPPED_ITEM_PTR : CREATURE_PTR->Inventory().ItemsEquipped())
            {
                if (EQUIPPED_ITEM_PTR->IsBodypart() == false)
                {
                    CREATURE_PTR->ItemUnEquip(EQUIPPED_ITEM_PTR);
                }
            }

            // move non-bodypart item pointers into deadNonPlayerItemsHeld_
            for (auto const & UNEQUIPPED_ITEM_PTR : CREATURE_PTR->Inventory().Items())
            {
                if (UNEQUIPPED_ITEM_PTR->IsBodypart() == false)
                {
                    CREATURE_PTR->ItemRemove(UNEQUIPPED_ITEM_PTR);
                    deadNonPlayerItemsHeld_.items_pvec.emplace_back(UNEQUIPPED_ITEM_PTR);
                }
            }
        }

        return item::TreasureFactory::Make(deadNonPlayerPartyPVec_, deadNonPlayerItemsLockbox_);
    }

    void Encounter::EndTreasureStageTasks(
        const item::ItemCache & ITEM_CACHE_WORN, const item::ItemCache & ITEM_CACHE_OWNED)
    {
        deadNonPlayerItemsHeld_ = ITEM_CACHE_WORN;
        deadNonPlayerItemsLockbox_ = ITEM_CACHE_OWNED;

        // At the end of this function:
        //  - All item pointers in deadNonPlayerItemsHeld_ and deadNonPlayerItemsLockbox_
        //    will be free'd by the ItemWarehouse.
        //  - Everything in deadNonPlayerItemsHeld_ and deadNonPlayerItemsLockbox_ will be
        //    zero'd.
        //  - deadNonPlayerPartyPVec_ will be empty and all it's held pointers free'd.
        //
        item::ItemWarehouse::Instance()->Free(deadNonPlayerItemsHeld_.items_pvec);
        deadNonPlayerItemsHeld_.coins = 0_coin;
        deadNonPlayerItemsHeld_.gems = 0_gem;

        item::ItemWarehouse::Instance()->Free(deadNonPlayerItemsLockbox_.items_pvec);
        deadNonPlayerItemsLockbox_.coins = 0_coin;
        deadNonPlayerItemsLockbox_.gems = 0_gem;

        creature::CreatureWarehouse::Access().Free(deadNonPlayerPartyPVec_);
    }

    item::ItemCache Encounter::TakeDeadNonPlayerItemsHeldCache()
    {
        item::ItemCache copy{ deadNonPlayerItemsHeld_ };
        deadNonPlayerItemsHeld_.Reset();
        return copy;
    }

    item::ItemCache Encounter::TakeDeadNonPlayerItemsLockboxCache()
    {
        item::ItemCache copy{ deadNonPlayerItemsLockbox_ };
        deadNonPlayerItemsLockbox_.Reset();
        return copy;
    }

    bool Encounter::DidAllEnemiesRunAway() const
    {
        return ((runawayNonPlayerPartyPVec_.empty() == false) && deadNonPlayerPartyPVec_.empty());
    }

    float Encounter::DefeatedPartyTreasureRatioPer() const
    {
        return item::TreasureFactory::TreasureRatioPer(deadNonPlayerPartyPVec_);
    }

    void Encounter::GenerateFirstEncounter() { combat::Encounter::Instance()->Setup_First(); }

    void Encounter::PopulateTurnInfoMap()
    {
        turnInfoMap_.Clear();

        for (auto const & CREATURE_PTR : nonPlayerPartyPVec_)
        {
            // enemy creatures need a real populated strategy info object
            TurnInfo turnInfo;

            turnInfo.SetStrategyInfo(strategy::ChanceFactory::Instance()
                                         ->Get(CREATURE_PTR->Race(), CREATURE_PTR->Role())
                                         .Make());

            turnInfoMap_[CREATURE_PTR] = turnInfo;
        }

        for (auto const & CHARACTER_PTR : game::Game::Instance()->State().Party().Characters())
        {
            turnInfoMap_[CHARACTER_PTR] = TurnInfo();
        }
    }

    void Encounter::SortAndSetTurnCreature()
    {
        creature::CreaturePVec_t allLivingCreaturesPVec;

        misc::Vector::Append(
            creature::Algorithms::Players(creature::Algorithms::Living), allLivingCreaturesPVec);

        misc::Vector::Append(
            creature::Algorithms::NonPlayers(creature::Algorithms::Living), allLivingCreaturesPVec);

        if (turnOverPVec_.size() >= allLivingCreaturesPVec.size())
        {
            turnOverPVec_.clear();
        }

        creature::CreaturePVec_t creaturesThatHaveNotTakenTurnYetPVec(
            misc::Vector::Exclude(allLivingCreaturesPVec, turnOverPVec_));

        M_ASSERT_OR_LOGANDTHROW_SS(
            (creaturesThatHaveNotTakenTurnYetPVec.empty() == false),
            "combat::Encounter::SortAndSetTurnCreature("
                << ((turnCreaturePtrOpt_) ? turnCreaturePtrOpt_->Obj().Name() : "nullptr")
                << ") resulted in an empty creaturesThatHaveNotTakenTurnYetPVec.");

        if (creaturesThatHaveNotTakenTurnYetPVec.size() > 1)
        {
            std::sort(
                creaturesThatHaveNotTakenTurnYetPVec.begin(),
                creaturesThatHaveNotTakenTurnYetPVec.end(),
                [](auto const & A, auto const & B) { return A->Speed() > B->Speed(); });
        }

        turnCreaturePtrOpt_ = creaturesThatHaveNotTakenTurnYetPVec.at(0);
    }

    const std::string
        Encounter::CreaturesSummary(const creature::CreaturePVec_t & CREATURES_PVEC) const
    {
        using RaceRolePair_t = std::pair<std::string, std::string>;
        misc::VectorMap<RaceRolePair_t, std::size_t> raceRoleMap;

        // count all race/role combinations
        for (auto const & CREATURE_PTR : CREATURES_PVEC)
        {
            raceRoleMap[std::make_pair(CREATURE_PTR->RaceName(), CREATURE_PTR->RoleName())]++;
        }

        // make a single string summary of all race/role combinations
        std::ostringstream ss;
        for (auto itr(raceRoleMap.begin()); itr != raceRoleMap.end(); ++itr)
        {
            auto const NEXT_RACEROLECOUNT_PAIR{ *itr };

            if (itr != raceRoleMap.begin())
            {
                ss << ", ";

                auto nextItr{ itr };
                std::advance(nextItr, 1);
                if (nextItr == raceRoleMap.end())
                {
                    ss << "and ";
                }
            }

            ss << NEXT_RACEROLECOUNT_PAIR.second << " " << NEXT_RACEROLECOUNT_PAIR.first.first
               << " " << NEXT_RACEROLECOUNT_PAIR.first.second
               << ((NEXT_RACEROLECOUNT_PAIR.second > 1) ? "s" : "");
        }

        return ss.str();
    }

    void Encounter::RemoveCreatureFromPVec(
        const creature::CreaturePtr_t CREATURE_PTR, creature::CreaturePVec_t & creaturePVec)
    {
        creaturePVec.erase(
            std::remove(std::begin(creaturePVec), std::end(creaturePVec), CREATURE_PTR),
            std::end(creaturePVec));
    }

} // namespace combat
} // namespace heroespath
