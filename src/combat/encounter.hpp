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
#ifndef HEROESPATH_COMBAT_ENCOUNTER_HPP_INCLUDED
#define HEROESPATH_COMBAT_ENCOUNTER_HPP_INCLUDED
//
// encounter.hpp
//  Code that orchestrates an encounter from start to finish,
//  and owns the pointers to the non-player party.
//
#include "combat/turn-action-enum.hpp"
#include "combat/turn-info.hpp"
#include "creature/creature-factory.hpp"
#include "item/item-cache.hpp"
#include "item/treasure-image-enum.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"

#include <memory>
#include <vector>

namespace heroespath
{
namespace creature
{
    // forward declarations
    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;
    using CreaturePVec_t = std::vector<CreaturePtr_t>;
} // namespace creature
namespace combat
{

    // Manages an encounter with the player party
    class Encounter
    {
    public:
        Encounter(const Encounter &) = delete;
        Encounter(Encounter &&) = delete;
        Encounter & operator=(const Encounter &) = delete;
        Encounter & operator=(Encounter &&) = delete;

    public:
        Encounter();
        ~Encounter();

        static misc::NotNull<Encounter *> Instance();
        static void Acquire();
        static void Release();

        // the player party CreaturePtr_ts are kept by the state::GameState in a player::Party
        // object, but all of the non-player CreaturePtr_ts are kept here in these vectors
        const creature::CreaturePVec_t & LivingNonPlayers() const { return nonPlayerPartyPVec_; }
        const creature::CreaturePVec_t & DeadNonPlayers() const { return deadNonPlayerPartyPVec_; }
        const creature::CreaturePVec_t & RunawayNonPlayers() const
        {
            return runawayNonPlayerPartyPVec_;
        }
        const creature::CreaturePVec_t & RunawayPlayers() const { return runawayPlayersVec_; }

        const std::string LivingNonPlayersSummary() const
        {
            return CreaturesSummary(nonPlayerPartyPVec_);
        }

        bool IsRunaway(const creature::CreaturePtr_t) const;
        void Runaway(const creature::CreaturePtr_t);

        std::size_t GetRoundCount() { return roundCounter_; }
        bool HasStarted() const { return hasStarted_; }

        const creature::CreaturePtrOpt_t CurrentTurnCreaturePtrOpt() const
        {
            return turnCreaturePtrOpt_;
        }

        const TurnInfo GetTurnInfoCopy(const creature::CreaturePtr_t) const;

        void SetTurnInfo(const creature::CreaturePtr_t CREATURE_PTR, const TurnInfo & TURN_INFO)
        {
            turnInfoMap_[CREATURE_PTR] = TURN_INFO;
        }

        void SetIsFlying(const creature::CreaturePtr_t CREATURE_PTR, const bool IS_FLYING)
        {
            turnInfoMap_[CREATURE_PTR].SetIsFlying(IS_FLYING);
        }

        void SetTurnActionInfo(
            const creature::CreaturePtr_t CREATURE_PTR, const TurnActionInfo & TURN_ACTION_INFO)
        {
            turnInfoMap_[CREATURE_PTR].SetTurnActionInfo(TURN_ACTION_INFO);
        }

        void HandleKilledCreature(const creature::CreaturePtr_t);
        void IncrementTurn();
        void BeginCombatTasks();
        void EndCombatTasks();

        item::TreasureImage::Enum BeginTreasureStageTasks();

        void EndTreasureStageTasks(
            const item::ItemCache & ITEM_CACHE_WORN, const item::ItemCache & ITEM_CACHE_OWNED);

        const item::ItemCache TakeDeadNonPlayerItemsHeldCache();
        const item::ItemCache TakeDeadNonPlayerItemsLockboxCache();

        bool DidAllEnemiesRunAway() const;

        const creature::CreaturePtrOpt_t LockPickCreaturePtrOpt() const
        {
            return lockPickCreaturePtrOpt_;
        }

        void LockPickCreaturePtr(const creature::CreaturePtr_t CREATURE_PTR)
        {
            lockPickCreaturePtrOpt_ = CREATURE_PTR;
        }

        float DefeatedPartyTreasureRatioPer() const;

    private:
        void PopulateTurnInfoMap();
        void SortAndSetTurnCreature();
        const std::string CreaturesSummary(const creature::CreaturePVec_t &) const;

    private:
        static std::unique_ptr<Encounter> instanceUPtr_;

        creature::CreatureFactory creatureFactory_;

        // Non-player creature pointers are owned by these vectors.
        // Each non-player creature pointer must only ever be in one of these vectors at a time.
        //
        // The destructor calls Warehouse::Free() for each of these, but that is only in case
        // the application exits abnormally.  Normally, the correct sequence of functions is called
        // (BeginCombatTasks(), EndCombatTasks(), BeginTreasureStageTasks(),
        // EndTreasureStageTasks()) which will leave these vectors empty.
        creature::CreaturePVec_t nonPlayerPartyPVec_;
        creature::CreaturePVec_t deadNonPlayerPartyPVec_;
        creature::CreaturePVec_t runawayNonPlayerPartyPVec_;

        // copies of player character pointers are stored temporarily in this vector
        creature::CreaturePVec_t runawayPlayersVec_;

        std::size_t roundCounter_;
        bool hasStarted_;
        creature::CreaturePVec_t turnOverPVec_;
        std::size_t turnIndex_;
        TurnInfoMap_t turnInfoMap_;

        // this member always stores a copy, and is never responsible for lifetime
        creature::CreaturePtrOpt_t turnCreaturePtrOpt_;

        // the pointers in these two item caches are observer pointers but this class is responsible
        // for their lifetime, meaning this class is responsible for calling ItemWarehouse::Free()
        // on each.  Normally, the correct sequence of functions is called (BeginCombatTasks(),
        // EndCombatTasks(), BeginTreasureStageTasks(), EndTreasureStageTasks()) which frees and
        // empties these vectors.  If the application exits abnormally, then the destructor will
        // will ensure these are free'd.

        // contains all items the dead enemies were wearing or holding when killed
        item::ItemCache deadNonPlayerItemsHeld_;

        // conatins all the items the dead enemies were holding in the chest/lockbox
        item::ItemCache deadNonPlayerItemsLockbox_;

        // this member always stores a copy, and is never responsible for lifetime
        creature::CreaturePtrOpt_t lockPickCreaturePtrOpt_;
    };

} // namespace combat
} // namespace heroespath

#endif // HEROESPATH_COMBAT_ENCOUNTER_HPP_INCLUDED
