// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUIDEMO_LOOPCOMMANDADDSTAGE_HPP_INCLUDED
#define HEROESPATH_GUIDEMO_LOOPCOMMANDADDSTAGE_HPP_INCLUDED
//
// loop-cmd-addstage.hpp
//  A common header for all loop-add-stage commands.
//
#include "sfml-util/loop-cmd.hpp"

#include "game/phase-enum.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/not-null.hpp"
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

#include <boost/type_index.hpp>

#include <memory>
#include <string>

namespace heroespath
{
namespace creature
{
    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;
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

        void Execute() override
        {
            const sfml_util::IStagePtr_t ISTAGE_PTR{ new StageType_t() };
            ISTAGE_PTR->Setup();
            game::LoopManager::Instance()->CommandLoopAccess(this).AddStage(ISTAGE_PTR);
        }
    };

    class LoopCmd_AddStage_Combat : public sfml_util::LoopCmd
    {
    public:
        explicit LoopCmd_AddStage_Combat(const bool WILL_ADVANCE_TURN)
            : LoopCmd("AddStage_CombatStage")
            , willAdvanceTurn_(WILL_ADVANCE_TURN)
        {}

        virtual ~LoopCmd_AddStage_Combat() = default;

        void Execute() override
        {
            const sfml_util::IStagePtr_t ISTAGE_PTR(new stage::CombatStage(willAdvanceTurn_));
            ISTAGE_PTR->Setup();
            game::LoopManager::Instance()->CommandLoopAccess(this).AddStage(ISTAGE_PTR);
        }

    private:
        bool willAdvanceTurn_;
    };

    class LoopCmd_AddStage_Inventory : public sfml_util::LoopCmd
    {
    public:
        LoopCmd_AddStage_Inventory(
            const creature::CreaturePtr_t TURN_CREATURE_PTR,
            const creature::CreaturePtr_t INVENTORY_CREATURE_PTR,
            const game::Phase::Enum CURRENT_PHASE)
            : LoopCmd("AddStage_InventoryStage")
            , turnCreaturePtr(TURN_CREATURE_PTR)
            , inventoryCreaturePtr(INVENTORY_CREATURE_PTR)
            , currentPhase_(CURRENT_PHASE)
        {}

        virtual ~LoopCmd_AddStage_Inventory() = default;

        void Execute() override
        {
            const sfml_util::IStagePtr_t ISTAGE_PTR(
                new InventoryStage(turnCreaturePtr, inventoryCreaturePtr, currentPhase_));

            ISTAGE_PTR->Setup();
            game::LoopManager::Instance()->CommandLoopAccess(this).AddStage(ISTAGE_PTR);
        }

    private:
        creature::CreaturePtr_t turnCreaturePtr;
        creature::CreaturePtr_t inventoryCreaturePtr;
        game::Phase::Enum currentPhase_;
    };

} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_GUIDEMO_LOOPCOMMANDADDSTAGE_HPP_INCLUDED
