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
#ifndef HEROESPATH_POPUP_POPUPENUMS_HPP_INCLUDE
#define HEROESPATH_POPUP_POPUPENUMS_HPP_INCLUDE
//
// popup-enums.hpp
//
#include "popup/popup-response-enum.hpp"

#include <string>

namespace heroespath
{
namespace popup
{

    struct PopupButtons
    {
        enum Enum : unsigned
        {
            None = 0,
            Okay = ResponseTypes::Okay,
            Select = ResponseTypes::Select,
            Cancel = ResponseTypes::Cancel,
            SelectCancel = ResponseTypes::Select | ResponseTypes::Cancel,
            Continue = ResponseTypes::Continue,
            YesNo = ResponseTypes::Yes | ResponseTypes::No,
            YesNoCancel = ResponseTypes::Yes | ResponseTypes::No | ResponseTypes::Cancel
        };

        static const std::string ToString(const PopupButtons::Enum E);
        static bool IsValid(const PopupButtons::Enum E);
    };

    struct PopupButtonColor
    {
        enum Enum
        {
            Light = 0,
            Dark,
            Count
        };

        static const std::string ToString(const PopupButtonColor::Enum);
        static bool IsValid(const PopupButtonColor::Enum);
    };

    struct PopupImage
    {
        enum Enum
        {
            Banner = 0,
            Regular,
            RegularSidebar,
            Large,
            LargeSidebar,
            Custom,
            Spellbook,
            MusicSheet,
            Count
        };

        static const std::string ToString(const PopupImage::Enum);
        static bool IsValid(const PopupImage::Enum);
    };
} // namespace popup
} // namespace heroespath

#endif // HEROESPATH_POPUP_POPUPENUMS_HPP_INCLUDE
