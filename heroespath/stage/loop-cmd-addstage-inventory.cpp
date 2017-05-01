//
// loop-cmd-addstage-inventory.cpp
//
#include "loop-cmd-addstage.hpp"

#include "heroespath/stage/inventory-stage.hpp"
#include "utilz/assertlogandthrow.hpp"


namespace heroespath
{
namespace stage
{

    LoopCmd_AddStage_Inventory::LoopCmd_AddStage_Inventory(sfml_util::ILoopSPtr_t & loopSPtr, creature::CreaturePtr_t creaturePtr)
    :
        LoopCmd     ("AddStage_\"Inventory\"", loopSPtr),
        creaturePtr_(creaturePtr)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((creaturePtr_ != nullptr), "heroespath::stage::LoopCmd_AddStage_Inventory() was given a nullptr CreaturePtr_t.");
    }


    LoopCmd_AddStage_Inventory::~LoopCmd_AddStage_Inventory()
    {}


    bool LoopCmd_AddStage_Inventory::Execute()
    {
        sfml_util::IStageSPtr_t inventoryStageSPtr( new heroespath::stage::InventoryStage(creaturePtr_) );
        inventoryStageSPtr->Setup();
        iLoopSPtr_->AddStage(inventoryStageSPtr);
        return true;
    }

}
}
