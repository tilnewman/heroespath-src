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

    LoopCmd_AddStage_Inventory::LoopCmd_AddStage_Inventory(sfml_util::ILoopSPtr_t & loopSPtr, creature::CreaturePtr_t creaturePtr)
    :
        LoopCmd     ("AddStage_\"Inventory\"", loopSPtr),
        creaturePtr_(creaturePtr)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((creaturePtr_ != nullptr), "game::stage::LoopCmd_AddStage_Inventory() was given a nullptr CreaturePtr_t.");
    }


    LoopCmd_AddStage_Inventory::~LoopCmd_AddStage_Inventory()
    {}


    bool LoopCmd_AddStage_Inventory::Execute()
    {
        sfml_util::IStageSPtr_t inventoryStageSPtr( new game::stage::InventoryStage(creaturePtr_) );
        inventoryStageSPtr->Setup();
        iLoopSPtr_->AddStage(inventoryStageSPtr);
        return true;
    }

}
}
