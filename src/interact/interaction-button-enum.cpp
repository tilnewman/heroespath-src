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
// interation-button-enum.cpp
//
#include "interaction-button-enum.hpp"
#include <exception>
#include <sstream>

namespace heroespath
{
namespace interact
{

    const std::string Buttons::ToString(const Buttons::Enum BUTTON_TYPE)
    {
        switch (BUTTON_TYPE)
        {
            case Ignore:
                return "Ignore";
            case Continue:
                return "Continue";
            case Goodbye:
                return "Goodbye";
            case Yes:
                return "Yes";
            case No:
                return "No";
            case Unlock:
                return "Unlock";
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "map::Interaction_::Buttons::Enum::ToString(" << BUTTON_TYPE
                   << ")_InvalidValueError.";

                throw std::range_error(ss.str());
            }
        }
    }

    sf::Keyboard::Key Buttons::Key(const Buttons::Enum BUTTON_TYPE)
    {
        switch (BUTTON_TYPE)
        {
            case Ignore:
                return sf::Keyboard::I;
            case Continue:
                return sf::Keyboard::C;
            case Goodbye:
                return sf::Keyboard::G;
            case Yes:
                return sf::Keyboard::Y;
            case No:
                return sf::Keyboard::N;
            case Unlock:
                return sf::Keyboard::U;
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "map::Interact::Buttons::Enum::Key(" << BUTTON_TYPE << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }
} // namespace interact
} // namespace heroespath
