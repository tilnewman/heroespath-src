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
// text-button-credits.cpp
//
#include "text-button-credits.hpp"

#include "sfml-util/gui/mouse-text-info.hpp"

#include "game/loop-manager.hpp"


namespace game
{

    CreditsButton::CreditsButton(const std::string & NAME)
    :
        TextButton(std::string(NAME).append("_CreditsButton"))
    {}


    CreditsButton::CreditsButton(const std::string &                   NAME,
                                 const float                           POS_LEFT,
                                 const float                           POS_TOP,
                                 const sfml_util::gui::MouseTextInfo & MOUSE_TEXT_INFO,
                                 const float                           CLICK_SHIFT)
    :
        TextButton (std::string(NAME).append("_CreditsButton"), POS_LEFT, POS_TOP, MOUSE_TEXT_INFO, CLICK_SHIFT)
    {}


    CreditsButton::~CreditsButton()
    {}


    void CreditsButton::Setup(const float                           POS_LEFT,
                              const float                           POS_TOP,
                              const sfml_util::gui::MouseTextInfo & MOUSE_TEXT_INFO,
                              const float                           CLICK_SHIFT)
    {
        TextButton::Setup(POS_LEFT, POS_TOP, MOUSE_TEXT_INFO, CLICK_SHIFT);
    }


    void CreditsButton::OnClick(const sf::Vector2f &)
    {
        LoopManager::Instance()->Goto_Credits();
    }

}
