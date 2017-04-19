//
// loop-cmd-addstage-mainmenu.cpp
//
#include "loop-cmd-addstage.hpp"
#include "heroespath/stage/main-menu-stage.hpp"


namespace heroespath
{
namespace stage
{

    LoopCmd_AddStage_MainMenu::LoopCmd_AddStage_MainMenu(sfml_util::ILoopSPtr_t & loopSPtr)
    :
        LoopCmd("AddStage_\"MainMenu\"", loopSPtr)
    {}


    LoopCmd_AddStage_MainMenu::~LoopCmd_AddStage_MainMenu()
    {}


    bool LoopCmd_AddStage_MainMenu::Execute()
    {
        sfml_util::IStageSPtr_t mainMenuStageSPtr( new heroespath::stage::MainMenuStage );
        mainMenuStageSPtr->Setup();
        iLoopSPtr_->AddStage(mainMenuStageSPtr);
        return true;
    }

}
}
