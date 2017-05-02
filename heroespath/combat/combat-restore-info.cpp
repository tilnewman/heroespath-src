//
// combat-restore-info.cpp
//
#include "combat-restore-info.hpp"

#include "heroespath/combat/combat-node.hpp"
#include "heroespath/combat/combat-display.hpp"


namespace heroespath
{
namespace combat
{

    RestoreInfo::RestoreInfo()
    :
        canTurnAdvance_        (false),
        hasRestored_           (false),
        creaturesFlyingPVec_   (),
        creatureBlockingPosMap_()
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
        //nothing to do, health is saved in the creature owned by State in a shared_ptr
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
            if (nextCombatNodeCPtr->IsFlying())
                creaturesFlyingPVec_.push_back(nextCombatNodeCPtr->Creature());
    }


    void RestoreInfo::FlyingCreaturesRestore(combat::CombatDisplayPtrC_t combatDisplayPtrC)
    {
        for (auto const nextFlyingCreaturePtr : creaturesFlyingPVec_)
            combatDisplayPtrC->HandleFlyingChange(nextFlyingCreaturePtr, true);
    }


    void RestoreInfo::CreaturePositionsSave(const combat::CombatDisplayPtrC_t COMBAT_DISPLAY_CPTRC)
    {
        creatureBlockingPosMap_.clear();

        combat::CombatNodePVec_t combatNodesPVec;
        COMBAT_DISPLAY_CPTRC->GetCombatNodes(combatNodesPVec);

        for (auto const nextCombatNodePtrC : combatNodesPVec)
        {
            creatureBlockingPosMap_[nextCombatNodePtrC->Creature()] = nextCombatNodePtrC->GetBlockingPos();
        }
    }


    void RestoreInfo::CreaturePositionsRestore(combat::CombatDisplayPtrC_t combatDisplayPtrC)
    {
        combat::CombatNodePVec_t combatNodesPVec;
        combatDisplayPtrC->GetCombatNodes(combatNodesPVec);

        for (auto const NEXT_CREATUREPOS_PAIR : creatureBlockingPosMap_)
            for (auto const nextCombatNodePtrC : combatNodesPVec)
                if (NEXT_CREATUREPOS_PAIR.first == nextCombatNodePtrC->Creature())
                    nextCombatNodePtrC->SetBlockingPos(NEXT_CREATUREPOS_PAIR.second);

        combatDisplayPtrC->CombatTreeObj().ResetAllEdges();
        combatDisplayPtrC->PositionCombatTreeCells(false);
        combatDisplayPtrC->UpdateWhichNodesWillDraw();
    }

}
}
