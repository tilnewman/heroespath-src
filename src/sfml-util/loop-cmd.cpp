// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
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

    LoopCmd::~LoopCmd() = default;

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
} // namespace sfml_util
} // namespace heroespath
