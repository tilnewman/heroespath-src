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
#include "sfml-util/stage.hpp"
#include "sfutil/display.hpp"

namespace heroespath
{
namespace sfml_util
{

    void LoopCmd_Execute::Execute()
    {
        game::LoopManager::Instance()->CommandLoopAccess(this).Execute();
    }

    void LoopCmd_StateChange::Execute() { game::LoopManager::Instance()->SetState(newState_); }

    void LoopCmd_SetMouseVisibility::Execute()
    {
        game::LoopManager::Instance()->CommandLoopAccess(this).SetMouseVisibility(isVisible_);
    }

    void LoopCmd_SetHoldTime::Execute()
    {
        game::LoopManager::Instance()->CommandLoopAccess(this).SetHoldTime(holdTimeSec_);
    }

    void LoopCmd_RemoveAllStages::Execute()
    {
        game::LoopManager::Instance()->CommandLoopAccess(this).FreeAllStages();
    }

    void LoopCmd_FadeIn::Execute()
    {
        game::LoopManager::Instance()->CommandLoopAccess(this).FadeIn(
            speed_, willExitAfter_, fadeFromColor_);
    }

    void LoopCmd_FadeOut::Execute()
    {
        game::LoopManager::Instance()->CommandLoopAccess(this).FadeOut(speed_, fadeToColor_);
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

    const std::string LoopCmd_FadeIn::Name() const
    {
        std::ostringstream ss;
        ss << LoopCmd::Name() << "_FromColor=" << fadeFromColor_ << "_WithSpeedMult=" << speed_
           << "_willExitAfter=" << std::boolalpha << willExitAfter_;

        return ss.str();
    }

    const std::string LoopCmd_FadeOut::Name() const
    {
        std::ostringstream ss;
        ss << LoopCmd::Name() << "To=" << fadeToColor_ << "_WithSpeedMult=" << speed_;
        return ss.str();
    }

    const std::string LoopCmd_FakeMouseClick::Name() const
    {
        std::ostringstream ss;
        ss << LoopCmd::Name() << "At=" << clickPosV_;
        return ss.str();
    }

} // namespace sfml_util
} // namespace heroespath
