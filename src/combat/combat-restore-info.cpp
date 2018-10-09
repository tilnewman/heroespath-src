// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
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
        for (const auto & FLYING_CREATURE_PTR : creaturesFlyingPVec_)
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
