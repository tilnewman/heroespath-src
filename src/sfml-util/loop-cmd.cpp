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
// loop-cmd.cpp
//
#include "loop-cmd.hpp"

#include "game/loop-manager.hpp"

namespace heroespath
{
namespace sfml_util
{

    LoopCmd::LoopCmd(const std::string & NAME)
        : NAME_(std::string(NAME).append("_LoopCommand"))
    {}

    LoopCmd::~LoopCmd() {}

    bool LoopCmd_Execute::Execute()
    {
        return game::LoopManager::Instance()->CommandLoopAccess(this).Execute();
    }

    bool LoopCmd_StateChange::Execute()
    {
        game::LoopManager::Instance()->CommandLoopAccess(this).SetState(STATE_);
        return true;
    }

    bool LoopCmd_SetMouseVisibility::Execute()
    {
        game::LoopManager::Instance()->CommandLoopAccess(this).SetMouseVisibility(IS_VISIBLE_);
        return true;
    }

    bool LoopCmd_AddStage_Default::Execute()
    {
        game::LoopManager::Instance()->CommandLoopAccess(this).AddStage(new Stage("Default"));
        return true;
    }

    bool LoopCmd_SetHoldTime::Execute()
    {
        game::LoopManager::Instance()->CommandLoopAccess(this).SetHoldTime(TIME_);
        return true;
    }

    bool LoopCmd_RemoveAllStages::Execute()
    {
        game::LoopManager::Instance()->CommandLoopAccess(this).FreeAllStages();
        return true;
    }

    bool LoopCmd_ExitAfterFade::Execute()
    {
        game::LoopManager::Instance()->CommandLoopAccess(this).SetWillExitAfterFade(true);
        return true;
    }

    bool LoopCmd_FadeIn::Execute()
    {
        game::LoopManager::Instance()->CommandLoopAccess(this).FadeIn(
            FADE_FROM_COLOR_, SPEED_MULT_, WILL_HOLD_FADE_);

        return true;
    }

    bool LoopCmd_FadeOut::Execute()
    {
        game::LoopManager::Instance()->CommandLoopAccess(this).FadeOut(
            FADE_TO_COLOR_, SPEED_MULT_, WILL_HOLD_FADE_);

        return true;
    }

    bool LoopCmd_ExitAfterKeypress::Execute()
    {
        game::LoopManager::Instance()->CommandLoopAccess(this).SetWillExitOnKeypress(
            WILL_EXIT_ON_KEYSTROKE_);

        return true;
    }

    bool LoopCmd_ExitAfterMouseclick::Execute()
    {
        game::LoopManager::Instance()->CommandLoopAccess(this).SetWillExitOnMouseclick(
            WILL_EXIT_ON_MOUSECLICK_);

        return true;
    }

    bool LoopCmd_RemoveFocus::Execute()
    {
        game::LoopManager::Instance()->CommandLoopAccess(this).RemoveFocus();
        return true;
    }

    bool LoopCmd_IgnoreMouse::Execute()
    {
        game::LoopManager::Instance()->CommandLoopAccess(this).SetIgnoreMouse(WILL_IGNORE_MOUSE_);

        return true;
    }

    bool LoopCmd_IgnoreKeystrokes::Execute()
    {
        game::LoopManager::Instance()->CommandLoopAccess(this).SetIgnoreKeystrokes(
            WILL_IGNORE_KEYSTROKES_);

        return true;
    }

    bool LoopCmd_FakeMouseClick::Execute()
    {
        game::LoopManager::Instance()->CommandLoopAccess(this).FakeMouseClick(MOUSE_CLICK_POS_);

        return true;
    }
}
}
