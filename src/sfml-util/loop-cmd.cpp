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

    void LoopCmd_Execute::Execute()
    {
        game::LoopManager::Instance()->CommandLoopAccess(this).Execute();
    }

    void LoopCmd_StateChange::Execute()
    {
        game::LoopManager::Instance()->CommandLoopAccess(this).SetState(newState_);
    }

    void LoopCmd_SetMouseVisibility::Execute()
    {
        game::LoopManager::Instance()->CommandLoopAccess(this).SetMouseVisibility(isVisible_);
    }

    void LoopCmd_AddStage_Default::Execute()
    {
        game::LoopManager::Instance()->CommandLoopAccess(this).AddStage(
            new Stage("Default", {}, false));
    }

    void LoopCmd_SetHoldTime::Execute()
    {
        game::LoopManager::Instance()->CommandLoopAccess(this).SetHoldTime(holdTimeSec_);
    }

    void LoopCmd_RemoveAllStages::Execute()
    {
        game::LoopManager::Instance()->CommandLoopAccess(this).FreeAllStages();
    }

    void LoopCmd_ExitAfterFade::Execute()
    {
        game::LoopManager::Instance()->CommandLoopAccess(this).SetWillExitAfterFade(true);
    }

    void LoopCmd_FadeIn::Execute()
    {
        game::LoopManager::Instance()->CommandLoopAccess(this).FadeIn(
            fadeFromColor_, speedMult_, willHoldAfter_);
    }

    void LoopCmd_FadeOut::Execute()
    {
        game::LoopManager::Instance()->CommandLoopAccess(this).FadeOut(
            fadeToColor_, speedMult_, willHoldAfter_);
    }

    void LoopCmd_ExitAfterKeypress::Execute()
    {
        game::LoopManager::Instance()->CommandLoopAccess(this).SetWillExitOnKeypress(
            willExitOnKeypress_);
    }

    void LoopCmd_ExitAfterMouseclick::Execute()
    {
        game::LoopManager::Instance()->CommandLoopAccess(this).SetWillExitOnMouseclick(
            willExitOnMouseClick_);
    }

    void LoopCmd_RemoveFocus::Execute()
    {
        game::LoopManager::Instance()->CommandLoopAccess(this).RemoveFocus();
    }

    void LoopCmd_IgnoreMouse::Execute()
    {
        game::LoopManager::Instance()->CommandLoopAccess(this).SetIgnoreMouse(willIgnoreMouse_);
    }

    void LoopCmd_IgnoreKeystrokes::Execute()
    {
        game::LoopManager::Instance()->CommandLoopAccess(this).SetIgnoreKeystrokes(
            willIgnoreKeyStrokes_);
    }

    void LoopCmd_FakeMouseClick::Execute()
    {
        game::LoopManager::Instance()->CommandLoopAccess(this).FakeMouseClick(clickPosV_);
    }

} // namespace sfml_util
} // namespace heroespath
