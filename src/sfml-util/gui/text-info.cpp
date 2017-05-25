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
// text-info.cpp
//
#include "text-info.hpp"

#include "utilz/real.hpp"
#include "sfml-util/sfml-util.hpp"
#include "utilz/platform.hpp"

#include "utilz/assertlogandthrow.hpp"

#include <tuple>


namespace sfml_util
{
namespace gui
{

    TextInfo::TextInfo( const std::string &   TEXT,
                        const FontSPtr_t &    FONT,
                        const unsigned int    CHAR_SIZE,
                        const sf::Color &     COLOR,
                        const sf::BlendMode & BLEND_MODE,
                        const sf::Uint32      STYLE,
                        const Justified::Enum JUSTIFIED,
                        const bool            IS_OUTLINE_ONLY,
                        const float           OUTLINE_THICKNESS)
    :
        text	        (TEXT),
        fontSPtr	    (FONT),
        charSize	    (CHAR_SIZE),
        color	        (COLOR),
        blendMode       (BLEND_MODE),
        style	        (STYLE),
        justified       (JUSTIFIED),
        isOutlineOnly	(IS_OUTLINE_ONLY),
        outlineThickness(OUTLINE_THICKNESS)
    {}


    TextInfo::TextInfo( const std::string &   TEXT,
                        const FontSPtr_t &    FONT,
                        const unsigned int    CHAR_SIZE,
                        const sf::Color &     COLOR,
                        const Justified::Enum JUSTIFIED )
    :
        text	        (TEXT),
        fontSPtr	    (FONT),
        charSize	    (CHAR_SIZE),
        color	        (COLOR),
        blendMode       (sf::BlendAlpha),
        style	        (sf::Text::Style::Regular),
        justified       (JUSTIFIED),
        isOutlineOnly	(false),
        outlineThickness(0.0f)
    {}


    bool operator<(const TextInfo & L, const TextInfo & R)
    {
        if (L.color != R.color)
            return sfml_util::color::ColorLess(L.color, R.color);

        if (L.blendMode != R.blendMode)
            return sfml_util::color::BlendModeLess(L.blendMode, R.blendMode);

        return std::tie(L.text,
                        L.fontSPtr,
                        L.charSize,
                        L.style,
                        L.justified,
                        L.isOutlineOnly,
                        L.outlineThickness)
               <
               std::tie(R.text,
                        R.fontSPtr,
                        R.charSize,
                        R.style,
                        R.justified,
                        R.isOutlineOnly,
                        R.outlineThickness);
    }


    bool operator==(const TextInfo & L, const TextInfo & R)
    {
        if (utilz::IsRealClose(L.outlineThickness, R.outlineThickness) == false)
            return false;

        return std::tie(L.text,
                        L.fontSPtr,
                        L.charSize,
                        L.color,
                        L.blendMode,
                        L.style,
                        L.justified,
                        L.isOutlineOnly)
               ==
               std::tie(R.text,
                        R.fontSPtr,
                        R.charSize,
                        R.color,
                        R.blendMode,
                        R.style,
                        R.justified,
                        R.isOutlineOnly);
    }


    void SetupText(sf::Text & text, const TextInfo & TEXT_INFO)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((false == TEXT_INFO.text.empty()), "sfml_util::gui::SetupText() given a TEXT_INFO.string that was empty.");
        text.setString(TEXT_INFO.text);

        M_ASSERT_OR_LOGANDTHROW_SS((nullptr != TEXT_INFO.fontSPtr.get()), "sfml_util::gui::SetupText(\"" << TEXT_INFO.text << "\") was given a null font pointer.");
        text.setFont(*TEXT_INFO.fontSPtr);
        text.setStyle(TEXT_INFO.style);
        text.setCharacterSize(TEXT_INFO.charSize);

//linux SFML lib does not seem to support outline fonts...
#ifdef PLATFORM_DETECTED_IS_LINUX
        text.setColor(TEXT_INFO.color);
#else
        if (TEXT_INFO.isOutlineOnly)
        {
            text.setFillColor(sf::Color::Transparent);
            text.setOutlineColor(TEXT_INFO.color);
            text.setOutlineThickness(TEXT_INFO.outlineThickness);
        }
        else
        {
            text.setFillColor(TEXT_INFO.color);
        }
#endif
    }

}
}
