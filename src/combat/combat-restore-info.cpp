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

    void RestoreInfo::PrepareForStageChange(const combat::CombatDisplayPtrC_t COMBAT_DISPLAY_CPTRC)
    {
        CanTurnAdvance(false);
        Save(COMBAT_DISPLAY_CPTRC);
    }

    void RestoreInfo::Save(const combat::CombatDisplayPtrC_t COMBAT_DISPLAY_CPTRC)
    {
        CreatureHealthSave(COMBAT_DISPLAY_CPTRC);
        FlyingCreaturesSave(COMBAT_DISPLAY_CPTRC);
        CreaturePositionsSave(COMBAT_DISPLAY_CPTRC);
    }

    void RestoreInfo::Restore(combat::CombatDisplayPtrC_t combatDisplayPtrC)
    {
        hasRestored_ = true;
        CreatureHealthRestore(combatDisplayPtrC);
        FlyingCreaturesRestore(combatDisplayPtrC);
        CreaturePositionsRestore(combatDisplayPtrC);
    }

    void RestoreInfo::CreatureHealthSave(const combat::CombatDisplayPtrC_t)
    {
        // nothing to do, health is saved in the creature owned by State in a shared_ptr
    }

    void RestoreInfo::CreatureHealthRestore(combat::CombatDisplayPtrC_t combatDisplayPtrC)
    {
        combat::CombatNodePVec_t combatNodesPVec;
        combatDisplayPtrC->GetCombatNodes(combatNodesPVec);

        for (auto const nextCombatNodePtrC : combatNodesPVec)
            nextCombatNodePtrC->HealthChangeTasks();
    }

    void RestoreInfo::FlyingCreaturesSave(const combat::CombatDisplayPtrC_t COMBAT_DISPLAY_CPTRC)
    {
        creaturesFlyingPVec_.clear();

        combat::CombatNodePVec_t combatNodesPVec;
        COMBAT_DISPLAY_CPTRC->GetCombatNodes(combatNodesPVec);

        for (auto const nextCombatNodeCPtr : combatNodesPVec)
        {
            if (nextCombatNodeCPtr->IsFlying())
            {
                creaturesFlyingPVec_.emplace_back(nextCombatNodeCPtr->Creature());
            }
        }
    }

    void RestoreInfo::FlyingCreaturesRestore(combat::CombatDisplayPtrC_t combatDisplayPtrC)
    {
        for (auto const nextFlyingCreaturePtr : creaturesFlyingPVec_)
        {
            combatDisplayPtrC->HandleFlyingChange(nextFlyingCreaturePtr, true);
        }
    }

    void RestoreInfo::CreaturePositionsSave(const combat::CombatDisplayPtrC_t COMBAT_DISPLAY_CPTRC)
    {
        creatureBlockingPosMap_.clear();

        combat::CombatNodePVec_t combatNodesPVec;
        COMBAT_DISPLAY_CPTRC->GetCombatNodes(combatNodesPVec);

        for (auto const nextCombatNodePtrC : combatNodesPVec)
        {
            creatureBlockingPosMap_[nextCombatNodePtrC->Creature()]
                = nextCombatNodePtrC->GetBlockingPos();
        }
    }

    void RestoreInfo::CreaturePositionsRestore(combat::CombatDisplayPtrC_t combatDisplayPtrC)
    {
        combat::CombatNodePVec_t combatNodesPVec;
        combatDisplayPtrC->GetCombatNodes(combatNodesPVec);

        for (auto const & NEXT_CREATUREPOS_PAIR : creatureBlockingPosMap_)
        {
            for (auto const nextCombatNodePtrC : combatNodesPVec)
            {
                if (NEXT_CREATUREPOS_PAIR.first == nextCombatNodePtrC->Creature())
                {
                    nextCombatNodePtrC->SetBlockingPos(NEXT_CREATUREPOS_PAIR.second);
                }
            }
        }

        combatDisplayPtrC->CombatTreeObj().ResetAllEdges();
        combatDisplayPtrC->PositionCombatTreeCells(false);
        combatDisplayPtrC->UpdateWhichNodesWillDraw();
    }
} // namespace combat
} // namespace heroespath
