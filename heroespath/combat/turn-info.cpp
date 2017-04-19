//
// turn-info.cpp
//
#include "turn-info.hpp"
#include <tuple>


namespace heroespath
{
namespace combat
{

    TurnInfo::TurnInfo(const bool                   IS_FLYING,
                       const TurnAction::Enum       TURN_STATE,
                       const combat::strategy::Info STRATEGY_INFO)
    :
        isFlying_                  (IS_FLYING),
        turnActionInfo_            (TURN_STATE),
        strategyInfo_              (STRATEGY_INFO),
        firstAttackedByCreaturePtr_(nullptr),
        firstHitByCreaturePtr_     (nullptr),
        lastAttackedByCreaturePtr_ (nullptr),
        lastHitByCreaturePtr_      (nullptr),
        firstToMakeMusicPtr_       (nullptr),
        lastToMakeMusicPtr_        (nullptr),
        firstToCastPtr_            (nullptr),
        lastToCastPtr_             (nullptr),
        mostDamagePair_            (std::make_pair<stats::Health_t, creature::CreaturePtr_t>(0, nullptr)),
        castCount_                 (0),
        wasHitLastTurn_            (false)
    {}


    void TurnInfo::RemoveDeadCreatureTasks(creature::CreatureCPtrC_t CREATURE_CPTRC)
    {
        if (firstAttackedByCreaturePtr_ == CREATURE_CPTRC)
            firstAttackedByCreaturePtr_ = nullptr;

        if (firstHitByCreaturePtr_ == CREATURE_CPTRC)
            firstHitByCreaturePtr_ = nullptr;

        if (lastAttackedByCreaturePtr_ == CREATURE_CPTRC)
            lastAttackedByCreaturePtr_ = nullptr;

        if (lastHitByCreaturePtr_ == CREATURE_CPTRC)
            lastHitByCreaturePtr_ = nullptr;

        if (firstToMakeMusicPtr_ == CREATURE_CPTRC)
            firstToMakeMusicPtr_ = nullptr;

        if (lastToMakeMusicPtr_ == CREATURE_CPTRC)
            lastToMakeMusicPtr_ = nullptr;

        if (firstToCastPtr_ == CREATURE_CPTRC)
            firstToCastPtr_ = nullptr;

        if (lastToCastPtr_ == CREATURE_CPTRC)
            lastToCastPtr_ = nullptr;

        if (mostDamagePair_.second == CREATURE_CPTRC)
            mostDamagePair_ = std::make_pair(0, nullptr);
    }


    bool operator<(const TurnInfo & L, const TurnInfo & R)
    {
        return std::tie(L.isFlying_,
                        L.turnActionInfo_,
                        L.strategyInfo_,
                        L.firstAttackedByCreaturePtr_,
                        L.firstHitByCreaturePtr_,
                        L.lastAttackedByCreaturePtr_,
                        L.lastHitByCreaturePtr_,
                        L.firstToMakeMusicPtr_,
                        L.lastToMakeMusicPtr_,
                        L.firstToCastPtr_,
                        L.lastToCastPtr_,
                        L.mostDamagePair_,
                        L.castCount_,
                        L.wasHitLastTurn_)
               <
               std::tie(R.isFlying_,
                        R.turnActionInfo_,
                        R.strategyInfo_,
                        R.firstAttackedByCreaturePtr_,
                        R.firstHitByCreaturePtr_,
                        R.lastAttackedByCreaturePtr_,
                        R.lastHitByCreaturePtr_,
                        R.firstToMakeMusicPtr_,
                        R.lastToMakeMusicPtr_,
                        R.firstToCastPtr_,
                        R.lastToCastPtr_,
                        R.mostDamagePair_,
                        R.castCount_,
                        R.wasHitLastTurn_);
    }


    bool operator==(const TurnInfo & L, const TurnInfo & R)
    {
        return std::tie(L.isFlying_,
                        L.turnActionInfo_,
                        L.strategyInfo_,
                        L.firstAttackedByCreaturePtr_,
                        L.firstHitByCreaturePtr_,
                        L.lastAttackedByCreaturePtr_,
                        L.lastHitByCreaturePtr_,
                        L.firstToMakeMusicPtr_,
                        L.lastToMakeMusicPtr_,
                        L.firstToCastPtr_,
                        L.lastToCastPtr_,
                        L.mostDamagePair_,
                        L.castCount_,
                        L.wasHitLastTurn_)
               ==
               std::tie(R.isFlying_,
                        R.turnActionInfo_,
                        R.strategyInfo_,
                        R.firstAttackedByCreaturePtr_,
                        R.firstHitByCreaturePtr_,
                        R.lastAttackedByCreaturePtr_,
                        R.lastHitByCreaturePtr_,
                        R.firstToMakeMusicPtr_,
                        R.lastToMakeMusicPtr_,
                        R.firstToCastPtr_,
                        R.lastToCastPtr_,
                        R.mostDamagePair_,
                        R.castCount_,
                        R.wasHitLastTurn_);
    }

}
}
