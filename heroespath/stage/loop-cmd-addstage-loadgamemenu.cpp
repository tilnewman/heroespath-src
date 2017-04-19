//
// loop-cmd-addstage-loadgamemenu.cpp
//
#include "loop-cmd-addstage.hpp"
#include "heroespath/stage/load-game-menu-stage.hpp"


namespace heroespath
{
namespace stage
{

    LoopCmd_AddStage_LoadGameMenu::LoopCmd_AddStage_LoadGameMenu(sfml_util::ILoopSPtr_t & loopSPtr)
    :
        LoopCmd("AddStage_\"LoadGameMenu\"", loopSPtr)
    {}


    LoopCmd_AddStage_LoadGameMenu::~LoopCmd_AddStage_LoadGameMenu()
    {}


    bool LoopCmd_AddStage_LoadGameMenu::Execute()
    {
        sfml_util::IStageSPtr_t loadGameMenuStageSPtr( new heroespath::stage::LoadGameStage() );
        loadGameMenuStageSPtr->Setup();
        iLoopSPtr_->AddStage(loadGameMenuStageSPtr);
        return true;
    }

}
}
