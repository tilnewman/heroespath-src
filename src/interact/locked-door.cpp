// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// locked-door.cpp
//
#include "locked-door.hpp"

#include "game/game-state.hpp"
#include "game/game.hpp"
#include "game/maps.hpp"
#include "game/world.hpp"
#include "interact/lock-interactions.hpp"
#include "misc/vectors.hpp"
#include "popup/popup-manager.hpp"
#include "popup/popup-stage-char-select.hpp"
#include "popup/popup-stage-treasure-trap.hpp"
#include "stage/adventure-stage-interact-stage.hpp"

namespace heroespath
{
namespace interact
{

    LockedDoor::LockedDoor(const map::Transition & TRANSITION)
        : InteractionBase(
            Interact::Lock,
            InteractionBase::MakeTextInfo("This door is locked.", Text::System),
            ButtonVec_t({ Button(Buttons::Unlock), Button(Buttons::Ignore) }),
            "media-images-misc-door-locked",
            gui::sound_effect::DoorLocked)
        , transition_(TRANSITION)
    {}

    bool LockedDoor::OnInteraction(
        const stage::InteractStagePtr_t INTERACTION_STAGE_PTR, const Button & BUTTON)
    {
        if (BUTTON.Which() == Buttons::Ignore)
        {
            HandleIgnore(INTERACTION_STAGE_PTR);
            return true;
        }
        else if (BUTTON.Which() == Buttons::Unlock)
        {
            Lock(); // this refers to locking the interaction NOT the door

            INTERACTION_STAGE_PTR->LockPick().PopupCharacterSelection(
                INTERACTION_STAGE_PTR, INTERACTION_STAGE_PTR.Ptr());

            return true;
        }

        return false;
    }

    bool LockedDoor::OnSuccess(const stage::InteractStagePtr_t INTERACTION_STAGE_PTR)
    {
        game::Game::Instance()->State().GetWorld().GetMaps().Current().IsDoorLocked(
            transition_.WhichLevel(), false);

        INTERACTION_STAGE_PTR->MapTransition(transition_);
        INTERACTION_STAGE_PTR->InteractionManager().RemoveCurrent();
        return true;
    }

} // namespace interact
} // namespace heroespath
