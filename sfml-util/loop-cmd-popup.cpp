//
// loop-cmd-popup.cpp
//
#include "loop-cmd-popup.hpp"

#include "sfml-util/iloop.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/popup-stage.hpp"
#include "sfml-util/gui/popup-manager.hpp"

#include "heroespath/i-popup-callback.hpp"


namespace sfml_util
{

    LoopCmd_AddStage_Popup::LoopCmd_AddStage_Popup(sfml_util::ILoopSPtr_t &      iloopSPtr,
                                                   const heroespath::PopupInfo & POPUP_INFO)
    :
        LoopCmd    ("AddStage_\"Popup\"", iloopSPtr),
        POPUP_INFO_(POPUP_INFO)
    {}


    LoopCmd_AddStage_Popup::~LoopCmd_AddStage_Popup()
    {}


    bool LoopCmd_AddStage_Popup::Execute()
    {
        sfml_util::IStageSPtr_t popupStageSPtr( sfml_util::gui::PopupManager::Instance()->CreatePopupStage(POPUP_INFO_) );
        iLoopSPtr_->SetPopup(popupStageSPtr);
        return true;
    }


    LoopCmd_RemoveStage_Popup::LoopCmd_RemoveStage_Popup(sfml_util::ILoopSPtr_t & iloopSPtr)
    :
        LoopCmd("RemoveStage_\"Popup\"", iloopSPtr)
    {}


    LoopCmd_RemoveStage_Popup::~LoopCmd_RemoveStage_Popup()
    {}


    bool LoopCmd_RemoveStage_Popup::Execute()
    {
        sfml_util::IStageSPtr_t emptyStageSPtr;
        iLoopSPtr_->SetPopup(emptyStageSPtr);
        return true;
    }

}
