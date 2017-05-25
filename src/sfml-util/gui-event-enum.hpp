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
#ifndef SFMLUTIL_GUIEVENT_ENUM_HPP_INCLUDED
#define SFMLUTIL_GUIEVENT_ENUM_HPP_INCLUDED
//
// gui-event-enum.hpp
//
#include <string>


namespace sfml_util
{

    struct GuiEvent
    {
        enum Enum : unsigned int
        {
            None             = 0,
            Click            = 1 << 0,
            DoubleClick      = 1 << 1,
            MouseWheel       = 1 << 2,
            SelectionChange  = 1 << 3,
            Keypress         = 1 << 4,
            FocusChange      = 1 << 5
        };

        static const std::string ToString(const GuiEvent::Enum);
        static bool IsValid(const GuiEvent::Enum);
    };

}

#endif //SFMLUTIL_GUIEVENT_ENUM_HPP_INCLUDED