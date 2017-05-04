#ifndef GAME_COMBAT_TURNINFO_HPP_INCLUDED
#define GAME_COMBAT_TURNINFO_HPP_INCLUDED
//
// turn-info.hpp
//
#include "game/combat/strategy-info.hpp"
#include "game/combat/turn-action-info.hpp"
#include "game/combat/turn-action-enum.hpp"
#include "game/stats/types.hpp"

#include <map>
#include <utility>


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
}
namespace combat
{

    using DamageCreaturePair_t = std::pair<stats::Health_t, creature::CreaturePtr_t>;


    //Everything needed to decide what a creature will do during their turn in combat.
    class TurnInfo
    {
    public:
        TurnInfo(const bool             IS_FLYING     = false,
                 const TurnAction::Enum TURN_STATE    = TurnAction::Count,
                 const strategy::Info   STRATEGY_INFO = strategy::Info());

        inline bool GetIsFlying() const                                         { return isFlying_; }
        inline void SetIsFlying(const bool B)                                   { isFlying_ = B; }

        inline const TurnActionInfo GetTurnActionInfo() const                   { return turnActionInfo_; }
        inline void SetTurnActionInfo(const TurnActionInfo & TAI)               { turnActionInfo_ = TAI; }

        inline strategy::Info GetStrategyInfo() const                           { return strategyInfo_; }
        inline void SetStrategyInfo(const strategy::Info & SI)                  { strategyInfo_ = SI; }

        inline creature::CreaturePtr_t GetFirstAttackedByCreature() const       { return firstAttackedByCreaturePtr_; }
        inline void SetFirstAttackedByCreature(creature::CreaturePtr_t P )      { firstAttackedByCreaturePtr_ = P; }

        inline creature::CreaturePtr_t GetLastAttackedByCreature() const        { return lastAttackedByCreaturePtr_; }
        inline void SetLastAttackedByCreature(creature::CreaturePtr_t P)        { lastAttackedByCreaturePtr_ = P; }

        inline creature::CreaturePtr_t GetFirstHitByCreature() const            { return firstHitByCreaturePtr_; }
        inline void SetFirstHitByCreature(creature::CreaturePtr_t SPTR)         { firstHitByCreaturePtr_ = SPTR; }

        inline creature::CreaturePtr_t GetLastHitByCreature() const             { return lastHitByCreaturePtr_; }
        inline void SetLastHitByCreature(creature::CreaturePtr_t SPTR)          { lastHitByCreaturePtr_ = SPTR; }

        inline creature::CreaturePtr_t GetFirstToMakeMusicCreature() const      { return firstToMakeMusicPtr_; }
        inline void SetFirstToMakeMusicCreature(creature::CreaturePtr_t SPTR)   { firstToMakeMusicPtr_ = SPTR; }

        inline creature::CreaturePtr_t GetLastToMakeMusicCreature() const       { return lastToMakeMusicPtr_; }
        inline void SetLastToMakeMusicCreature(creature::CreaturePtr_t SPTR)    { lastToMakeMusicPtr_ = SPTR; }

        inline bool IsFirstAttackedByAlsoFirstHitByCreature() const             { return firstAttackedByCreaturePtr_ == firstHitByCreaturePtr_; }
        inline bool IsLastAttackedByAlsoLastHitByCreature() const               { return lastAttackedByCreaturePtr_ == lastHitByCreaturePtr_; }

        inline creature::CreaturePtr_t GetFirstToCastCreature() const           { return firstToCastPtr_; }
        inline void SetFirstToCastCreature(creature::CreaturePtr_t SPTR)        { firstToCastPtr_ = SPTR; }

        inline creature::CreaturePtr_t GetLastToCastCreature() const            { return lastToCastPtr_; }
        inline void SetLastToCastCreature(creature::CreaturePtr_t SPTR)         { lastToCastPtr_ = SPTR; }

        inline const DamageCreaturePair_t GetMostDamageCreature() const         { return mostDamagePair_; }
        inline void SetMostDamageCreature(const DamageCreaturePair_t & P)       { mostDamagePair_ = P; }

        inline std::size_t CastCount() const                                    { return castCount_; }
        inline std::size_t CastCountIncrement()                                 { return ++castCount_; }

        inline bool GetWasHitLastTurn() const                                   { return wasHitLastTurn_; }
        inline void SetWasHitLastTurn(const bool B)                             { wasHitLastTurn_ = B; }

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
        bool wasHitLastTurn_;
    };

    using TurnInfoMap_t = std::map<creature::CreaturePtr_t, TurnInfo>;


    bool operator<(const TurnInfo & L, const TurnInfo & R);

    bool operator==(const TurnInfo & L, const TurnInfo & R);

    inline bool operator!=(const TurnInfo & L, const TurnInfo & R) { return ! (L == R); }

}
}

#endif //GAME_COMBAT_TURNINFO_HPP_INCLUDED
