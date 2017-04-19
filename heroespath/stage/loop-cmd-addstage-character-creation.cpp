//
// loop-cmd-addstage-character-creation.cpp
//
#include "loop-cmd-addstage.hpp"
#include "heroespath/stage/character-stage.hpp"


namespace heroespath
{
namespace stage
{

    LoopCmd_AddStage_Character_Creation::LoopCmd_AddStage_Character_Creation(sfml_util::ILoopSPtr_t & loopSPtr)
    :
        LoopCmd("AddStage_\"Character Creation\"", loopSPtr)
    {}


    LoopCmd_AddStage_Character_Creation::~LoopCmd_AddStage_Character_Creation()
    {}


    bool LoopCmd_AddStage_Character_Creation::Execute()
    {
        sfml_util::IStageSPtr_t characterCreationStageSPtr( new heroespath::stage::CharacterStage() );
        characterCreationStageSPtr->Setup();
        iLoopSPtr_->AddStage(characterCreationStageSPtr);
        return true;
    }

}
}
