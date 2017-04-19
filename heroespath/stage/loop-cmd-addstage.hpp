#ifndef APPBASE_GUIDEMO_LOOPCOMMANDADDSTAGE_INCLUDED
#define APPBASE_GUIDEMO_LOOPCOMMANDADDSTAGE_INCLUDED
//
// loop-cmd-addstage.hpp
//  A common header for all loop-add-stage commands.
//
#include "sfml-util/loop-cmd.hpp"

#include <memory>


namespace heroespath
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
        LoopCmd_AddStage_Camp(sfml_util::ILoopSPtr_t & loopSPtr);
        virtual ~LoopCmd_AddStage_Camp();
        virtual bool Execute();
    };

    using LoopCmd_AddStage_CampSPtr_t = std::shared_ptr<LoopCmd_AddStage_Camp>;



    class LoopCmd_AddStage_Settings : public sfml_util::LoopCmd
    {
    public:
        LoopCmd_AddStage_Settings(sfml_util::ILoopSPtr_t & loopSPtr);
        virtual ~LoopCmd_AddStage_Settings();
        virtual bool Execute();
    };

    using LoopCmd_AddStage_SettingsSPtr_t = std::shared_ptr<LoopCmd_AddStage_Settings>;



    class LoopCmd_AddStage_Party_Creation : public sfml_util::LoopCmd
    {
    public:
        LoopCmd_AddStage_Party_Creation(sfml_util::ILoopSPtr_t & loopSPtr);
        virtual ~LoopCmd_AddStage_Party_Creation();
        virtual bool Execute();
    };

    using LoopCmd_AddStage_Party_CreationSPtr_t = std::shared_ptr<LoopCmd_AddStage_Party_Creation>;



    class LoopCmd_AddStage_MainMenu : public sfml_util::LoopCmd
    {
    public:
        LoopCmd_AddStage_MainMenu(sfml_util::ILoopSPtr_t & loopSPtr);
        virtual ~LoopCmd_AddStage_MainMenu();
        virtual bool Execute();
    };

    using LoopCmd_AddStage_MainMenuSPtr_t = std::shared_ptr<LoopCmd_AddStage_MainMenu>;



    class LoopCmd_AddStage_LoadGameMenu : public sfml_util::LoopCmd
    {
    public:
        LoopCmd_AddStage_LoadGameMenu(sfml_util::ILoopSPtr_t & loopSPtr);
        virtual ~LoopCmd_AddStage_LoadGameMenu();
        virtual bool Execute();
    };

    using LoopCmd_AddStage_LoadGameMenuSPtr_t = std::shared_ptr<LoopCmd_AddStage_LoadGameMenu>;



    class LoopCmd_AddStage_Intro : public sfml_util::LoopCmd
    {
    public:
        LoopCmd_AddStage_Intro(sfml_util::ILoopSPtr_t & loopSPtr);
        virtual ~LoopCmd_AddStage_Intro();
        virtual bool Execute();
    };

    using LoopCmd_AddStage_IntroSPtr_t = std::shared_ptr<LoopCmd_AddStage_Intro>;



    class LoopCmd_AddStage_Inn : public sfml_util::LoopCmd
    {
    public:
        LoopCmd_AddStage_Inn(sfml_util::ILoopSPtr_t & loopSPtr);
        virtual ~LoopCmd_AddStage_Inn();
        virtual bool Execute();
    };

    using LoopCmd_AddStage_InnSPtr_t = std::shared_ptr<LoopCmd_AddStage_Inn>;



    class LoopCmd_AddStage_Credits : public sfml_util::LoopCmd
    {
    public:
        LoopCmd_AddStage_Credits(sfml_util::ILoopSPtr_t & loopSPtr);
        virtual ~LoopCmd_AddStage_Credits();
        virtual bool Execute();
    };

    using LoopCmd_AddStage_CreditsSPtr_t = std::shared_ptr<LoopCmd_AddStage_Credits>;



    class LoopCmd_AddStage_Character_Creation : public sfml_util::LoopCmd
    {
    public:
        LoopCmd_AddStage_Character_Creation(sfml_util::ILoopSPtr_t & loopSPtr);
        virtual ~LoopCmd_AddStage_Character_Creation();
        virtual bool Execute();
    };

    using LoopCmd_AddStage_Character_CreationSPtr_t = std::shared_ptr<LoopCmd_AddStage_Character_Creation>;



    class LoopCmd_AddStage_Combat : public sfml_util::LoopCmd
    {
    public:
        LoopCmd_AddStage_Combat(sfml_util::ILoopSPtr_t & loopSPtr);
        virtual ~LoopCmd_AddStage_Combat();
        virtual bool Execute();
    };

    using LoopCmd_AddStage_CombatSPtr_t = std::shared_ptr<LoopCmd_AddStage_Combat>;


    class LoopCmd_AddStage_Inventory : public sfml_util::LoopCmd
    {
    public:
        LoopCmd_AddStage_Inventory(sfml_util::ILoopSPtr_t & loopSPtr, creature::CreaturePtr_t creaturePtr);
        virtual ~LoopCmd_AddStage_Inventory();
        virtual bool Execute();
    private:
        creature::CreaturePtr_t creaturePtr_;
    };

    using LoopCmd_AddStage_InventorySPtr_t = std::shared_ptr<LoopCmd_AddStage_Inventory>;


    class LoopCmd_AddStage_Test : public sfml_util::LoopCmd
    {
    public:
        LoopCmd_AddStage_Test(sfml_util::ILoopSPtr_t & loopSPtr);
        virtual ~LoopCmd_AddStage_Test();
        virtual bool Execute();
    };

    using LoopCmd_AddStage_TestSPtr_t = std::shared_ptr<LoopCmd_AddStage_Test>;

}
}
#endif //APPBASE_GUIDEMO_LOOPCOMMANDADDSTAGE_INCLUDED
