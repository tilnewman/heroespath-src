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
#ifndef SFMLUTIL_GUI_MOUSETEXTINFO_HPP_INCLUDED
#define SFMLUTIL_GUI_MOUSETEXTINFO_HPP_INCLUDED
//
// mouse-text-info.hpp
//  A handy wrapper struct that holds three colors relating to mouse position.
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/gui/text-info.hpp"

#include "popup/popup-info.hpp"

#include <string>
#include <vector>
#include <memory>


namespace sfml_util
{
namespace gui
{

    class MouseTextInfo
    {
    public:
        MouseTextInfo();

        explicit MouseTextInfo(const TextInfo & TEXT_INFO);

        MouseTextInfo(
            const TextInfo & UP,
            const TextInfo & DOWN,
            const TextInfo & OVER);

        MouseTextInfo(
            const TextInfo &  TEXT_INFO_UP,
            const sf::Color & COLOR_DOWN,
            const sf::Color & COLOR_OVER);

        MouseTextInfo(
            const std::string & TEXT,
            const FontPtr_t     FONT_PTR,
            const unsigned int  SIZE,
            const sf::Color &   COLOR_UP   = sf::Color(220,220,220),
            const sf::Color &   COLOR_DOWN = sf::Color::White,
            const sf::Color &   COLOR_OVER = sf::Color::White);

        //MouseTextInfo generating helper functions
        static const MouseTextInfo Make_PopupButtonSet(
            const std::string & NAME,
            const heroespath::popup::PopupInfo &);

        TextInfo up;
        TextInfo down;
        TextInfo over;
    };


    using MouseTextInfoUPtr_t = std::unique_ptr<MouseTextInfo>;
    using MouseTextInfoVec_t  = std::vector<MouseTextInfo>;


    bool operator<(const MouseTextInfo & L, const MouseTextInfo & R);

    bool operator==(const MouseTextInfo & L, const MouseTextInfo & R);

    inline bool operator!=(const MouseTextInfo & L, const MouseTextInfo & R)
    {
        return ! (L == R);
    }

}
}

#endif //SFMLUTIL_GUI_MOUSETEXTINFO_HPP_INCLUDED
