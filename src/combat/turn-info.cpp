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
// turn-info.cpp
//
#include "turn-info.hpp"

#include <tuple>


namespace heroespath
{
namespace combat
{

    TurnInfo::TurnInfo(const bool                     IS_FLYING,
                       const TurnAction::Enum         TURN_STATE,
                       const combat::strategy::Info & STRATEGY_INFO)
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
        mostDamagePair_            (std::make_pair(0, nullptr)),
        castCount_                 (0),
        songCount_                 (0),
        wasHitLastTurn_            (false)
    {}


    TurnInfo::TurnInfo(const TurnInfo & TI)
    :
        isFlying_       (TI.isFlying_),
        turnActionInfo_ (TI.turnActionInfo_),
        strategyInfo_   (TI.strategyInfo_),

        //The lifetime of these objects is not managed by this class.
        //Usage is short-term observation only, so ptr copying is safe.
        firstAttackedByCreaturePtr_ (TI.firstAttackedByCreaturePtr_),
        firstHitByCreaturePtr_      (TI.firstHitByCreaturePtr_),
        lastAttackedByCreaturePtr_  (TI.lastAttackedByCreaturePtr_ ),
        lastHitByCreaturePtr_       (TI.lastHitByCreaturePtr_),
        firstToMakeMusicPtr_        (TI.firstToMakeMusicPtr_),
        lastToMakeMusicPtr_         (TI.lastToMakeMusicPtr_),
        firstToCastPtr_             (TI.firstToCastPtr_),
        lastToCastPtr_              (TI.lastToCastPtr_),

        mostDamagePair_ (TI.mostDamagePair_),
        castCount_      (TI.castCount_     ),
        songCount_      (TI.songCount_     ),
        wasHitLastTurn_ (TI.wasHitLastTurn_)
    {}


    TurnInfo & TurnInfo::operator=(const TurnInfo & TI)
    {
        if ( & TI != this)
        {
            isFlying_                   = TI.isFlying_;
            turnActionInfo_             = TI.turnActionInfo_;
            strategyInfo_               = TI.strategyInfo_;
            firstAttackedByCreaturePtr_ = TI.firstAttackedByCreaturePtr_;
            firstHitByCreaturePtr_      = TI.firstHitByCreaturePtr_;
            lastAttackedByCreaturePtr_  = TI.lastAttackedByCreaturePtr_ ;
            lastHitByCreaturePtr_       = TI.lastHitByCreaturePtr_;
            firstToMakeMusicPtr_        = TI.firstToMakeMusicPtr_;
            lastToMakeMusicPtr_         = TI.lastToMakeMusicPtr_;
            firstToCastPtr_             = TI.firstToCastPtr_;
            lastToCastPtr_              = TI.lastToCastPtr_;
            mostDamagePair_             = TI.mostDamagePair_;
            castCount_                  = TI.castCount_;
            songCount_                  = TI.songCount_;
            wasHitLastTurn_             = TI.wasHitLastTurn_;
        }

        return * this;
    }


    void TurnInfo::RemoveDeadCreatureTasks(creature::CreatureCPtrC_t CREATURE_CPTRC)
    {
        if (firstAttackedByCreaturePtr_ == CREATURE_CPTRC)
        {
            firstAttackedByCreaturePtr_ = nullptr;
        }

        if (firstHitByCreaturePtr_ == CREATURE_CPTRC)
        {
            firstHitByCreaturePtr_ = nullptr;
        }

        if (lastAttackedByCreaturePtr_ == CREATURE_CPTRC)
        {
            lastAttackedByCreaturePtr_ = nullptr;
        }

        if (lastHitByCreaturePtr_ == CREATURE_CPTRC)
        {
            lastHitByCreaturePtr_ = nullptr;
        }

        if (firstToMakeMusicPtr_ == CREATURE_CPTRC)
        {
            firstToMakeMusicPtr_ = nullptr;
        }

        if (lastToMakeMusicPtr_ == CREATURE_CPTRC)
        {
            lastToMakeMusicPtr_ = nullptr;
        }

        if (firstToCastPtr_ == CREATURE_CPTRC)
        {
            firstToCastPtr_ = nullptr;
        }

        if (lastToCastPtr_ == CREATURE_CPTRC)
        {
            lastToCastPtr_ = nullptr;
        }

        if (mostDamagePair_.second == CREATURE_CPTRC)
        {
            mostDamagePair_ = std::make_pair(0_health, nullptr);
        }
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
                        L.songCount_,
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
                        R.songCount_,
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
                        L.songCount_,
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
                        R.songCount_,
                        R.wasHitLastTurn_);
    }

}
}
