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
// condition-enum.cpp
//
#include "condition-enum.hpp"

#include "game/game-data-file.hpp"

#include <exception>
#include <sstream>


namespace game
{
namespace creature
{

    const std::string Conditions::ToString(const Conditions::Enum E)
    {
        switch (E)
        {
            case Conditions::Good:          { return "Good"; }
            case Conditions::Frightened:    { return "Frightened"; }
            case Conditions::Dazed:         { return "Dazed"; }
            case Conditions::Tripped:       { return "Tripped"; }
            case Conditions::AsleepNatural: { return "AsleepNatural"; }
            case Conditions::Poisoned:      { return "Poisoned"; }
            case Conditions::AsleepMagical: { return "AsleepMagical"; }
            case Conditions::Unconscious:   { return "Unconscious"; }
            case Conditions::Stone:         { return "Stone"; }
            case Conditions::Dead:          { return "Dead"; }
            case Conditions::Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::creature::Conditions::Enum::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    const std::string Conditions::Name(const Conditions::Enum E)
    {
        if (E == Conditions::AsleepNatural)
        {
            return "Asleep";
        }
        else if (E == Conditions::AsleepMagical)
        {
            return "Magical Sleep";
        }
        else
        {
            return ToString(E);
        }
    }


    const std::string Conditions::Desc(const Conditions::Enum E)
    {
        std::ostringstream keySS;
        keySS << "heroespath-creature-condition-" << ToString(E) << "-desc";
        return GameDataFile::Instance()->GetCopyStr(keySS.str());
    }


    namespace condition
    {
        //see comments in header
        const std::size_t Severity::ALL             {    0 };
        const std::size_t Severity::GOOD            {    1 };
        const std::size_t Severity::LEAST_BENEFITIAL{    2 };
        const std::size_t Severity::MOST_BENEFITIAL { 1000 };
        const std::size_t Severity::LEAST_HARMFUL   { 1001 };
        const std::size_t Severity::MOST_HARMFUL    { 2000 };


        //see comments in header
        std::size_t Severity::Get(const Conditions::Enum E)
        {
            switch (E)
            {
                case Conditions::Good:          { return GOOD; }
                case Conditions::Frightened:    { return LEAST_HARMFUL; }
                case Conditions::Dazed:         { return 1100; }
                case Conditions::Tripped:       { return 1200; }
                case Conditions::AsleepNatural: { return 1300; }
                case Conditions::Poisoned:      { return 1400; }
                case Conditions::AsleepMagical: { return 1500; }
                case Conditions::Unconscious:   { return 1800; }
                case Conditions::Stone:         { return 1900; }
                case Conditions::Dead:          { return MOST_HARMFUL; }
                case Conditions::Count:
                default:
                {
                    std::ostringstream ss;
                    ss << "game::creature::Conditions::Enum::Severity(" << E << ")_InvalidValueError.";
                    throw std::range_error(ss.str());
                }
            }
        }

    }

}
}