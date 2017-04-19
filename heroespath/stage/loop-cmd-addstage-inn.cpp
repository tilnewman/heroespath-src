//
// loop-cmd-addstage-inn.cpp
//
#include "loop-cmd-addstage.hpp"
#include "heroespath/stage/inn-stage.hpp"


namespace heroespath
{
namespace stage
{

    LoopCmd_AddStage_Inn::LoopCmd_AddStage_Inn(sfml_util::ILoopSPtr_t & loopSPtr)
    :
        LoopCmd("AddStage_\"Inn\"", loopSPtr)
    {}


    LoopCmd_AddStage_Inn::~LoopCmd_AddStage_Inn()
    {}


    bool LoopCmd_AddStage_Inn::Execute()
    {
        sfml_util::IStageSPtr_t innStageSPtr( new heroespath::stage::InnStage() );
        innStageSPtr->Setup();
        iLoopSPtr_->AddStage(innStageSPtr);
        return true;
    }

}
}
