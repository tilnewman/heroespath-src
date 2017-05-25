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
// target-enum.cpp
//
#include "target-enum.hpp"

#include "misc/random.hpp"

#include <sstream>


namespace game
{

    const std::string TargetType::ToString(const Enum E)
    {
        switch (E)
        {
            case None:              { return "None"; }
            case SingleOpponent:    { return "SingleOpponent"; }
            case AllOpponents:      { return "AllOpponents"; }
            case SingleCompanion:   { return "SingleCompanion"; }
            case AllCompanions:     { return "AllCompanions"; }
            case Item:              { return "Item"; }
            case QuestSpecific:     { return "QuestSpecific"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::TargetType::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    const std::string TargetType::Name(const Enum E)
    {
        switch (E)
        {
            case None:              { return "(None)"; }
            case SingleOpponent:    { return "Single Opponent"; }
            case AllOpponents:      { return "All Opponents"; }
            case SingleCompanion:   { return "Single Companion"; }
            case AllCompanions:     { return "All Companions"; }
            case Item:              { return "Item"; }
            case QuestSpecific:     { return "(Quest Specific)"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::TargetType::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    const std::string TargetType::ActionPhrase(const Enum E)
    {
        auto const PREAMBLE{ ((misc::random::Bool()) ? "at " : "upon ") };

        std::ostringstream ss;
        switch (E)
        {
            case SingleOpponent:    { ss << "a single opponent"; break; }
            case AllOpponents:      { ss << "all opponent"; break; }
            case SingleCompanion:   { ss << "a single companion"; break; }
            case AllCompanions:     { ss << "all companion"; break; }
            case Item:              { ss << "an item"; break; }
            case QuestSpecific:
            case None:
            case Count:
            default:                { return ""; }
        }

        return PREAMBLE + ss.str();
    }

}