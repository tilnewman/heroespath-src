//
// loop-cmd-addstage-test.cpp
//
#include "loop-cmd-addstage.hpp"
#include "game/stage/testing-stage.hpp"


namespace game
{
namespace stage
{

    LoopCmd_AddStage_Test::LoopCmd_AddStage_Test(sfml_util::ILoopSPtr_t & loopSPtr)
    :
        LoopCmd      ("AddStage_\"Test\"", loopSPtr)
    {}


    LoopCmd_AddStage_Test::~LoopCmd_AddStage_Test()
    {}


    bool LoopCmd_AddStage_Test::Execute()
    {
        sfml_util::IStageSPtr_t testStageSPtr( new game::stage::TestingStage() );
        testStageSPtr->Setup();
        iLoopSPtr_->AddStage(testStageSPtr);
        return true;
    }

}
}
