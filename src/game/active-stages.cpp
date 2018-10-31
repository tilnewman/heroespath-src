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
        : non_popup_uvec()
        , focus_entity_ptr_opt(boost::none)
        , popup_uptr()
        , popup_callback_handler_ptr_opt(boost::none)
        , popup_desc("")
        , popup_response()
    {}

    void ActiveStages::SetIsFadingForAllStages(const bool IS_FADING)
    {
        M_HP_LOG_DBG("fade flag being set for all stages to " << std::boolalpha << IS_FADING);

        for (auto & iStageUPtr : non_popup_uvec)
        {
            iStageUPtr->IsFading(IS_FADING);
        }

        if (popup_uptr)
        {
            popup_uptr->IsFading(IS_FADING);
        }
    }

    void ActiveStages::HandlePopupResponseCallback()
    {
        if (!popup_callback_handler_ptr_opt || (popup::ResponseTypes::None == popup_response.type))
        {
            return;
        }

        // who knows what a Stage will do when handling a popup response callback, so use a copy
        // of the popup_response to avoid passing a pointer/reference to GameController's
        // ActiveStages object (context_)
        const auto POPUP_RESPONSE_COPY { popup_response };
        popup_callback_handler_ptr_opt.value()->HandleCallback(&POPUP_RESPONSE_COPY);

        RemovePopupResponse();
    }

    void ActiveStages::ReplaceStages(const stage::SetupPacket & SETUP_PACKET)
    {
        non_popup_uvec.clear();
        non_popup_uvec = stage::StageFactory::Make(SETUP_PACKET);
    }

    void ActiveStages::ReplacePopupStage(
        const popup::PopupInfo & POPUP_INFO,
        const sfml_util::PopupCallback_t::IHandlerPtr_t POPUP_HANDLER_PTR)
    {
        RemovePopupAll();
        popup_response.Reset(POPUP_INFO.Name());
        popup_desc = POPUP_INFO.ToStringShort();
        popup_callback_handler_ptr_opt = POPUP_HANDLER_PTR;
        popup_uptr = stage::StageFactory::MakePopup(POPUP_INFO);
    }

    void ActiveStages::SetPopupResponse(
        const popup::ResponseTypes::Enum TYPE, const std::size_t SELECTION)
    {
        popup_response.type = TYPE;
        popup_response.selection_opt = SELECTION;
    }

    void ActiveStages::Focus(const sfml_util::IEntityPtr_t & IENTITY_PTR)
    {
        FocusRemove();
        FocusSet(IENTITY_PTR);
    }

    void ActiveStages::RemovePopupResponse()
    {
        popup_response.Reset();
        popup_desc.clear();
        popup_callback_handler_ptr_opt = boost::none;
    }

    void ActiveStages::RemovePopupAll()
    {
        RemovePopupStage();
        RemovePopupResponse();
    }

    void ActiveStages::FocusSet(const sfml_util::IEntityPtr_t IENTITY_PTR)
    {
        focus_entity_ptr_opt = IENTITY_PTR;
        focus_entity_ptr_opt.value()->SetHasFocus(true);

        for (auto & iStageUPtr : non_popup_uvec)
        {
            iStageUPtr->SetFocus(IENTITY_PTR);
        }

        if (popup_uptr)
        {
            popup_uptr->SetFocus(IENTITY_PTR);
        }
    }

    void ActiveStages::FocusRemove()
    {
        focus_entity_ptr_opt = boost::none;

        for (auto & iStageUPtr : non_popup_uvec)
        {
            iStageUPtr->RemoveFocus();
        }

        if (popup_uptr)
        {
            popup_uptr->RemoveFocus();
        }
    }

} // namespace game
} // namespace heroespath
