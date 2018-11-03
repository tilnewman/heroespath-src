// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// active-stages.cpp
//
#include "active-stages.hpp"

#include "misc/log-macros.hpp"
#include "popup/popup-info.hpp"
#include "stage/stage-factory.hpp"

namespace heroespath
{
namespace game
{

    ActiveStages::ActiveStages()
        : nonPopupUVec_()
        , popupUPtr_()
        , popupCallbackHandlerPtrOpt_(boost::none)
        , popupDescriptionStr_("")
        , popupResponse_()
    {}

    void ActiveStages::SetIsFadingForAllStages(const bool IS_FADING)
    {
        M_HP_LOG_DBG("fade flag being set for all stages to " << std::boolalpha << IS_FADING);

        for (auto & iStageUPtr : nonPopupUVec_)
        {
            iStageUPtr->IsFading(IS_FADING);
        }

        if (popupUPtr_)
        {
            popupUPtr_->IsFading(IS_FADING);
        }
    }

    void ActiveStages::HandlePopupResponseCallback()
    {
        if (!popupCallbackHandlerPtrOpt_ || (popup::ResponseTypes::None == popupResponse_.type))
        {
            return;
        }

        // who knows what a Stage will do when handling a popup response callback, so use a copy
        // of the popupResponse_ to avoid passing a pointer/reference to GameController's
        // ActiveStages object (context_)
        const auto popupResponseCopy { popupResponse_ };
        popupCallbackHandlerPtrOpt_.value()->HandleCallback(misc::MakeNotNull(&popupResponseCopy));

        RemovePopupResponse();
    }

    void ActiveStages::ReplaceStages(const stage::SetupPacket & SETUP_PACKET)
    {
        nonPopupUVec_.clear();
        nonPopupUVec_ = stage::StageFactory::Make(SETUP_PACKET);
    }

    void ActiveStages::ReplacePopupStage(
        const popup::PopupInfo & POPUP_INFO,
        const gui::PopupCallback_t::IHandlerPtr_t POPUP_HANDLER_PTR)
    {
        RemovePopupAll();
        popupResponse_.Reset(POPUP_INFO.Name());
        popupDescriptionStr_ = POPUP_INFO.ToStringShort();
        popupCallbackHandlerPtrOpt_ = POPUP_HANDLER_PTR;
        popupUPtr_ = stage::StageFactory::MakePopup(POPUP_INFO);
    }

    void ActiveStages::SetPopupResponse(
        const popup::ResponseTypes::Enum TYPE, const std::size_t SELECTION)
    {
        popupResponse_.type = TYPE;
        popupResponse_.selection_opt = SELECTION;
    }

    void ActiveStages::RemovePopupResponse()
    {
        popupResponse_.Reset();
        popupDescriptionStr_.clear();
        popupCallbackHandlerPtrOpt_ = boost::none;
    }

    void ActiveStages::RemovePopupAll()
    {
        RemovePopupStage();
        RemovePopupResponse();
    }

    void ActiveStages::FocusRemove()
    {
        for (auto & iStageUPtr : nonPopupUVec_)
        {
            iStageUPtr->RemoveFocus();
        }

        if (popupUPtr_)
        {
            popupUPtr_->RemoveFocus();
        }
    }

} // namespace game
} // namespace heroespath
