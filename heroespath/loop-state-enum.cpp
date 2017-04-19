//
// loop-state-enum.cpp
//
#include "loop-state-enum.hpp"

#include "sfml-util/boost-string-includes.hpp"

#include <sstream>
#include <exception>


namespace heroespath
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
                ss << "heroespath::LoopState::ToString(" << E << ")_InvalidValueError.";
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
            ss << "heroespath::LoopState::FromString(\"" << ENUM_STR << "\") unknown stage name.";
            throw std::runtime_error(ss.str());
        }
    }

}
