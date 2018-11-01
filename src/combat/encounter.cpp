// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// encounter.cpp
//
#include "encounter.hpp"

#include "creature/algorithms.hpp"
#include "creature/creature-factory.hpp"
#include "creature/creature-warehouse.hpp"
#include "creature/creature.hpp"
#include "creature/player-party.hpp"
#include "game/game-state.hpp"
#include "game/game.hpp"
#include "game/world.hpp"
#include "item/item-warehouse.hpp"
#include "item/item.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/log-macros.hpp"
#include "misc/vectors.hpp"
#include "sfml-util/music-enum.hpp"
#include "sfml-util/sound-manager.hpp"

// TODO remove this include when finished cleaning up, see comments below
#include "game/game-controller.hpp"

#include <algorithm>
#include <exception>
#include <sstream>
#include <utility>

namespace heroespath
{
namespace combat
{

    std::unique_ptr<Encounter> Encounter::instanceUPtr_;

    Encounter::Encounter()
        : nonPlayerPartyPVec_()
        , deadNonPlayerPartyPVec_()
        , runawayNonPlayerPartyPVec_()
        , runawayPlayersVec_()
        , roundCounter_(0)
        , hasStarted_(false)
        , turnOverPVec_()
        , turnIndex_(0)
        , turnInfoMap_()
        , turnCreaturePtrOpt_()
        , deadNonPlayerItemsHeld_()
        , deadNonPlayerItemsLockbox_()
        , lockPickCreaturePtrOpt_()
        , creatureStrategies_()
        , treasureFactory_()
    {
        M_HP_LOG_DBG("Subsystem Construction: Encounter");
    }

    Encounter::~Encounter()
    {
        M_HP_LOG_DBG("Subsystem Destruction: Encounter");

        if (nonPlayerPartyPVec_.empty() == false)
        {
            M_HP_LOG_WRN(
                "combat::Encounter::~Encounter() found nonPlayerPartyPVec_ was not empty.  Size="
                << nonPlayerPartyPVec_.size());

            creature::CreatureWarehouse::Access().Free(nonPlayerPartyPVec_);
        }

        if (deadNonPlayerPartyPVec_.empty() == false)
        {
            M_HP_LOG_WRN(
                "combat::Encounter::~Encounter() found deadNonPlayerPartyPVec_ was not empty.  "
                "Size="
                << deadNonPlayerPartyPVec_.size());

            creature::CreatureWarehouse::Access().Free(deadNonPlayerPartyPVec_);
        }

        if (runawayNonPlayerPartyPVec_.empty() == false)
        {
            M_HP_LOG_WRN(
                "combat::Encounter::~Encounter() found runawayNonPlayerPartyPVec_ was not empty.  "
                "Size="
                << runawayNonPlayerPartyPVec_.size());

            creature::CreatureWarehouse::Access().Free(runawayNonPlayerPartyPVec_);
        }

        if (deadNonPlayerItemsHeld_.items_pvec.empty() == false)
        {
            M_HP_LOG_WRN(
                "combat::Encounter::~Encounter() found deadNonPlayerItemsHeld_.items_pvec was not "
                "empty.  Size="
                << deadNonPlayerItemsHeld_.items_pvec.size());

            item::ItemWarehouse::Access().Free(deadNonPlayerItemsHeld_.items_pvec);
        }

        if (deadNonPlayerItemsLockbox_.items_pvec.empty() == false)
        {
            M_HP_LOG_WRN(
                "combat::Encounter::~Encounter() found deadNonPlayerItemsLockbox_.items_pvec was "
                "not empty.  Size="
                << deadNonPlayerItemsLockbox_.items_pvec.size());

            item::ItemWarehouse::Access().Free(deadNonPlayerItemsLockbox_.items_pvec);
        }
    }

    misc::NotNull<Encounter *> Encounter::Instance()
    {
        if (!instanceUPtr_)
        {
            M_HP_LOG_ERR("Subsystem Instance() called but instanceUPtr_ was null: Encounter");
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
            M_HP_LOG_ERR("Subsystem Acquire() after Construction: Encounter");
        }
    }

    void Encounter::Release()
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (instanceUPtr_), "combat::Encounter::Release() found instanceUPtr that was null.");

        instanceUPtr_.reset();
    }

    bool Encounter::IsRunaway(const creature::CreaturePtr_t CREATURE_PTR) const
    {
        const auto IS_RUNAWAY_PLAYER {
            std::find(runawayPlayersVec_.begin(), runawayPlayersVec_.end(), CREATURE_PTR)
            != runawayPlayersVec_.end()
        };

        const auto IS_RUNAWAY_NONPLAYER { std::find(
                                              std::begin(runawayNonPlayerPartyPVec_),
                                              std::end(runawayNonPlayerPartyPVec_),
                                              CREATURE_PTR)
                                          != std::end(runawayNonPlayerPartyPVec_) };

        return (IS_RUNAWAY_PLAYER || IS_RUNAWAY_NONPLAYER);
    }

    void Encounter::Runaway(const creature::CreaturePtr_t CREATURE_PTR)
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (IsRunaway(CREATURE_PTR) == false),
            "combat::Encounter::IsRunaway() given a creature that already ran away.");

        if (CREATURE_PTR->IsPlayerCharacter())
        {
            runawayPlayersVec_.emplace_back(CREATURE_PTR);
        }
        else
        {
            nonPlayerPartyPVec_.erase(
                std::remove(
                    std::begin(nonPlayerPartyPVec_), std::end(nonPlayerPartyPVec_), CREATURE_PTR),
                std::end(nonPlayerPartyPVec_));

            runawayNonPlayerPartyPVec_.emplace_back(CREATURE_PTR);
        }
    }

    const TurnInfo Encounter::GetTurnInfoCopy(const creature::CreaturePtr_t CREATURE_PTR) const
    {
        const auto FOUND_ITER { turnInfoMap_.Find(CREATURE_PTR) };

        M_HP_ASSERT_OR_LOG_AND_THROW(
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
            nonPlayerPartyPVec_.erase(
                std::remove(
                    std::begin(nonPlayerPartyPVec_), std::end(nonPlayerPartyPVec_), CREATURE_PTR),
                std::end(nonPlayerPartyPVec_));

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
        // nonPlayerPartyPvec_ must be left alone because it will have already been populated.
        // Okay, wait, that is the responsibility of the Adventure Stage, and it hasn't been
        // implemented yet... SO TODO This will need to be triggered by the Adventure stage and
        // moved to its own object responsible for creating random sets of creatures for each
        // fight.
        if (game::Game::Instance()->State().IsNewGame())
        {
            creature::CreatureFactory creatureFactory;
            nonPlayerPartyPVec_ = creatureFactory.MakeFirstEncounterEnemies();
        }

        // TODO move this to the Adventure Stage as well
        // TODO Encounter will need a function that plays combat music and that takes a bool
        //     for whether or not to play the same music or to pick new music.
        if (game::GameController::Instance()->GetPhase() == game::Phase::Combat)
        {
            gui::SoundManager::Instance()->MusicStart(gui::music::CombatIntro);
        }

        M_HP_ASSERT_OR_LOG_AND_THROW(
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
        //  - nonPlayerPartyPVec_ should be emptied into deadNonPlayerPartyPVec_
        //  - deadNonPlayerPartyPVec_ retains all enemy CreaturePtr_ts for looting in Treasure Stage
        //  - runawayNonPlayerPartyPVec_ should be empty, having all its pointers
        //    free'd by the CreatureWarehouse because they will not contribute to looting during
        //    Treasure Stage and are no longer needed.
        //  - So all ...PVecs should be empty except deadNonPlayerPartyPVec_ unless all enemies ran
        //  away.

        // Move all remaining enemy creatures (stoned, incapacitated, etc) to
        // deadNonPlayerPartyPVec_ so that they are available for looting in Treasure Stage.
        std::copy(
            std::begin(nonPlayerPartyPVec_),
            std::end(nonPlayerPartyPVec_),
            std::back_inserter(deadNonPlayerPartyPVec_));

        nonPlayerPartyPVec_.clear();

        // Free runaway enemy creature pointers so they will NOT
        // contribute to looting in Treasure Stage.  Note that this Free() call also clears the
        // vector.
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
        for (const auto & CREATURE_PTR : deadNonPlayerPartyPVec_)
        {
            // unequip non-bodypart items
            for (const auto & EQUIPPED_ITEM_PTR : CREATURE_PTR->Inventory().ItemsEquipped())
            {
                if (EQUIPPED_ITEM_PTR->IsBodypart() == false)
                {
                    CREATURE_PTR->ItemUnEquip(EQUIPPED_ITEM_PTR);
                }
            }

            // move non-bodypart item pointers into deadNonPlayerItemsHeld_
            for (const auto & UNEQUIPPED_ITEM_PTR : CREATURE_PTR->Inventory().Items())
            {
                if (UNEQUIPPED_ITEM_PTR->IsBodypart() == false)
                {
                    CREATURE_PTR->ItemRemove(UNEQUIPPED_ITEM_PTR);
                    deadNonPlayerItemsHeld_.items_pvec.emplace_back(UNEQUIPPED_ITEM_PTR);
                }
            }
        }

        return treasureFactory_.Make(deadNonPlayerPartyPVec_, deadNonPlayerItemsLockbox_);
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
        //  - deadNonPlayerPartyPVec_ will be empty and all its held pointers free'd.
        //
        item::ItemWarehouse::Access().Free(deadNonPlayerItemsHeld_.items_pvec);
        deadNonPlayerItemsHeld_.Reset();

        item::ItemWarehouse::Access().Free(deadNonPlayerItemsLockbox_.items_pvec);
        deadNonPlayerItemsLockbox_.Reset();

        creature::CreatureWarehouse::Access().Free(deadNonPlayerPartyPVec_);
    }

    const item::ItemCache Encounter::TakeDeadNonPlayerItemsHeldCache()
    {
        item::ItemCache copy { deadNonPlayerItemsHeld_ };
        deadNonPlayerItemsHeld_.Reset();
        return copy;
    }

    const item::ItemCache Encounter::TakeDeadNonPlayerItemsLockboxCache()
    {
        item::ItemCache copy { deadNonPlayerItemsLockbox_ };
        deadNonPlayerItemsLockbox_.Reset();
        return copy;
    }

    bool Encounter::DidAllEnemiesRunAway() const
    {
        return ((runawayNonPlayerPartyPVec_.empty() == false) && deadNonPlayerPartyPVec_.empty());
    }

    float Encounter::DefeatedPartyTreasureRatioPer() const
    {
        return treasureFactory_.TreasureRatioPer(deadNonPlayerPartyPVec_);
    }

    void Encounter::BeginAndEndFakeCombatForTesting()
    {
        BeginCombatTasks();

        for (const auto & NEXT_CREATURE_PTR : creature::Algorithms::NonPlayers())
        {
            HandleKilledCreature(NEXT_CREATURE_PTR);
        }

        EndCombatTasks();
    }

    void Encounter::PopulateTurnInfoMap()
    {
        turnInfoMap_.Clear();

        for (const auto & CREATURE_PTR : nonPlayerPartyPVec_)
        {
            // enemy creatures need a real populated strategy info object
            TurnInfo turnInfo;

            turnInfo.SetStrategyInfo(
                creatureStrategies_.Get(CREATURE_PTR->Race(), CREATURE_PTR->Role()).Make());

            turnInfoMap_[CREATURE_PTR] = turnInfo;
        }

        for (const auto & CHARACTER_PTR : game::Game::Instance()->State().Party().Characters())
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

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (creaturesThatHaveNotTakenTurnYetPVec.empty() == false),
            "combat::Encounter::SortAndSetTurnCreature("
                << ((turnCreaturePtrOpt_) ? turnCreaturePtrOpt_.value()->Name() : "nullptr")
                << ") resulted in an empty creaturesThatHaveNotTakenTurnYetPVec.");

        if (creaturesThatHaveNotTakenTurnYetPVec.size() > 1)
        {
            std::sort(
                creaturesThatHaveNotTakenTurnYetPVec.begin(),
                creaturesThatHaveNotTakenTurnYetPVec.end(),
                [](const auto & A, const auto & B) { return A->Speed() > B->Speed(); });
        }

        turnCreaturePtrOpt_ = creaturesThatHaveNotTakenTurnYetPVec.at(0);
    }

    const std::string
        Encounter::CreaturesSummary(const creature::CreaturePVec_t & CREATURES_PVEC) const
    {
        using RaceRolePair_t = std::pair<std::string, std::string>;
        misc::VectorMap<RaceRolePair_t, std::size_t> raceRoleMap;

        // count all race/role combinations
        for (const auto & CREATURE_PTR : CREATURES_PVEC)
        {
            raceRoleMap[std::make_pair(CREATURE_PTR->RaceName(), CREATURE_PTR->RoleName())]++;
        }

        // make a single string summary of all race/role combinations
        std::ostringstream ss;
        for (auto itr(raceRoleMap.begin()); itr != raceRoleMap.end(); ++itr)
        {
            const auto NEXT_RACEROLECOUNT_PAIR { *itr };

            if (itr != raceRoleMap.begin())
            {
                ss << ", ";

                auto nextItr { itr };
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

} // namespace combat
} // namespace heroespath
