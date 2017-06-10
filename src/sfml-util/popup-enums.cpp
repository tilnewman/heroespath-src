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
// popup-enums.cpp
//
#include "popup-enums.hpp"

#include <sstream>
#include <exception>


namespace sfml_util
{

    const std::string PopupButtons::ToString(const PopupButtons::Enum E)
    {
        switch (E)
        {
            case None:         { return "None"; }
            case Okay:         { return "Okay";         }
            case Select:       { return "Select";       }
            case Cancel:       { return "Cancel";       }
            case SelectCancel: { return "SelectCancel"; }
            case Continue:     { return "Continue";     }
            case YesNo:        { return "YesNo";        }
            case YesNoCancel:  { return "YesNoCancel";  }
            default:
            {
                std::ostringstream ss;
                ss << "sfml_util::PopupButtons::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    bool PopupButtons::IsValid(const PopupButtons::Enum E)
    {
        switch (E)
        {
            case None:
            case Okay:
            case Select:
            case Cancel:
            case SelectCancel:
            case Continue:
            case YesNo:
            case YesNoCancel: { return true;  }
            default:          { return false; }
        }
    }


    const std::string PopupButtonColor::ToString(const PopupButtonColor::Enum E)
    {
        switch(E)
        {
            case Light: { return "Light"; }
            case Dark:  { return "Dark";  }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "sfml_util::PopupButtonColor::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    bool PopupButtonColor::IsValid(const PopupButtonColor::Enum E)
    {
        switch (E)
        {
            case Light:
            case Dark:  { return true;  }
            case Count:
            default:    { return false; }
        }
    }


    const std::string PopupImage::ToString(const PopupImage::Enum E)
    {
        switch (E)
        {
            case Banner:            { return "Banner"; }
            case Regular:           { return "Regular"; }
            case RegularSidebar:    { return "RegularSidebar"; }
            case Large:             { return "Large"; }
            case LargeSidebar:      { return "LargeSidebar"; }
            case Custom:            { return "Custom"; }
            case Spellbook:         { return "Spellbook"; }
            case MusicSheet:        { return "MusicSheet"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "sfml_util::PopupImage::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    bool PopupImage::IsValid(const PopupImage::Enum E)
    {
        switch (E)
        {
            case Banner:
            case Regular:
            case RegularSidebar:
            case Large:
            case LargeSidebar:
            case Custom:
            case Spellbook:
            case MusicSheet: { return true;  }
            case Count:
            default:         { return false; }
        }
    }

}
