//
// loop-cmd-addstage-party-creation.cpp
//
#include "loop-cmd-addstage.hpp"
#include "heroespath/stage/party-stage.hpp"


namespace heroespath
{
namespace stage
{

    LoopCmd_AddStage_Party_Creation::LoopCmd_AddStage_Party_Creation(sfml_util::ILoopSPtr_t & loopSPtr)
    :
        LoopCmd("AddStage_\"Party Creation\"", loopSPtr)
    {}


    LoopCmd_AddStage_Party_Creation::~LoopCmd_AddStage_Party_Creation()
    {}


    bool LoopCmd_AddStage_Party_Creation::Execute()
    {
        sfml_util::IStageSPtr_t characterCreationStageSPtr( new heroespath::stage::PartyStage() );
        characterCreationStageSPtr->Setup();
        iLoopSPtr_->AddStage(characterCreationStageSPtr);
        return true;
    }

}
}
