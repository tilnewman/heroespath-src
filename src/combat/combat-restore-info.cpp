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
// combat-restore-info.cpp
//
#include "combat-restore-info.hpp"

#include "combat/combat-display.hpp"
#include "combat/combat-node.hpp"

namespace heroespath
{
namespace combat
{

    RestoreInfo::RestoreInfo()
        : canTurnAdvance_(false)
        , hasRestored_(false)
        , creaturesFlyingPVec_()
        , creatureBlockingPosMap_()
    {}

    void RestoreInfo::PrepareForStageChange(const combat::CombatDisplayPtr_t COMBAT_DISPLAY_PTR)
    {
        CanTurnAdvance(false);
        Save(COMBAT_DISPLAY_PTR);
    }

    void RestoreInfo::Save(const combat::CombatDisplayPtr_t COMBAT_DISPLAY_PTR)
    {
        CreatureHealthSave(COMBAT_DISPLAY_PTR);
        FlyingCreaturesSave(COMBAT_DISPLAY_PTR);
        CreaturePositionsSave(COMBAT_DISPLAY_PTR);
    }

    void RestoreInfo::Restore(const combat::CombatDisplayPtr_t COMBAT_DISPLAY_PTR)
    {
        hasRestored_ = true;
        CreatureHealthRestore(COMBAT_DISPLAY_PTR);
        FlyingCreaturesRestore(COMBAT_DISPLAY_PTR);
        CreaturePositionsRestore(COMBAT_DISPLAY_PTR);
    }

    void RestoreInfo::CreatureHealthSave(const combat::CombatDisplayPtr_t)
    {
        // nothing to do, health is saved in the creature owned by State in a shared_ptr
    }

    void RestoreInfo::CreatureHealthRestore(const combat::CombatDisplayPtr_t COMBAT_DISPLAY_PTR)
    {
        COMBAT_DISPLAY_PTR->UpdateHealthTasks();
    }

    void RestoreInfo::FlyingCreaturesSave(const combat::CombatDisplayPtr_t COMBAT_DISPLAY_PTR)
    {
        creaturesFlyingPVec_ = COMBAT_DISPLAY_PTR->GetCreaturesFlying();
    }

    void RestoreInfo::FlyingCreaturesRestore(const combat::CombatDisplayPtr_t COMBAT_DISPLAY_PTR)
    {
        for (auto const & FLYING_CREATURE_PTR : creaturesFlyingPVec_)
        {
            COMBAT_DISPLAY_PTR->HandleFlyingChange(FLYING_CREATURE_PTR, true);
        }
    }

    void RestoreInfo::CreaturePositionsSave(const combat::CombatDisplayPtr_t COMBAT_DISPLAY_PTR)
    {
        creatureBlockingPosMap_ = COMBAT_DISPLAY_PTR->MakeCreatureBlockingMap();
    }

    void RestoreInfo::CreaturePositionsRestore(const combat::CombatDisplayPtr_t COMBAT_DISPLAY_PTR)
    {
        COMBAT_DISPLAY_PTR->SetBlockingPositions(creatureBlockingPosMap_);
    }

} // namespace combat
} // namespace heroespath
