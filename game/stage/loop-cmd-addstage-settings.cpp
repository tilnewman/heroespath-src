//
// loop-cmd-addstage-settings.cpp
//
#include "loop-cmd-addstage.hpp"
#include "game/stage/settings-stage.hpp"


namespace game
{
namespace stage
{
    LoopCmd_AddStage_Settings::LoopCmd_AddStage_Settings(sfml_util::ILoopSPtr_t & loopSPtr)
    :
        LoopCmd("AddStage_\"Settings\"", loopSPtr)
    {}


    LoopCmd_AddStage_Settings::~LoopCmd_AddStage_Settings()
    {}


    bool LoopCmd_AddStage_Settings::Execute()
    {
        sfml_util::IStageSPtr_t settingsStageSPtr( new game::stage::SettingsStage() );
        settingsStageSPtr->Setup();
        iLoopSPtr_->AddStage(settingsStageSPtr);
        return true;
    }

}
}
