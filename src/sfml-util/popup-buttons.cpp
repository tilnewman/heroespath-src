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
// popup-buttons.cpp
//
#include "popup-buttons.hpp"

#include "sfml-util/gui/mouse-text-info.hpp"
#include "sfml-util/font-manager.hpp"
#include "sfml-util/sound-manager.hpp"

#include "game/loop-manager.hpp"


namespace sfml_util
{

    PopupButton_Yes::PopupButton_Yes(const game::PopupInfo & POPUP_INFO, const float POS_LEFT, const float POS_TOP)
    :
        TextButton("PopupButton_Yes", POS_LEFT, POS_TOP, gui::MouseTextInfo::Make_PopupButtonSet("Yes", POPUP_INFO))
    {}

    void PopupButton_Yes::OnClick(const sf::Vector2f &)
    {
        SoundManager::Instance()->SoundEffectsSet_Thock().PlayRandom();
        game::LoopManager::Instance()->PopupWaitEnd(Response::Yes);
    }


    PopupButton_No::PopupButton_No(const game::PopupInfo & POPUP_INFO, const float POS_LEFT, const float POS_TOP)
    :
        TextButton("PopupButton_No", POS_LEFT, POS_TOP, gui::MouseTextInfo::Make_PopupButtonSet("No", POPUP_INFO))
    {}

    void PopupButton_No::OnClick(const sf::Vector2f &)
    {
        SoundManager::Instance()->SoundEffectsSet_Thock().PlayRandom();
        game::LoopManager::Instance()->PopupWaitEnd(Response::No);
    }


    PopupButton_Cancel::PopupButton_Cancel(const game::PopupInfo & POPUP_INFO, const float POS_LEFT, const float POS_TOP)
    :
        TextButton("PopupButton_Cancel", POS_LEFT, POS_TOP, gui::MouseTextInfo::Make_PopupButtonSet("Cancel", POPUP_INFO))
    {}

    void PopupButton_Cancel::OnClick(const sf::Vector2f &)
    {
        SoundManager::Instance()->SoundEffectsSet_Thock().PlayRandom();
        game::LoopManager::Instance()->PopupWaitEnd(Response::Cancel);
    }


    PopupButton_Continue::PopupButton_Continue(const game::PopupInfo & POPUP_INFO, const float POS_LEFT, const float POS_TOP)
    :
        TextButton("PopupButton_Continue", POS_LEFT, POS_TOP, gui::MouseTextInfo::Make_PopupButtonSet("Continue", POPUP_INFO))
    {}

    void PopupButton_Continue::OnClick(const sf::Vector2f &)
    {
        SoundManager::Instance()->SoundEffectsSet_Thock().PlayRandom();
        game::LoopManager::Instance()->PopupWaitEnd(Response::Continue);
    }


    PopupButton_Okay::PopupButton_Okay(const game::PopupInfo & POPUP_INFO, const float POS_LEFT, const float POS_TOP)
    :
        TextButton("PopupButton_Okay", POS_LEFT, POS_TOP, gui::MouseTextInfo::Make_PopupButtonSet("Okay", POPUP_INFO))
    {}

    void PopupButton_Okay::OnClick(const sf::Vector2f &)
    {
        SoundManager::Instance()->SoundEffectsSet_Thock().PlayRandom();
        game::LoopManager::Instance()->PopupWaitEnd(Response::Okay);
    }


    PopupButton_Select::PopupButton_Select(const game::PopupInfo & POPUP_INFO, const float POS_LEFT, const float POS_TOP)
    :
        TextButton("PopupButton_Select", POS_LEFT, POS_TOP, gui::MouseTextInfo::Make_PopupButtonSet("Select", POPUP_INFO)),
        selection_(-1)//any negative number will work here
    {}

    void PopupButton_Select::OnClick(const sf::Vector2f &)
    {
        if (selection_ >= 0)
        {
            SoundManager::Instance()->SoundEffectsSet_Thock().PlayRandom();
            game::LoopManager::Instance()->PopupWaitEnd(Response::Select, static_cast<std::size_t>(selection_));
        }
    }

}
