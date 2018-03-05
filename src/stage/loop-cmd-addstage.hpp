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
#ifndef HEROESPATH_GUIDEMO_LOOPCOMMANDADDSTAGE_HPP_INCLUDED
#define HEROESPATH_GUIDEMO_LOOPCOMMANDADDSTAGE_HPP_INCLUDED
//
// loop-cmd-addstage.hpp
//  A common header for all loop-add-stage commands.
//
#include "sfml-util/loop-cmd.hpp"

#include "game/phase-enum.hpp"
#include "stage/adventure-stage.hpp"
#include "stage/camp-stage.hpp"
#include "stage/character-stage.hpp"
#include "stage/combat-stage.hpp"
#include "stage/credits-stage.hpp"
#include "stage/inn-stage.hpp"
#include "stage/intro-stage.hpp"
#include "stage/inventory-stage.hpp"
#include "stage/load-game-menu-stage.hpp"
#include "stage/main-menu-stage.hpp"
#include "stage/party-stage.hpp"
#include "stage/settings-stage.hpp"
#include "stage/testing-stage.hpp"
#include "stage/treasure-stage.hpp"

#include "misc/assertlogandthrow.hpp"

#include <boost/type_index.hpp> //for boost::typeindex::type_id<T>().pretty_name()

#include <memory>
#include <string>

namespace heroespath
{
namespace creature
{
    class Creature;
    using CreaturePtr_t = Creature *;
} // namespace creature
namespace stage
{

    template <typename StageType_t>
    class LoopCmd_AddStage : public sfml_util::LoopCmd
    {
    public:
        LoopCmd_AddStage()
            : LoopCmd(std::string("AddStage_")
                          .append(boost::typeindex::type_id<StageType_t>().pretty_name()))
        {}

        virtual ~LoopCmd_AddStage() = default;

        virtual bool Execute()
        {
            auto stagePtr(new StageType_t());
            stagePtr->Setup();
            game::LoopManager::Instance()->CommandLoopAccess(this).AddStage(stagePtr);
            return true;
        }
    };

    class LoopCmd_AddStage_Combat : public sfml_util::LoopCmd
    {
    public:
        LoopCmd_AddStage_Combat(const LoopCmd_AddStage_Combat &) = delete;
        LoopCmd_AddStage_Combat(const LoopCmd_AddStage_Combat &&) = delete;
        LoopCmd_AddStage_Combat & operator=(const LoopCmd_AddStage_Combat &) = delete;
        LoopCmd_AddStage_Combat & operator=(const LoopCmd_AddStage_Combat &&) = delete;

    public:
        explicit LoopCmd_AddStage_Combat(const bool WILL_ADVANCE_TURN)
            : LoopCmd("AddStage_Combat")
            , willAdvanceTurn_(WILL_ADVANCE_TURN)
        {}

        virtual ~LoopCmd_AddStage_Combat() = default;

        virtual bool Execute()
        {
            auto stagePtr(new stage::CombatStage(willAdvanceTurn_));
            stagePtr->Setup();
            game::LoopManager::Instance()->CommandLoopAccess(this).AddStage(stagePtr);
            return true;
        }

    private:
        bool willAdvanceTurn_;
    };

    class LoopCmd_AddStage_Inventory : public sfml_util::LoopCmd
    {
    public:
        LoopCmd_AddStage_Inventory(const LoopCmd_AddStage_Inventory &) = delete;
        LoopCmd_AddStage_Inventory(const LoopCmd_AddStage_Inventory &&) = delete;
        LoopCmd_AddStage_Inventory & operator=(const LoopCmd_AddStage_Inventory &) = delete;
        LoopCmd_AddStage_Inventory & operator=(const LoopCmd_AddStage_Inventory &&) = delete;

    public:
        LoopCmd_AddStage_Inventory(
            const creature::CreaturePtr_t TURN_CREATURE_PTR,
            const creature::CreaturePtr_t INVENTORY_CREATURE_PTR,
            const game::Phase::Enum CURRENT_PHASE)
            : LoopCmd("AddStage_Inventory")
            , turnCreaturePtr(TURN_CREATURE_PTR)
            , inventoryCreaturePtr(INVENTORY_CREATURE_PTR)
            , currentPhase_(CURRENT_PHASE)
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (turnCreaturePtr != nullptr),
                "stage::LoopCmd_AddStage_Inventory() was given a nullptr TURN_CREATURE_PTR.");

            M_ASSERT_OR_LOGANDTHROW_SS(
                (inventoryCreaturePtr != nullptr),
                "stage::LoopCmd_AddStage_Inventory() was given a nullptr "
                    << "INVENTORY_CREATURE_PTR.");
        }

        virtual ~LoopCmd_AddStage_Inventory() = default;

        virtual bool Execute()
        {
            auto inventoryStagePtr(
                new InventoryStage(turnCreaturePtr, inventoryCreaturePtr, currentPhase_));

            inventoryStagePtr->Setup();
            game::LoopManager::Instance()->CommandLoopAccess(this).AddStage(inventoryStagePtr);
            return true;
        }

    private:
        creature::CreaturePtr_t turnCreaturePtr;
        creature::CreaturePtr_t inventoryCreaturePtr;
        game::Phase::Enum currentPhase_;
    };
} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_GUIDEMO_LOOPCOMMANDADDSTAGE_HPP_INCLUDED
