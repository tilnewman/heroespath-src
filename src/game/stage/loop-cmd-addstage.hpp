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

    class LoopCmd_AddStage_Camp : public sfml_util::LoopCmd
    {
    public:
        explicit LoopCmd_AddStage_Camp(sfml_util::ILoopSPtr_t & loopSPtr);
        virtual ~LoopCmd_AddStage_Camp();
        virtual bool Execute();
    };

    using LoopCmd_AddStage_CampSPtr_t = std::shared_ptr<LoopCmd_AddStage_Camp>;



    class LoopCmd_AddStage_Settings : public sfml_util::LoopCmd
    {
    public:
        explicit LoopCmd_AddStage_Settings(sfml_util::ILoopSPtr_t & loopSPtr);
        virtual ~LoopCmd_AddStage_Settings();
        virtual bool Execute();
    };

    using LoopCmd_AddStage_SettingsSPtr_t = std::shared_ptr<LoopCmd_AddStage_Settings>;



    class LoopCmd_AddStage_Party_Creation : public sfml_util::LoopCmd
    {
    public:
        explicit LoopCmd_AddStage_Party_Creation(sfml_util::ILoopSPtr_t & loopSPtr);
        virtual ~LoopCmd_AddStage_Party_Creation();
        virtual bool Execute();
    };

    using LoopCmd_AddStage_Party_CreationSPtr_t =
        std::shared_ptr<LoopCmd_AddStage_Party_Creation>;



    class LoopCmd_AddStage_MainMenu : public sfml_util::LoopCmd
    {
    public:
        explicit LoopCmd_AddStage_MainMenu(sfml_util::ILoopSPtr_t & loopSPtr);
        virtual ~LoopCmd_AddStage_MainMenu();
        virtual bool Execute();
    };

    using LoopCmd_AddStage_MainMenuSPtr_t = std::shared_ptr<LoopCmd_AddStage_MainMenu>;



    class LoopCmd_AddStage_LoadGameMenu : public sfml_util::LoopCmd
    {
    public:
        explicit LoopCmd_AddStage_LoadGameMenu(sfml_util::ILoopSPtr_t & loopSPtr);
        virtual ~LoopCmd_AddStage_LoadGameMenu();
        virtual bool Execute();
    };

    using LoopCmd_AddStage_LoadGameMenuSPtr_t =
        std::shared_ptr<LoopCmd_AddStage_LoadGameMenu>;



    class LoopCmd_AddStage_Intro : public sfml_util::LoopCmd
    {
    public:
        explicit LoopCmd_AddStage_Intro(sfml_util::ILoopSPtr_t & loopSPtr);
        virtual ~LoopCmd_AddStage_Intro();
        virtual bool Execute();
    };

    using LoopCmd_AddStage_IntroSPtr_t = std::shared_ptr<LoopCmd_AddStage_Intro>;



    class LoopCmd_AddStage_Inn : public sfml_util::LoopCmd
    {
    public:
        explicit LoopCmd_AddStage_Inn(sfml_util::ILoopSPtr_t & loopSPtr);
        virtual ~LoopCmd_AddStage_Inn();
        virtual bool Execute();
    };

    using LoopCmd_AddStage_InnSPtr_t = std::shared_ptr<LoopCmd_AddStage_Inn>;



    class LoopCmd_AddStage_Credits : public sfml_util::LoopCmd
    {
    public:
        explicit LoopCmd_AddStage_Credits(sfml_util::ILoopSPtr_t & loopSPtr);
        virtual ~LoopCmd_AddStage_Credits();
        virtual bool Execute();
    };

    using LoopCmd_AddStage_CreditsSPtr_t = std::shared_ptr<LoopCmd_AddStage_Credits>;



    class LoopCmd_AddStage_Character_Creation : public sfml_util::LoopCmd
    {
    public:
        explicit LoopCmd_AddStage_Character_Creation(sfml_util::ILoopSPtr_t & loopSPtr);
        virtual ~LoopCmd_AddStage_Character_Creation();
        virtual bool Execute();
    };

    using LoopCmd_AddStage_Character_CreationSPtr_t =
        std::shared_ptr<LoopCmd_AddStage_Character_Creation>;



    class LoopCmd_AddStage_Combat : public sfml_util::LoopCmd
    {
    public:
        explicit LoopCmd_AddStage_Combat(sfml_util::ILoopSPtr_t & loopSPtr,
                                         const bool               WILL_ADVANCE_TURN);

        virtual ~LoopCmd_AddStage_Combat();
        virtual bool Execute();

    private:
        const bool WILL_ADVANCE_TURN_;
    };

    using LoopCmd_AddStage_CombatSPtr_t = std::shared_ptr<LoopCmd_AddStage_Combat>;


    class LoopCmd_AddStage_Inventory : public sfml_util::LoopCmd
    {
        LoopCmd_AddStage_Inventory(const LoopCmd_AddStage_Inventory &) =delete;
        LoopCmd_AddStage_Inventory & operator=(const LoopCmd_AddStage_Inventory &) =delete;

    public:
        explicit LoopCmd_AddStage_Inventory(sfml_util::ILoopSPtr_t &      loopSPtr,
                                            const creature::CreaturePtr_t TURN_CREATURE_PTR,
                                            const creature::CreaturePtr_t INVENTORY_CREATURE_PTR,
                                            const Phase::Enum             CURRENT_PHASE);

        virtual ~LoopCmd_AddStage_Inventory();
        virtual bool Execute();
    private:
        const creature::CreaturePtr_t TURN_CREATURE_PTR_;
        const creature::CreaturePtr_t INVENTORY_CREATURE_PTR_;
        const Phase::Enum CURRENT_PHASE_;
    };

    using LoopCmd_AddStage_InventorySPtr_t = std::shared_ptr<LoopCmd_AddStage_Inventory>;


    class LoopCmd_AddStage_Test : public sfml_util::LoopCmd
    {
    public:
        explicit LoopCmd_AddStage_Test(sfml_util::ILoopSPtr_t & loopSPtr);
        virtual ~LoopCmd_AddStage_Test();
        virtual bool Execute();
    };

    using LoopCmd_AddStage_TestSPtr_t = std::shared_ptr<LoopCmd_AddStage_Test>;

}
}
#endif //GUIDEMO_LOOPCOMMANDADDSTAGE_INCLUDED
