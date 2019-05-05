// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// stage-tracker.hpp
//
#include "stage-tracker.hpp"

#include "misc/log-macros.hpp"

namespace heroespath
{
namespace game
{

    void StageTracker::SetCurrent(const stage::Stage::Enum NEW_CURRENT_STAGE)
    {
        if (stage::Stage::IsPlayable(NEW_CURRENT_STAGE) == false)
        {
            M_HP_LOG_ERR(
                "Attempt to set new current stage to " + stage::Stage::ToString(NEW_CURRENT_STAGE)
                + " which is not a valid playable stage.  Not setting this as the new current "
                  "stage.  Ignoring the request.");

            return;
        }

        if (NEW_CURRENT_STAGE == current)
        {
            return;
        }

        if (stage::Stage::IsPlayableAndNotPopup(current))
        {
            before_popup = current;
        }
        else if (stage::Stage::IsPlayableAndNotPopup(previous))
        {
            before_popup = previous;
        }

        previous = current;
        current = NEW_CURRENT_STAGE;
    }

    void StageTracker::SetCurrentToNonPopupPrevious()
    {
        const auto NON_POPUP_PREVIOUS { GetNonPopupPrevious() };

        if (stage::Stage::IsPlayableAndNotPopup(NON_POPUP_PREVIOUS))
        {
            SetCurrent(NON_POPUP_PREVIOUS);
        }
        else
        {
            M_HP_LOG_ERR("Ignored because there is no valid non-popup-previous stage.");
        }
    }

    stage::Stage::Enum StageTracker::GetNonPopupPrevious() const
    {
        if (stage::Stage::IsPlayableAndNotPopup(previous))
        {
            return previous;
        }
        else
        {
            return before_popup;
        }
    }

    Phase::Enum StageTracker::GetPhase() const
    {
        const auto CURRENT_STAGE { GetCurrent() };

        if (CURRENT_STAGE == stage::Stage::Adventure)
        {
            return Phase::Exploring;
        }
        else if (CURRENT_STAGE == stage::Stage::Combat)
        {
            return Phase::Combat;
        }
        else if (CURRENT_STAGE == stage::Stage::Inventory)
        {
            return Phase::Inventory;
        }
        else if (CURRENT_STAGE == stage::Stage::Popup)
        {
            const auto PREVIOUS_STAGE_NOT_POPUP { GetNonPopupPrevious() };

            if (PREVIOUS_STAGE_NOT_POPUP == stage::Stage::Adventure)
            {
                return Phase::Exploring;
            }
            else if (PREVIOUS_STAGE_NOT_POPUP == stage::Stage::Combat)
            {
                return Phase::Combat;
            }
            else if (PREVIOUS_STAGE_NOT_POPUP == stage::Stage::Inventory)
            {
                return Phase::Inventory;
            }
        }

        return Phase::None;
    }

} // namespace game
} // namespace heroespath

/*





// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com


*/
