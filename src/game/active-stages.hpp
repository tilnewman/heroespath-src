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
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
#include "popup/popup-response.hpp"
#include "sfml-util/callback.hpp"
#include "sfml-util/i-entity.hpp"
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

    enum class WhichStages
    {
        All,
        Foreground,
        PopupOnly,
        NonPopupOnly
    };

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

        bool HasPopupStage() const { return !!popup_uptr; }
        bool HasNonPopupStage() const { return !non_popup_uvec.empty(); }
        bool HasAnyStages() const { return (HasPopupStage() || HasNonPopupStage()); }

        const std::string AllStageNames() const
        {
            std::ostringstream ss;

            if (popup_uptr)
            {
                ss << popup_uptr->GetStageName();
            }

            for (const auto & ISTAGE_UPTR : non_popup_uvec)
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
        void ExecuteOn(
            const WhichStages WHICH_STAGES, Lambda_t & lambda, const bool NON_POPUP_FIRST = false)
        {
            const auto WILL_EXECUTE_ON_POPUP { (
                HasPopupStage() && (WHICH_STAGES != WhichStages::NonPopupOnly)) };

            const auto POPUP_ONLY_FOREGROUND_RESTRICTION { (
                (WHICH_STAGES == WhichStages::Foreground) && HasPopupStage()) };

            const auto WILL_EXECUTE_ON_NON_POPUPS { (
                HasNonPopupStage() && !POPUP_ONLY_FOREGROUND_RESTRICTION
                && (WHICH_STAGES != WhichStages::PopupOnly)) };

            if (NON_POPUP_FIRST)
            {
                if (WILL_EXECUTE_ON_NON_POPUPS)
                {
                    ExecuteOnNonPopupStages<Lambda_t>(lambda);
                }

                if (WILL_EXECUTE_ON_POPUP)
                {
                    ExecuteOnPopupStage<Lambda_t>(lambda);
                }
            }
            else
            {
                if (WILL_EXECUTE_ON_POPUP)
                {
                    ExecuteOnPopupStage<Lambda_t>(lambda);
                }

                if (WILL_EXECUTE_ON_NON_POPUPS)
                {
                    ExecuteOnNonPopupStages<Lambda_t>(lambda);
                }
            }
        }

        void HandlePopupResponseCallback();
        void ReplaceStages(const stage::SetupPacket & SETUP_PACKET);
        void RemovePopupStage() { popup_uptr.reset(); }

        void ReplacePopupStage(
            const popup::PopupInfo & POPUP_INFO,
            const sfml_util::PopupCallback_t::IHandlerPtr_t POPUP_HANDLER_PTR);

        void SetPopupResponse(const popup::ResponseTypes::Enum TYPE, const std::size_t SELECTION);
        void Focus(const sfml_util::IEntityPtr_t & IENTITY_PTR);

    private:
        void RemovePopupResponse();
        void RemovePopupAll();

        template <typename Lambda_t>
        void ExecuteOnPopupStage(Lambda_t & lambda)
        {
            if (popup_uptr)
            {
                lambda(*this, popup_uptr);
            }
        }

        template <typename Lambda_t>
        void ExecuteOnNonPopupStages(Lambda_t & lambda)
        {
            for (auto & iStageUPtr : non_popup_uvec)
            {
                lambda(*this, iStageUPtr);
            }
        }

        void FocusSet(const sfml_util::IEntityPtr_t IENTITY_PTR);
        void FocusRemove();

    private:
        // this vector owns and controls the lifetime of all Stages in the game
        stage::IStageUVec_t non_popup_uvec;

        // observer pointer copy from one of non_popup_uvec's Stages
        sfml_util::IEntityPtrOpt_t focus_entity_ptr_opt;

        // this owns and controls the lifetime of all Popup Stages in the game
        // this pointer will never be added to iStageUPVec_
        stage::IStageUPtr_t popup_uptr;

        // this is an observer pointer to the Stage that handles all popup callbacks
        sfml_util::PopupCallback_t::IHandlerPtrOpt_t popup_callback_handler_ptr_opt;

        std::string popup_desc;

        popup::PopupResponse popup_response;
    };

} // namespace game
} // namespace heroespath

#endif // HEROESPATH_GAME_ACTIVE_STAGES_INCLUDED
