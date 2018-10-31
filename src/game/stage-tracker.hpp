// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GAME_STAGE_TRACKER_HPP_INCLUDED
#define HEROESPATH_GAME_STAGE_TRACKER_HPP_INCLUDED
//
// stage-tracker.hpp
//
#include "game/phase-enum.hpp"
#include "stage/stage-enum.hpp"

namespace heroespath
{
namespace game
{

    // Keeps track of the current stage, the previous stage, and the previous stage that was
    // playable and not a popup.
    class StageTracker
    {
    public:
        stage::Stage::Enum GetCurrent() const { return current; }
        void SetCurrent(const stage::Stage::Enum NEW_CURRENT_STAGE);
        void SetCurrentToNonPopupPrevious();
        stage::Stage::Enum GetNonPopupPrevious() const;
        Phase::Enum GetPhase() const;

    private:
        stage::Stage::Enum current = stage::Stage::None;
        stage::Stage::Enum previous = stage::Stage::None;
        stage::Stage::Enum before_popup = stage::Stage::None;
    };

} // namespace game
} // namespace heroespath

#endif // HEROESPATH_GAME_STAGE_TRACKER_HPP_INCLUDED
