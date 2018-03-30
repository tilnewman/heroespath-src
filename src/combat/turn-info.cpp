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
#include "creature/creature.hpp"

#include <tuple>

namespace heroespath
{
namespace combat
{

    TurnInfo::TurnInfo(
        const bool IS_FLYING,
        const TurnAction::Enum TURN_STATE,
        const combat::strategy::Info & STRATEGY_INFO)
        : isFlying_(IS_FLYING)
        , turnActionInfo_(TURN_STATE)
        , strategyInfo_(STRATEGY_INFO)
        , firstAttackedByCreaturePtrOpt_(boost::none)
        , firstHitByCreaturePtrOpt_(boost::none)
        , lastAttackedByCreaturePtrOpt_(boost::none)
        , lastHitByCreaturePtrOpt_(boost::none)
        , firstToMakeMusicPtrOpt_(boost::none)
        , lastToMakeMusicPtrOpt_(boost::none)
        , firstToCastPtrOpt_(boost::none)
        , lastToCastPtrOpt_(boost::none)
        , mostDamagedHealthAbs_(0_health)
        , mostDamagedCreaturePtrOpt_(boost::none)
        , castCount_(0)
        , songCount_(0)
        , wasHitLastTurn_(false)
    {}

    void TurnInfo::UpdateMostDamagedCreature(
        const Health_t DAMAGE_ABS, const creature::CreaturePtr_t CREATURE_PTR)
    {
        if (DAMAGE_ABS > mostDamagedHealthAbs_)
        {
            mostDamagedHealthAbs_ = DAMAGE_ABS;
            mostDamagedCreaturePtrOpt_ = CREATURE_PTR;
        }
    }

    void TurnInfo::RemoveDeadCreatureTasks(const creature::CreaturePtr_t CREATURE_PTR)
    {
        auto const CREATURE_PTR_OPT{ creature::CreaturePtrOpt_t(CREATURE_PTR) };

        if (firstAttackedByCreaturePtrOpt_ == CREATURE_PTR_OPT)
        {
            firstAttackedByCreaturePtrOpt_ = boost::none;
        }

        if (firstHitByCreaturePtrOpt_ == CREATURE_PTR_OPT)
        {
            firstHitByCreaturePtrOpt_ = boost::none;
        }

        if (lastAttackedByCreaturePtrOpt_ == CREATURE_PTR_OPT)
        {
            lastAttackedByCreaturePtrOpt_ = boost::none;
        }

        if (lastHitByCreaturePtrOpt_ == CREATURE_PTR_OPT)
        {
            lastHitByCreaturePtrOpt_ = boost::none;
        }

        if (firstToMakeMusicPtrOpt_ == CREATURE_PTR_OPT)
        {
            firstToMakeMusicPtrOpt_ = boost::none;
        }

        if (lastToMakeMusicPtrOpt_ == CREATURE_PTR_OPT)
        {
            lastToMakeMusicPtrOpt_ = boost::none;
        }

        if (firstToCastPtrOpt_ == CREATURE_PTR_OPT)
        {
            firstToCastPtrOpt_ = boost::none;
        }

        if (lastToCastPtrOpt_ == CREATURE_PTR_OPT)
        {
            lastToCastPtrOpt_ = boost::none;
        }

        if (mostDamagedCreaturePtrOpt_ == CREATURE_PTR_OPT)
        {
            mostDamagedCreaturePtrOpt_ = boost::none;
            mostDamagedHealthAbs_ = 0_health;
        }
    }

    bool operator<(const TurnInfo & L, const TurnInfo & R)
    {
        return std::tie(
                   L.isFlying_,
                   L.turnActionInfo_,
                   L.strategyInfo_,
                   L.firstAttackedByCreaturePtrOpt_,
                   L.firstHitByCreaturePtrOpt_,
                   L.lastAttackedByCreaturePtrOpt_,
                   L.lastHitByCreaturePtrOpt_,
                   L.firstToMakeMusicPtrOpt_,
                   L.lastToMakeMusicPtrOpt_,
                   L.firstToCastPtrOpt_,
                   L.lastToCastPtrOpt_,
                   L.mostDamagedHealthAbs_,
                   L.mostDamagedCreaturePtrOpt_,
                   L.castCount_,
                   L.songCount_,
                   L.wasHitLastTurn_)
            < std::tie(
                   R.isFlying_,
                   R.turnActionInfo_,
                   R.strategyInfo_,
                   R.firstAttackedByCreaturePtrOpt_,
                   R.firstHitByCreaturePtrOpt_,
                   R.lastAttackedByCreaturePtrOpt_,
                   R.lastHitByCreaturePtrOpt_,
                   R.firstToMakeMusicPtrOpt_,
                   R.lastToMakeMusicPtrOpt_,
                   R.firstToCastPtrOpt_,
                   R.lastToCastPtrOpt_,
                   R.mostDamagedHealthAbs_,
                   R.mostDamagedCreaturePtrOpt_,
                   R.castCount_,
                   R.songCount_,
                   R.wasHitLastTurn_);
    }

    bool operator==(const TurnInfo & L, const TurnInfo & R)
    {
        return std::tie(
                   L.isFlying_,
                   L.turnActionInfo_,
                   L.strategyInfo_,
                   L.firstAttackedByCreaturePtrOpt_,
                   L.firstHitByCreaturePtrOpt_,
                   L.lastAttackedByCreaturePtrOpt_,
                   L.lastHitByCreaturePtrOpt_,
                   L.firstToMakeMusicPtrOpt_,
                   L.lastToMakeMusicPtrOpt_,
                   L.firstToCastPtrOpt_,
                   L.lastToCastPtrOpt_,
                   L.mostDamagedHealthAbs_,
                   L.mostDamagedCreaturePtrOpt_,
                   L.castCount_,
                   L.songCount_,
                   L.wasHitLastTurn_)
            == std::tie(
                   R.isFlying_,
                   R.turnActionInfo_,
                   R.strategyInfo_,
                   R.firstAttackedByCreaturePtrOpt_,
                   R.firstHitByCreaturePtrOpt_,
                   R.lastAttackedByCreaturePtrOpt_,
                   R.lastHitByCreaturePtrOpt_,
                   R.firstToMakeMusicPtrOpt_,
                   R.lastToMakeMusicPtrOpt_,
                   R.firstToCastPtrOpt_,
                   R.lastToCastPtrOpt_,
                   R.mostDamagedHealthAbs_,
                   R.mostDamagedCreaturePtrOpt_,
                   R.castCount_,
                   R.songCount_,
                   R.wasHitLastTurn_);
    }

} // namespace combat
} // namespace heroespath
