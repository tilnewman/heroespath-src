// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GAME_ACTIVE_STAGES_INCLUDED
#define HEROESPATH_GAME_ACTIVE_STAGES_INCLUDED
//
// active-stages.hpp
//
#include "gui/callback.hpp"
#include "gui/i-entity.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
#include "popup/popup-response.hpp"
#include "stage/i-stage.hpp"
#include "stage/stage-setup-packet.hpp"

#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace heroespath
{

namespace popup
{
    class PopupInfo;
}

namespace game
{

    // Wraps all of the highest-level game objects (Stages, Popups, Focus, etc.) required by
    // Loop::Execution() to run the game.  Except for popup_callback_ptr and focus_entity all
    // pointers are owning pointers who control the lifetime.
    struct ActiveStages
    {
        ActiveStages();

        ActiveStages(const ActiveStages &) = delete;
        ActiveStages(ActiveStages &&) = delete;
        ActiveStages & operator=(const ActiveStages &) = delete;
        ActiveStages & operator=(ActiveStages &&) = delete;

        bool HasPopupStage() const { return !!popupUPtr_; }
        bool HasNonPopupStage() const { return !nonPopupUVec_.empty(); }
        bool HasAnyStages() const { return (HasPopupStage() || HasNonPopupStage()); }

        const std::string AllStageNames() const
        {
            std::ostringstream ss;

            if (popupUPtr_)
            {
                ss << popupUPtr_->GetStageName();
            }

            for (const auto & ISTAGE_UPTR : nonPopupUVec_)
            {
                if (ss.str().empty() == false)
                {
                    ss << ", ";
                }

                ss << ISTAGE_UPTR->GetStageName();
            }

            if (ss.str().empty())
            {
                return "ActiveStages(none)";
            }
            else
            {
                return "ActiveStages(" + ss.str() + ")";
            }
        }

        void SetIsFadingForAllStages(const bool IS_FADING);

        template <typename Lambda_t>
        void ExecuteOnPopupStage(Lambda_t & lambda)
        {
            if (popupUPtr_)
            {
                ExecuteAndHandleFocusChange(popupUPtr_.get(), lambda);
            }
        }

        template <typename Lambda_t>
        void ExecuteOnNonPopupStages(Lambda_t & lambda)
        {
            for (auto & iStageUPtr : nonPopupUVec_)
            {
                if (ExecuteAndHandleFocusChange(iStageUPtr.get(), lambda))
                {
                    break;
                }
            }
        }

        template <typename Lambda_t>
        void ExecuteOnForegroundStages(Lambda_t & lambda)
        {
            if (HasPopupStage())
            {
                ExecuteOnPopupStage(lambda);
            }
            else
            {
                ExecuteOnNonPopupStages(lambda);
            }
        }

        void HandlePopupResponseCallback();
        void ReplaceStages(const stage::SetupPacket & SETUP_PACKET);
        void RemovePopupStage() { popupUPtr_.reset(); }

        void ReplacePopupStage(
            const popup::PopupInfo & POPUP_INFO,
            const gui::PopupCallback_t::IHandlerPtr_t POPUP_HANDLER_PTR);

        void SetPopupResponse(const popup::ResponseTypes::Enum TYPE, const std::size_t SELECTION);

    private:
        // returns true if focus was changed
        template <typename Lambda_t>
        bool ExecuteAndHandleFocusChange(stage::IStagePtr_t iStagePtr, Lambda_t & lambda)
        {
            const gui::IEntityPtrOpt_t NEW_FOCUS_IENTITY_PTR_OPT { lambda(iStagePtr) };

            if (NEW_FOCUS_IENTITY_PTR_OPT)
            {
                M_HP_LOG(
                    "\"" << iStagePtr->GetStageName() << "\" stage changing focus to \""
                         << NEW_FOCUS_IENTITY_PTR_OPT.value()->GetEntityName() << "\"");

                FocusRemove();
                iStagePtr->SetFocus(NEW_FOCUS_IENTITY_PTR_OPT.value());
                return true;
            }
            else
            {
                return false;
            }
        }

        void RemovePopupResponse();
        void RemovePopupAll();
        void FocusRemove();

    private:
        // this vector owns and controls the lifetime of all Stages in the game
        stage::IStageUVec_t nonPopupUVec_;

        // this owns and controls the lifetime of all Popup Stages in the game
        // this pointer will never be added to iStageUPVec_
        stage::IStageUPtr_t popupUPtr_;

        // this is an observer pointer to the Stage that handles all popup callbacks
        gui::PopupCallback_t::IHandlerPtrOpt_t popupCallbackHandlerPtrOpt_;

        std::string popupDescriptionStr_;

        popup::PopupResponse popupResponse_;
    };

} // namespace game
} // namespace heroespath

#endif // HEROESPATH_GAME_ACTIVE_STAGES_INCLUDED
