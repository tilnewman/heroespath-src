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
#ifndef HEROESPATH_COMBAT_TURNINFO_HPP_INCLUDED
#define HEROESPATH_COMBAT_TURNINFO_HPP_INCLUDED
//
// turn-info.hpp
//
#include "combat/strategy-info.hpp"
#include "combat/turn-action-enum.hpp"
#include "combat/turn-action-info.hpp"
#include "misc/types.hpp"
#include "stats/trait.hpp"

#include <map>
#include <utility>

namespace heroespath
{
namespace creature
{
    // forward declarations
    class Creature;
    using CreaturePtr_t = Creature *;
    using CreatureCPtr_t = const Creature *;
    using CreaturePtrC_t = Creature * const;
    using CreatureCPtrC_t = const Creature * const;
}
namespace combat
{

    // stores the abs of the damage
    using DamageCreaturePair_t = std::pair<Health_t, creature::CreaturePtr_t>;

    // Everything needed to decide what a creature will do during their turn in combat.
    class TurnInfo
    {
    public:
        explicit TurnInfo(
            const bool IS_FLYING = false,
            const TurnAction::Enum TURN_STATE = TurnAction::Count,
            const strategy::Info & STRATEGY_INFO = strategy::Info());

        TurnInfo(const TurnInfo &);
        TurnInfo & operator=(const TurnInfo &);

        inline bool GetIsFlying() const { return isFlying_; }
        inline void SetIsFlying(const bool B) { isFlying_ = B; }

        inline const TurnActionInfo GetTurnActionInfo() const { return turnActionInfo_; }
        inline void SetTurnActionInfo(const TurnActionInfo & TAI) { turnActionInfo_ = TAI; }

        inline strategy::Info GetStrategyInfo() const { return strategyInfo_; }
        inline void SetStrategyInfo(const strategy::Info & SI) { strategyInfo_ = SI; }

        inline creature::CreaturePtr_t GetFirstAttackedByCreature() const
        {
            return firstAttackedByCreaturePtr_;
        }
        inline void SetFirstAttackedByCreature(creature::CreaturePtr_t P)
        {
            firstAttackedByCreaturePtr_ = P;
        }

        inline creature::CreaturePtr_t GetLastAttackedByCreature() const
        {
            return lastAttackedByCreaturePtr_;
        }
        inline void SetLastAttackedByCreature(creature::CreaturePtr_t P)
        {
            lastAttackedByCreaturePtr_ = P;
        }

        inline creature::CreaturePtr_t GetFirstHitByCreature() const
        {
            return firstHitByCreaturePtr_;
        }
        inline void SetFirstHitByCreature(creature::CreaturePtr_t SPTR)
        {
            firstHitByCreaturePtr_ = SPTR;
        }

        inline creature::CreaturePtr_t GetLastHitByCreature() const
        {
            return lastHitByCreaturePtr_;
        }
        inline void SetLastHitByCreature(creature::CreaturePtr_t SPTR)
        {
            lastHitByCreaturePtr_ = SPTR;
        }

        inline creature::CreaturePtr_t GetFirstToMakeMusicCreature() const
        {
            return firstToMakeMusicPtr_;
        }
        inline void SetFirstToMakeMusicCreature(creature::CreaturePtr_t SPTR)
        {
            firstToMakeMusicPtr_ = SPTR;
        }

        inline creature::CreaturePtr_t GetLastToMakeMusicCreature() const
        {
            return lastToMakeMusicPtr_;
        }
        inline void SetLastToMakeMusicCreature(creature::CreaturePtr_t SPTR)
        {
            lastToMakeMusicPtr_ = SPTR;
        }

        inline bool IsFirstAttackedByAlsoFirstHitByCreature() const
        {
            return firstAttackedByCreaturePtr_ == firstHitByCreaturePtr_;
        }
        inline bool IsLastAttackedByAlsoLastHitByCreature() const
        {
            return lastAttackedByCreaturePtr_ == lastHitByCreaturePtr_;
        }

        inline creature::CreaturePtr_t GetFirstToCastCreature() const { return firstToCastPtr_; }
        inline void SetFirstToCastCreature(creature::CreaturePtr_t SPTR) { firstToCastPtr_ = SPTR; }

        inline creature::CreaturePtr_t GetLastToCastCreature() const { return lastToCastPtr_; }
        inline void SetLastToCastCreature(creature::CreaturePtr_t SPTR) { lastToCastPtr_ = SPTR; }

        // stores the abs of the damage
        inline const DamageCreaturePair_t GetMostDamageCreaturePair() const
        {
            return mostDamagePair_;
        }
        inline void SetMostDamageCreaturePair(const DamageCreaturePair_t & P)
        {
            mostDamagePair_ = P;
        }

        inline std::size_t CastCount() const { return castCount_; }
        inline std::size_t CastCountIncrement() { return ++castCount_; }

        inline std::size_t SongCount() const { return songCount_; }
        inline std::size_t SongCountIncrement() { return ++songCount_; }

        inline bool GetWasHitLastTurn() const { return wasHitLastTurn_; }
        inline void SetWasHitLastTurn(const bool B) { wasHitLastTurn_ = B; }

        void RemoveDeadCreatureTasks(creature::CreatureCPtrC_t);

        friend bool operator<(const TurnInfo & L, const TurnInfo & R);
        friend bool operator==(const TurnInfo & L, const TurnInfo & R);

    private:
        bool isFlying_;
        TurnActionInfo turnActionInfo_;
        strategy::Info strategyInfo_;
        creature::CreaturePtr_t firstAttackedByCreaturePtr_;
        creature::CreaturePtr_t firstHitByCreaturePtr_;
        creature::CreaturePtr_t lastAttackedByCreaturePtr_;
        creature::CreaturePtr_t lastHitByCreaturePtr_;
        creature::CreaturePtr_t firstToMakeMusicPtr_;
        creature::CreaturePtr_t lastToMakeMusicPtr_;
        creature::CreaturePtr_t firstToCastPtr_;
        creature::CreaturePtr_t lastToCastPtr_;
        DamageCreaturePair_t mostDamagePair_;
        std::size_t castCount_;
        std::size_t songCount_;
        bool wasHitLastTurn_;
    };

    using TurnInfoMap_t = std::map<creature::CreaturePtr_t, TurnInfo>;

    bool operator<(const TurnInfo & L, const TurnInfo & R);

    bool operator==(const TurnInfo & L, const TurnInfo & R);

    inline bool operator!=(const TurnInfo & L, const TurnInfo & R) { return !(L == R); }
}
}

#endif // HEROESPATH_COMBAT_TURNINFO_HPP_INCLUDED
