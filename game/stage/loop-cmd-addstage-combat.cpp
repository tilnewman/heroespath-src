//
// loop-cmd-addstage-combat.cpp
//
#include "loop-cmd-addstage.hpp"
#include "game/stage/combat-stage.hpp"


namespace game
{
namespace stage
{

    LoopCmd_AddStage_Combat::LoopCmd_AddStage_Combat(sfml_util::ILoopSPtr_t & loopSPtr)
    :
        LoopCmd("AddStage_\"Combat\"", loopSPtr)
    {}


    LoopCmd_AddStage_Combat::~LoopCmd_AddStage_Combat()
    {}


    bool LoopCmd_AddStage_Combat::Execute()
    {
        sfml_util::IStageSPtr_t combatStageSPtr( new game::stage::CombatStage() );
        combatStageSPtr->Setup();
        iLoopSPtr_->AddStage(combatStageSPtr);
        return true;
    }

}
}