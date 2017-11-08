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
#include "item/item-cache.hpp"
#include "item/treasure-image-enum.hpp"

#include <memory>
#include <vector>


namespace heroespath
{
namespace creature
{
    //forward declarations
    class Creature;
    using CreaturePtr_t   = Creature *;
    using CreatureCPtr_t  = const Creature *;
    using CreaturePtrC_t  = Creature * const;
    using CreatureCPtrC_t = const Creature * const;
    using CreaturePVec_t  = std::vector<CreaturePtr_t>;
}
namespace player
{
    class Character;
    using CharacterPtrC_t = Character * const;
}
namespace non_player
{
    class Party;
    using PartyPtr_t = Party *;
    using PartyUPtr_t = std::unique_ptr<Party>;
}

namespace combat
{

    //Manages an encounter with the player party
    class Encounter
    {
        Encounter(const Encounter &) =delete;
        Encounter & operator=(const Encounter &) =delete;

    public:
        Encounter();
        ~Encounter();

        static Encounter * Instance();
        static void Acquire();
        static void Release();

        //the player party is kept by the State object
        non_player::Party & LivingNonPlayerParty();
        non_player::Party & DeadNonPlayerParty();
        non_player::Party & RunawayNonPlayerParty();

        inline const creature::CreaturePVec_t & RunawayPlayersVec() const
        {
            return runawayPlayersVec_;
        }

        bool IsRunaway(const creature::CreaturePtr_t) const;
        void Runaway(const creature::CreaturePtr_t);

        inline std::size_t GetRoundCount()  { return roundCounter_; }
        inline bool HasStarted() const      { return hasStarted_; }

        void Setup_First();

        inline creature::CreaturePtr_t CurrentTurnCreature() const
        {
            return turnCreaturePtr_;
        }

        const TurnInfo GetTurnInfoCopy(const creature::CreaturePtrC_t P) const;

        inline void SetTurnInfo(const creature::CreaturePtrC_t P, const TurnInfo & TI)
        {
            turnInfoMap_[P] = TI;
        }

        inline void SetIsFlying(const creature::CreaturePtrC_t P, const bool B)
        {
            turnInfoMap_[P].SetIsFlying(B);
        }

        inline void SetTurnActionInfo(const creature::CreaturePtrC_t P,
                                      const TurnActionInfo &         TAI)
        {
            turnInfoMap_[P].SetTurnActionInfo(TAI);
        }

        void HandleKilledCreature(creature::CreatureCPtrC_t);
        void IncrementTurn();
        void BeginCombatTasks();
        void EndCombatTasks();

        item::TreasureImage::Enum BeginTreasureStageTasks();

        void EndTreasureStageTasks(
            const item::ItemCache & ITEM_CACHE_WORN,
            const item::ItemCache & ITEM_CACHE_OWNED);

        item::ItemCache TakeDeadNonPlayerItemsHeldCache();
        item::ItemCache TakeDeadNonPlayerItemsLockboxCache();

        bool DidAllEnemiesRunAway() const;

        inline creature::CreaturePtr_t LockPickCreaturePtr() const
        {
            return lockPickCreaturePtr_;
        }

        inline void LockPickCreaturePtr(const creature::CreaturePtr_t NEW_CREATURE_PTR)
        {
            lockPickCreaturePtr_ = NEW_CREATURE_PTR;
        }

        float DefeatedPartyTreasureRatioPer() const;

    private:
        void GenerateFirstEncounter();
        void PopulateTurnInfoMap();
        void SortAndSetTurnCreature();

        //These functions are where all non-player charater pointers are free'd
        void FreeThenResetLivingNonPlayerParty();
        void FreeThenResetDeadNonPlayerParty();
        void FreeThenResetRunawayNonPlayerParty();
        void FreeThenReset(non_player::PartyUPtr_t &);

    private:
        static std::unique_ptr<Encounter> instanceUPtr_;

        //Non-player character pointers are owned by these party objects.
        //Each non-player character pointer must only ever be in one of these vecs.
        non_player::PartyUPtr_t nonPlayerPartyUPtr_;
        non_player::PartyUPtr_t deadNonPlayerPartyUPtr_;
        non_player::PartyUPtr_t runawayNonPlayerPartyUPtr_;

        //copies of player character pointers are store temporarily in this vector
        creature::CreaturePVec_t runawayPlayersVec_;

        std::size_t              roundCounter_;
        bool                     hasStarted_;
        creature::CreaturePVec_t turnOverPVec_;
        std::size_t              turnIndex_;
        TurnInfoMap_t            turnInfoMap_;

        //this member always stores a copy, and is never responsible for lifetime
        creature::CreaturePtr_t  turnCreaturePtr_;

        //contains all items the dead enemies were wearing or holding when killed
        item::ItemCache deadNonPlayerItemsHeld_;

        //conatins all the items the dead enemies were holding in the chest/lockbox
        item::ItemCache deadNonPlayerItemsLockbox_;

        //this member always stores a copy, and is never responsible for lifetime
        creature::CreaturePtr_t lockPickCreaturePtr_;
    };

}
}

#endif //HEROESPATH_COMBAT_ENCOUNTER_HPP_INCLUDED
