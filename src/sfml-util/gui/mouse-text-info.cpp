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
// mouse-text-info.cpp
//
#include "mouse-text-info.hpp"

#include "sfml-util/font-manager.hpp"

#include "game/popup-info.hpp"

#include <tuple>


namespace sfml_util
{
namespace gui
{

    MouseTextInfo::MouseTextInfo()
    :
        up  (),
        down(),
        over()
    {}


    MouseTextInfo::MouseTextInfo(const TextInfo & TEXT_INFO)
    :
        up  (TEXT_INFO),
        down(TEXT_INFO),
        over(TEXT_INFO)
    {}


    MouseTextInfo::MouseTextInfo(const TextInfo & UP,
                                 const TextInfo & DOWN,
                                 const TextInfo & OVER)
    :
        up  (UP),
        down(DOWN),
        over(OVER)
    {}


    MouseTextInfo::MouseTextInfo(const TextInfo &  TEXT_INFO_UP,
                                 const sf::Color & COLOR_DOWN,
                                 const sf::Color & COLOR_OVER)
    :
        up  (TEXT_INFO_UP),
        down(TEXT_INFO_UP),
        over(TEXT_INFO_UP)
    {
        down.color = COLOR_DOWN;
        over.color = COLOR_OVER;
    }


    MouseTextInfo::MouseTextInfo(const std::string & TEXT,
                                 const FontPtr_t     FONT_PTR,
                                 const unsigned int  SIZE,
                                 const sf::Color &   COLOR_UP,
                                 const sf::Color &   COLOR_DOWN,
                                 const sf::Color &   COLOR_OVER)
    :
        up  (TEXT, FONT_PTR, SIZE, COLOR_UP),
        down(TEXT, FONT_PTR, SIZE, COLOR_DOWN),
        over(TEXT, FONT_PTR, SIZE, COLOR_OVER)
    {}


    const MouseTextInfo MouseTextInfo::Make_PopupButtonSet(const std::string & NAME, const game::PopupInfo & POPUP_INFO)
    {
        return MouseTextInfo(NAME,
                             FontManager::Instance()->Font_PopupButton(),
                             FontManager::Instance()->Size_Larger(),
                             FontManager::Color_PopupButtonUp(POPUP_INFO.ButtonColor()),
                             FontManager::Color_PopupButtonDown(POPUP_INFO.ButtonColor()),
                             FontManager::Color_PopupButtonOver(POPUP_INFO.ButtonColor()));
    }


    bool operator<(const MouseTextInfo & L, const MouseTextInfo & R)
    {
        return std::tie(L.up, L.down, L.over) < std::tie(R.up, R.down, R.over);
    }


    bool operator==(const MouseTextInfo & L, const MouseTextInfo & R)
    {
        return std::tie(L.up, L.down, L.over) == std::tie(R.up, R.down, R.over);
    }

}
}
