//
// loop-cmd-addstage-credtis.cpp
//
#include "loop-cmd-addstage.hpp"
#include "heroespath/stage/credits-stage.hpp"


namespace heroespath
{
namespace stage
{

    LoopCmd_AddStage_Credits::LoopCmd_AddStage_Credits(sfml_util::ILoopSPtr_t & loopSPtr)
    :
        LoopCmd("AddStage_\"Credits\"", loopSPtr)
    {}


    LoopCmd_AddStage_Credits::~LoopCmd_AddStage_Credits()
    {}


    bool LoopCmd_AddStage_Credits::Execute()
    {
        sfml_util::IStageSPtr_t creditsStageSPtr( new heroespath::stage::CreditsStage() );
        creditsStageSPtr->Setup();
        iLoopSPtr_->AddStage(creditsStageSPtr);
        return true;
    }

}
}
