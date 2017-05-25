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
#ifndef SFMLUTIL_GUI_COLORSET_INCLUDED
#define SFMLUTIL_GUI_COLORSET_INCLUDED
//
// color-set.hpp
//  Simple struct encapsulating a GuiEntity's color scheme.
//
//suppress warnings that are safe to ignore in boost
#include "utilz/platform.hpp"
#ifdef PLATFORM_DETECTED_IS_APPLE
#pragma GCC diagnostic ignored "-Wfloat-equal"
#endif

#include <SFML/Graphics/Color.hpp>

#ifdef PLATFORM_DETECTED_IS_APPLE
#pragma GCC diagnostic warning "-Wfloat-equal"
#endif

#include <memory>
#include <string>
#include <vector>


namespace sfml_util
{
namespace gui
{

    //Encapsulates the colors required to draw a GuiEntity that can
    //have focus or not have focus.  Using the default constructor
    //will use sf::Color::White when HasFocus() and a slightly darker
    //white color when ! HasFocus().
    struct ColorSet
    {
        static const sf::Uint8 DEFAULT_OFFSET_VALUE_;
        static const sf::Color DEFAULT_OFFSET_COLOR_;

        //use this constructor for default white/offwhite colors
        ColorSet();

        //use this constructor to specify all custom colors
        ColorSet(const sf::Color & FG_WITH_FOCUS,
                 const sf::Color & BG_WITH_FOCUS,
                 const sf::Color & FG_WITHOUT_FOCUS,
                 const sf::Color & BG_WITHOUT_FOCUS);

        //use this constructor when the bg is white and colors are the same with and without focus
        explicit ColorSet(const sf::Color & FG);

        //use this constructor when the without_focus colors are automatically darker than those with_focus
        ColorSet(const sf::Color & FG_WITH_FOCUS,
                 const sf::Color & BG_WITH_FOCUS);

        void ForceWithoutFocusToMatchWithFocus();

        const std::string ToString(const bool WILL_WRAP = false) const;

        sf::Color foreground_with_focus;
        sf::Color background_with_focus;
        sf::Color foreground_without_focus;
        sf::Color background_without_focus;
    };


    using ColorSetSPtr_t = std::shared_ptr<ColorSet>;
    using ColorSetSVec_t = std::vector<ColorSetSPtr_t>;


    bool operator<(const ColorSet & L, const ColorSet & R);


    bool operator==(const ColorSet & L, const ColorSet & R);


    inline bool operator!=(const ColorSet & L, const ColorSet & R)
    {
        return ! (L == R);
    }

}
}
#endif //SFMLUTIL_GUI_COLORSET_INCLUDED
