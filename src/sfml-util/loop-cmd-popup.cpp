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
// loop-cmd-popup.cpp
//
#include "loop-cmd-popup.hpp"

#include "sfml-util/display.hpp"
#include "sfml-util/popup-stage.hpp"
#include "sfml-util/gui/popup-manager.hpp"

#include "game/i-popup-callback.hpp"
#include "game/loop-manager.hpp"


namespace sfml_util
{

    LoopCmd_AddStage_Popup::LoopCmd_AddStage_Popup(const game::PopupInfo & POPUP_INFO)
    :
        LoopCmd    ("AddStage_Popup"),
        POPUP_INFO_(POPUP_INFO)
    {}


    LoopCmd_AddStage_Popup::~LoopCmd_AddStage_Popup()
    {}


    bool LoopCmd_AddStage_Popup::Execute()
    {
        auto popupStagePtr( sfml_util::gui::PopupManager::Instance()->CreatePopupStage(POPUP_INFO_) );
        game::LoopManager::Instance()->CommandLoopAccess(this).SetPopup(popupStagePtr);
        return true;
    }


    LoopCmd_RemoveStage_Popup::LoopCmd_RemoveStage_Popup()
    :
        LoopCmd("RemoveStage_Popup")
    {}


    LoopCmd_RemoveStage_Popup::~LoopCmd_RemoveStage_Popup()
    {}


    bool LoopCmd_RemoveStage_Popup::Execute()
    {
        game::LoopManager::Instance()->CommandLoopAccess(this).FreePopupStage();
        return true;
    }

}
