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

#include <sstream>
#include <exception>


namespace game
{

    const std::string LoopState::ToString(const LoopState::Enum E)
    {
        switch (E)
        {
            case None:              { return "None"; }
            case Intro:             { return "Intro"; }
            case MainMenu:          { return "MainMenu"; }
            case LoadGameMenu:      { return "LoadGameMenu"; }
            case CharacterCreation: { return "CharacterCreation"; }
            case PartyCreation:     { return "PartyCreation"; }
            case Adventure:         { return "AdventureScreen"; }
            case Combat:            { return "Combat"; }
            case Camp:              { return "Camp"; }
            case Inn:               { return "Inn"; }
            case Query:             { return "Query"; }
            case Popup:             { return "Popup"; }
            case Credits:           { return "Credits"; }
            case Settings:          { return "Settings"; }
            case Exit:              { return "Exit"; }
            case Inventory:         { return "Inventory"; }
            case Test:              { return "Test"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::LoopState::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    LoopState::Enum LoopState::FromString(const std::string & ENUM_STR)
    {
        if (ENUM_STR == ToString(Intro))
            return Intro;
        else if (ENUM_STR == ToString(MainMenu))
            return MainMenu;
        else if (ENUM_STR == ToString(LoadGameMenu))
            return LoadGameMenu;
        else if (ENUM_STR == ToString(CharacterCreation))
            return CharacterCreation;
        else if (ENUM_STR == ToString(PartyCreation))
            return PartyCreation;
        else if (ENUM_STR == ToString(Adventure))
            return Adventure;
        else if (ENUM_STR == ToString(Combat))
            return Combat;
        else if (ENUM_STR == ToString(Camp))
            return Camp;
        else if (ENUM_STR == ToString(Inn))
            return Inn;
        else if (ENUM_STR == ToString(Query))
            return Query;
        else if (ENUM_STR == ToString(Credits))
            return Credits;
        else if (ENUM_STR == ToString(Settings))
            return Settings;
        else if (ENUM_STR == ToString(Inventory))
            return Inventory;
        else if (ENUM_STR == ToString(Test))
            return Test;
        else
        {
            std::ostringstream ss;
            ss << "game::LoopState::FromString(\"" << ENUM_STR << "\") unknown stage name.";
            throw std::runtime_error(ss.str());
        }
    }

}
