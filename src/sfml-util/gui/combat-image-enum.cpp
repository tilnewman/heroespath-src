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
// combat-image-enum.hpp
//
#include "combat-image-enum.hpp"

#include <sstream>
#include <exception>


namespace sfml_util
{
namespace gui
{

    const std::string CombatImageType::ToString(const Enum E)
    {
        switch (E)
        {
            case Wing:      { return "Wing"; }
            case Arrow1:    { return "Arrow1"; }
            case Arrow2:    { return "Arrow2"; }
            case Arrow3:    { return "Arrow3"; }
            case Bolt:      { return "Bolt"; }
            case Dart:      { return "Dart"; }
            case Stone1:    { return "Stone1"; }
            case Stone2:    { return "Stone2"; }
            case Stone3:    { return "Stone3"; }
            case Stone4:    { return "Stone4"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "sfml_util::CombatImageType::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    const std::string CombatImageType::Filename(const Enum E)
    {
        switch (E)
        {
            case Wing:      { return "wing.png";   }
            case Arrow1:    { return "arrow1.gif"; }
            case Arrow2:    { return "arrow2.gif"; }
            case Arrow3:    { return "arrow3.gif"; }
            case Bolt:      { return "arrow4.gif"; }
            case Dart:      { return "dart.gif";   }
            case Stone1:    { return "stone1.gif"; }
            case Stone2:    { return "stone2.gif"; }
            case Stone3:    { return "stone3.gif"; }
            case Stone4:    { return "stone4.gif"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "sfml_util::CombatImageType::Filename(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }

}
}
