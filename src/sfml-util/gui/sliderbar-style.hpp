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
#ifndef HEROESPATH_SFMLUTIL_SLIDERBARSYLE_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_SLIDERBARSYLE_HPP_INCLUDED
//
// sliderbar-style.hpp
//  A type that encapsulates the style of a SliderBar.
//
#include "sfml-util/brightness-enum.hpp"
#include "sfml-util/orientation-enum.hpp"

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        // encapsulates all the the style elements of a SliderBar
        struct SliderStyle
        {
            explicit SliderStyle(
                const Orientation::Enum ORIENTATION = Orientation::Vert,
                const Brightness::Enum BRIGHTNESS = Brightness::Bright,
                const bool WILL_LABEL_ARROWS = true,
                const bool IS_LINE_LARGE = true)
                : orientation(ORIENTATION)
                , brightness(BRIGHTNESS)
                , willLabelArrows(WILL_LABEL_ARROWS)
                , isLineLarge(IS_LINE_LARGE)
            {}

            Orientation::Enum orientation;
            Brightness::Enum brightness;
            bool willLabelArrows;
            bool isLineLarge;
        };

        inline bool operator==(const SliderStyle & L, const SliderStyle & R)
        {
            return (
                (L.brightness == R.brightness) && (L.isLineLarge == R.isLineLarge)
                && (L.orientation == R.orientation) && (L.willLabelArrows == R.willLabelArrows));
        }

        inline bool operator!=(const SliderStyle & L, const SliderStyle & R) { return !(L == R); }
    }
}
}

#endif // HEROESPATH_SFMLUTIL_SLIDERBARSYLE_HPP_INCLUDED
