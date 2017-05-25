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
#ifndef GAME_COMBAT_ENCOUNTER_INCLUDED
#define GAME_COMBAT_ENCOUNTER_INCLUDED
//
// encounter.hpp
//  Code that orchestrates an encounter from start to finish, and owns the pointers to the non-player party.
//
#include "game/combat/turn-action-enum.hpp"
#include "game/combat/turn-info.hpp"

#include <memory>
#include <vector>


namespace game
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
    using PartySPtr_t = std::shared_ptr<Party>;
}
namespace combat
{

    //types required by singleton implementation
    class Encounter;
    using EncounterSPtr_t = std::shared_ptr<Encounter>;


    //Manages an encounter with the player party
    class Encounter
    {
        //prevent copy construction
        Encounter(const Encounter &) =delete;

        //prevent copy assignment
        Encounter & operator=(const Encounter &) =delete;

        //prevent non-singleton construction
        Encounter();

    public:
        virtual ~Encounter();

        static EncounterSPtr_t Instance();

        inline non_player::PartySPtr_t NonPlayerParty() { return enemyPartySPtr_; }
        inline std::size_t GetRoundCount()              { return roundCounter_; }
        inline bool HasStarted() const                  { return hasStarted_; }

        void Setup_First();

        inline creature::CreaturePtr_t CurrentTurnCreature() const { return turnCreaturePtr_; }

        const TurnInfo GetTurnInfoCopy(const creature::CreaturePtrC_t P) const;

        inline void SetTurnInfo(const creature::CreaturePtrC_t P, const TurnInfo & TI)              { turnInfoMap_[P] = TI; }
        inline void SetIsFlying(const creature::CreaturePtrC_t P, const bool B)                     { turnInfoMap_[P].SetIsFlying(B); }
        inline void SetTurnActionInfo(const creature::CreaturePtrC_t P, const TurnActionInfo & TAI) { turnInfoMap_[P].SetTurnActionInfo(TAI); }

        void HandleKilledCreature(creature::CreatureCPtrC_t);
        void IncrementTurn();
        void StartTasks();
        void EndTasks();

    private:
        void GenerateFirstEncounter();
        void PopulateTurnInfoMap();
        void SortAndSetTurnCreature();

    private:
        static EncounterSPtr_t instance_;
        //
        non_player::PartySPtr_t  enemyPartySPtr_;
        non_player::PartySPtr_t  deadEnemyPartySPtr_;
        std::size_t              roundCounter_;
        bool                     hasStarted_;
        creature::CreaturePVec_t turnOverPVec_;
        std::size_t              turnIndex_;
        TurnInfoMap_t            turnInfoMap_;
        creature::CreaturePtr_t  turnCreaturePtr_;
    };

}
}
#endif //GAME_COMBAT_ENCOUNTER_INCLUDED