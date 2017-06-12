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
// loop-cmd-addstage-inventory.cpp
//
#include "loop-cmd-addstage.hpp"

#include "game/stage/inventory-stage.hpp"
#include "misc/assertlogandthrow.hpp"


namespace game
{
namespace stage
{

    LoopCmd_AddStage_Inventory::LoopCmd_AddStage_Inventory(
        sfml_util::ILoopSPtr_t &      loopSPtr,
        const creature::CreaturePtr_t TURN_CREATURE_PTR,
        const creature::CreaturePtr_t INVENTORY_CREATURE_PTR,
        const Phase::Enum             CURRENT_PHASE)
    :
        LoopCmd                ("AddStage_\"Inventory\"", loopSPtr),
        TURN_CREATURE_PTR_     (TURN_CREATURE_PTR),
        INVENTORY_CREATURE_PTR_(INVENTORY_CREATURE_PTR),
        CURRENT_PHASE_         (CURRENT_PHASE)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((TURN_CREATURE_PTR != nullptr),
            "game::stage::LoopCmd_AddStage_Inventory() was given a nullptr TURN_CREATURE_PTR.");

        M_ASSERT_OR_LOGANDTHROW_SS((INVENTORY_CREATURE_PTR != nullptr),
            "game::stage::LoopCmd_AddStage_Inventory() was given a nullptr "
            << "INVENTORY_CREATURE_PTR.");
    }


    LoopCmd_AddStage_Inventory::~LoopCmd_AddStage_Inventory()
    {}


    bool LoopCmd_AddStage_Inventory::Execute()
    {
        auto inventoryStagePtr( new game::stage::InventoryStage(TURN_CREATURE_PTR_,
                                                                INVENTORY_CREATURE_PTR_,
                                                                CURRENT_PHASE_) );

        inventoryStagePtr->Setup();
        iLoopSPtr_->AddStage(inventoryStagePtr);
        return true;
    }

}
}
