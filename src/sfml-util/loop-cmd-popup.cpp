// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// loop-cmd-popup.cpp
//
#include "loop-cmd-popup.hpp"

#include "game/loop-manager.hpp"
#include "popup/popup-manager.hpp"
#include "popup/popup-stage-generic.hpp"
#include "sfml-util/display.hpp"

namespace heroespath
{
namespace sfml_util
{

    LoopCmd_RemoveStage_Popup::LoopCmd_RemoveStage_Popup()
        : LoopCmd("RemoveStage_Popup")
    {}

    LoopCmd_RemoveStage_Popup::~LoopCmd_RemoveStage_Popup() = default;

    void LoopCmd_RemoveStage_Popup::Execute()
    {
        game::LoopManager::Instance()->CommandLoopAccess(this).FreePopupStage();
    }

} // namespace sfml_util
} // namespace heroespath
