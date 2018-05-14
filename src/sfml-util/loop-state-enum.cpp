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
// loop-state-enum.cpp
//
#include "loop-state-enum.hpp"

#include "misc/boost-string-includes.hpp"

namespace heroespath
{
namespace sfml_util
{

    const std::string LoopState::ToString(const LoopState::Enum E)
    {
        switch (E)
        {
            case None:
            {
                return "None";
            }
            case Intro:
            {
                return "Intro";
            }
            case MainMenu:
            {
                return "MainMenu";
            }
            case LoadGameMenu:
            {
                return "LoadGameMenu";
            }
            case CharacterCreation:
            {
                return "CharacterCreation";
            }
            case PartyCreation:
            {
                return "PartyCreation";
            }
            case Adventure:
            {
                return "Adventure";
            }
            case Combat:
            {
                return "Combat";
            }
            case Camp:
            {
                return "Camp";
            }
            case Inn:
            {
                return "Inn";
            }
            case Query:
            {
                return "Query";
            }
            case Popup:
            {
                return "Popup";
            }
            case Credits:
            {
                return "Credits";
            }
            case Settings:
            {
                return "Settings";
            }
            case Exit:
            {
                return "Exit";
            }
            case Inventory:
            {
                return "Inventory";
            }
            case Treasure:
            {
                return "Treasure";
            }
            case Test:
            {
                return "Test";
            }
            case Count:
            default:
            {
                ThrowInvalidValueForFunction(E, "ToString");
            }
        }
    }

} // namespace sfml_util
} // namespace heroespath
