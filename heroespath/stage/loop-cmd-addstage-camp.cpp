//
// loop-cmd-addstage-camp.cpp
//
#include "loop-cmd-addstage.hpp"
#include "heroespath/stage/camp-stage.hpp"


namespace heroespath
{
namespace stage
{

    LoopCmd_AddStage_Camp::LoopCmd_AddStage_Camp(sfml_util::ILoopSPtr_t & loopSPtr)
    :
        LoopCmd("AddStage_\"Camp\"", loopSPtr)
    {}


    LoopCmd_AddStage_Camp::~LoopCmd_AddStage_Camp()
    {}


    bool LoopCmd_AddStage_Camp::Execute()
    {
        sfml_util::IStageSPtr_t campStageSPtr( new heroespath::stage::CampStage() );
        campStageSPtr->Setup();
        iLoopSPtr_->AddStage(campStageSPtr);
        return true;
    }

}
}
