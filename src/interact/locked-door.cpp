// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
//
// locked-door.cpp
//
#include "locked-door.hpp"
#include "game/game.hpp"
#include "interact/lock-interactions.hpp"
#include "misc/vectors.hpp"
#include "popup/popup-manager.hpp"
#include "popup/popup-stage-char-select.hpp"
#include "popup/popup-stage-treasure-trap.hpp"
#include "stage/adventure-stage-interact-stage.hpp"
#include "state/game-state.hpp"
#include "state/maps.hpp"
#include "state/world.hpp"

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
              sfml_util::sound_effect::DoorLocked)
        , transition_(TRANSITION)
    {}

    bool LockedDoor::OnInteraction(
        stage::InteractStage * const interactStagePtr, const Button & BUTTON)
    {
        if (BUTTON.Which() == Buttons::Ignore)
        {
            interactStagePtr->InteractionManager().RemoveCurrent();
            return true;
        }
        else if (BUTTON.Which() == Buttons::Unlock)
        {
            Lock(); // this refers to locking the interaction NOT the door
            interactStagePtr->LockPick().PopupCharacterSelection(interactStagePtr);
            return true;
        }

        return false;
    }

    bool LockedDoor::OnSuccess(stage::InteractStage * const stagePtr)
    {
        game::Game::Instance()->State().GetWorld().GetMaps().Current().IsDoorLocked(
            transition_.WhichLevel(), false);

        stagePtr->MapTransition(transition_);
        stagePtr->InteractionManager().RemoveCurrent();
        return true;
    }
} // namespace interact
} // namespace heroespath
