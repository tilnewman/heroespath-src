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
#ifndef GUIDEMO_LOOPCOMMANDADDSTAGE_INCLUDED
#define GUIDEMO_LOOPCOMMANDADDSTAGE_INCLUDED
//
// loop-cmd-addstage.hpp
//  A common header for all loop-add-stage commands.
//
#include "sfml-util/loop-cmd.hpp"

#include "game/phase-enum.hpp"
#include "game/stage/adventure-stage.hpp"
#include "game/stage/treasure-stage.hpp"
#include "game/stage/camp-stage.hpp"
#include "game/stage/settings-stage.hpp"
#include "game/stage/party-stage.hpp"
#include "game/stage/main-menu-stage.hpp"
#include "game/stage/load-game-menu-stage.hpp"
#include "game/stage/intro-stage.hpp"
#include "game/stage/inn-stage.hpp"
#include "game/stage/credits-stage.hpp"
#include "game/stage/character-stage.hpp"
#include "game/stage/combat-stage.hpp"
#include "game/stage/testing-stage.hpp"
#include "game/stage/inventory-stage.hpp"

#include "misc/assertlogandthrow.hpp"

#include <boost/type_index.hpp>//for boost::typeindex::type_id<T>().pretty_name()

#include <memory>


namespace game
{
namespace creature
{
    class Creature;
    using CreaturePtr_t = Creature *;
}
namespace stage
{

    template<typename StageType_t>
    class LoopCmd_AddStage : public sfml_util::LoopCmd
    {
    public:
        explicit LoopCmd_AddStage()
        :
            LoopCmd(std::string("AddStage_").
                append(boost::typeindex::type_id<StageType_t>().pretty_name()))
        {}

        virtual ~LoopCmd_AddStage() {}

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
        LoopCmd_AddStage_Combat(const LoopCmd_AddStage_Combat &) = delete;
        LoopCmd_AddStage_Combat & operator=(const LoopCmd_AddStage_Combat &) = delete;

    public:
        LoopCmd_AddStage_Combat(const bool WILL_ADVANCE_TURN)
        :
            LoopCmd("AddStage_Combat"),
            willAdvanceTurn_(WILL_ADVANCE_TURN)
        {}

        virtual ~LoopCmd_AddStage_Combat() {}

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


    using LoopCmd_AddStage_CombatSPtr_t =
        std::shared_ptr<LoopCmd_AddStage_Combat>;


    class LoopCmd_AddStage_Inventory : public sfml_util::LoopCmd
    {
        LoopCmd_AddStage_Inventory(const LoopCmd_AddStage_Inventory &) = delete;
        LoopCmd_AddStage_Inventory & operator=(const LoopCmd_AddStage_Inventory &) = delete;

    public:
        LoopCmd_AddStage_Inventory(
            const creature::CreaturePtr_t TURN_CREATURE_PTR,
            const creature::CreaturePtr_t INVENTORY_CREATURE_PTR,
            const Phase::Enum             CURRENT_PHASE)
        :
            LoopCmd("AddStage_Inventory"),
            turnCreaturePtr(TURN_CREATURE_PTR),
            inventoryCreaturePtr(INVENTORY_CREATURE_PTR),
            currentPhase_(CURRENT_PHASE)
        {
            M_ASSERT_OR_LOGANDTHROW_SS((turnCreaturePtr != nullptr),
                "game::stage::LoopCmd_AddStage_Inventory() was given a nullptr TURN_CREATURE_PTR.");

            M_ASSERT_OR_LOGANDTHROW_SS((inventoryCreaturePtr != nullptr),
                "game::stage::LoopCmd_AddStage_Inventory() was given a nullptr "
                << "INVENTORY_CREATURE_PTR.");
        }

        virtual ~LoopCmd_AddStage_Inventory() {}

        virtual bool Execute()
        {
            auto inventoryStagePtr( new InventoryStage(
                turnCreaturePtr,
                inventoryCreaturePtr,
                currentPhase_));

            inventoryStagePtr->Setup();
            game::LoopManager::Instance()->CommandLoopAccess(this).AddStage(inventoryStagePtr);
            return true;
        }

    private:
        creature::CreaturePtr_t turnCreaturePtr;
        creature::CreaturePtr_t inventoryCreaturePtr;
        Phase::Enum currentPhase_;
    };


    using LoopCmd_AddStage_InventorySPtr_t =
        std::shared_ptr<LoopCmd_AddStage_Inventory>;

    using LoopCmd_AddStage_AdentureSPtr_t =
        std::shared_ptr< LoopCmd_AddStage<AdventureStage> >;

    using LoopCmd_AddStage_TreasureSPtr_t =
        std::shared_ptr< LoopCmd_AddStage<TreasureStage> >;

    using LoopCmd_AddStage_CampSPtr_t =
        std::shared_ptr< LoopCmd_AddStage<CampStage> >;

    using LoopCmd_AddStage_SettingsSPtr_t =
        std::shared_ptr< LoopCmd_AddStage<SettingsStage> >;

    using LoopCmd_AddStage_Party_CreationSPtr_t =
        std::shared_ptr< LoopCmd_AddStage<PartyStage> >;

    using LoopCmd_AddStage_MainMenuSPtr_t =
        std::shared_ptr < LoopCmd_AddStage<MainMenuStage> > ;

    using LoopCmd_AddStage_LoadGameMenuSPtr_t =
        std::shared_ptr< LoopCmd_AddStage<LoadGameStage> >;

    using LoopCmd_AddStage_IntroSPtr_t =
        std::shared_ptr< LoopCmd_AddStage<IntroStage> >;

    using LoopCmd_AddStage_InnSPtr_t =
        std::shared_ptr< LoopCmd_AddStage<InnStage> >;

    using LoopCmd_AddStage_CreditsSPtr_t =
        std::shared_ptr< LoopCmd_AddStage<CreditsStage> >;

    using LoopCmd_AddStage_Character_CreationSPtr_t =
        std::shared_ptr< LoopCmd_AddStage<CharacterStage> >;

    using LoopCmd_AddStage_TestSPtr_t =
        std::shared_ptr< LoopCmd_AddStage<TestingStage> >;

}
}
#endif //GUIDEMO_LOOPCOMMANDADDSTAGE_INCLUDED
