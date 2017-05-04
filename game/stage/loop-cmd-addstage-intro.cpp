//
// loop-cmd-addstage-intro.cpp
//
#include "loop-cmd-addstage.hpp"
#include "game/stage/intro-stage.hpp"


namespace game
{
namespace stage
{

    LoopCmd_AddStage_Intro::LoopCmd_AddStage_Intro(sfml_util::ILoopSPtr_t & loopSPtr)
    :
        LoopCmd("AddStage_\"Intro\"", loopSPtr)
    {}


    LoopCmd_AddStage_Intro::~LoopCmd_AddStage_Intro()
    {}


    bool LoopCmd_AddStage_Intro::Execute()
    {
        sfml_util::IStageSPtr_t introStageSPtr( new game::stage::IntroStage() );
        introStageSPtr->Setup();
        iLoopSPtr_->AddStage(introStageSPtr);
        return true;
    }

}
}
