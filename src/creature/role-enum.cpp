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
// role-enum.cpp
//
#include "role-enum.hpp"

#include "game/game-data-file.hpp"

#include <exception>
#include <sstream>


namespace heroespath
{
namespace creature
{

    const std::string role::ToString(const role::Enum E)
    {
        switch (E)
        {
            case Beastmaster:   { return "Beastmaster"; }
            case Sorcerer:      { return "Sorcerer"; }
            case Cleric:        { return "Cleric"; }
            case Knight:        { return "Knight"; }
            case Archer:        { return "Archer"; }
            case Bard:          { return "Bard"; }
            case Thief:         { return "Thief"; }
            case Wolfen:        { return "Wolfen"; }
            case Firebrand:     { return "Firebrand"; }
            case Sylavin:       { return "Sylavin"; }
            //
            case Thug:          { return "Thug"; }
            case Mugger:        { return "Mugger"; }
            case Drunk:         { return "Drunk"; }
            case Brute:         { return "Brute"; }
            case Berserker:     { return "Berserker"; }
            case Mountain:      { return "Mountain"; }
            case Grunt:         { return "Grunt"; }
            case Captain:       { return "Captain"; }
            case Chieftain:     { return "Chieftain"; }
            case Trader:        { return "Trader"; }
            case Warlord:       { return "Warlord"; }
            case Shaman:        { return "Shaman"; }
            case Smasher:       { return "Smasher"; }
            case Strangler:     { return "Strangler"; }
            case Soldier:       { return "Soldier"; }
            case TwoHeaded:     { return "TwoHeaded"; }
            case Giant:         { return "Giant"; }
            case Elder:         { return "Elder"; }
            case FourArmed:     { return "FourArmed"; }
            case Tendrilus:     { return "Tendrilus"; }
            case Wing:          { return "Wing"; }
            case Whelp:         { return "Whelp"; }
            case Pod:           { return "Pod"; }
            case Spike:         { return "Spike"; }
            case Skeleton:      { return "Skeleton"; }
            case Ranger:        { return "Ranger"; }
            case Water:         { return "Water"; }
            case Blacksmith:    { return "Blacksmith"; }
            case Spider:        { return "Spider"; }
            case Beetle:        { return "Beetle"; }
            case Boar:          { return "Boar"; }
            case Lion:          { return "Lion"; }
            case Ramonaut:      { return "Ramonaut"; }
            case Serpent:       { return "Serpent"; }
            case Bat:           { return "Bat"; }
            case Ghost:         { return "Ghost"; }
            case Cat:           { return "Cat"; }
            case Wolf:          { return "Wolf"; }
            case Count:
            default:
            {
                std::ostringstream ss;

                ss << "creature::role::ToString(" << E
                    << ")_InvalidValueError.";

                throw std::range_error(ss.str());
            }
        }
    }


    const std::string role::Desc(const role::Enum E)
    {
        std::ostringstream ss;
        ss << "heroespath-creature-role-" << ToString(E) << "-desc";
        return game::GameDataFile::Instance()->GetCopyStr(ss.str());
    }


    const std::string role::Abbr(const role::Enum E)
    {
        if (E == Beastmaster) { return "Bsm"; }
        else if (E == Thief)  { return "Thf"; }
        else if (E == Wolfen) { return "Wlf"; }
        else
            return Name(E).substr(0, 3);
    }


    bool role::CanFly(const role::Enum E)
    {
        return ((E == role::Firebrand) ||
                (E == role::Sylavin) ||
                (E == role::Wing) ||
                (E == role::Whelp) ||
                (E == role::Bat));
    }


    bool role::WillInitiallyFly(const role::Enum E)
    {
        return ((E == role::Wing) ||
                (E == role::Whelp) ||
                (E == role::Bat));
    }


    combat::BlockingPosType::Enum role::BlockingPosType(const role::Enum E)
    {
        switch (E)
        {
            case role::Mountain:
            case role::Spider:
            case role::Beetle:
            case role::Boar:
            case role::Lion:
            case role::Ramonaut:
            case role::Serpent:
            case role::Whelp:
            case role::Pod:
            case role::Spike:
            case role::Skeleton:
            case role::Thug:
            case role::Mugger:
            case role::Drunk:
            case role::Grunt:
            case role::FourArmed:
            case role::Tendrilus:
            case role::TwoHeaded:
            case role::Giant:
            case role::Smasher:
            case role::Strangler:
            case role::Soldier:
            case role::Brute:
            case role::Berserker:
            case role::Knight:
            case role::Wolfen:
            case role::Firebrand:
            case role::Ranger:
            case role::Water:
            case role::Blacksmith:
            case role::Cat:
            case role::Wolf:
            case role::Sylavin: { return combat::BlockingPosType::Front; }
            
            case role::Ghost:
            case role::Bat:
            case role::Beastmaster:
            case role::Bard:
            case role::Archer:
            case role::Wing: { return combat::BlockingPosType::Support; }

            case role::Captain:
            case role::Warlord:
            case role::Chieftain: { return combat::BlockingPosType::Commanders; }

            case role::Thief: { return combat::BlockingPosType::Reluctant; }

            case role::Elder:
            case role::Shaman:
            case role::Sorcerer:
            case role::Cleric: { return combat::BlockingPosType::Casters; }

            case role::Trader: { return combat::BlockingPosType::Last; }
            
            case role::Count:
            default:
            {
                std::ostringstream ss;
                ss << "role::BlockingPosType(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    std::vector<role::Enum> role::RolesOfBlockingPosType(
        const combat::BlockingPosType::Enum BLOCKING_POS_ENUM)
    {
        std::vector<role::Enum> rolesVec;
        for (int i(0); i<role::Count; ++i)
        {
            auto const ROLE{ static_cast<role::Enum>(i) };
            if (BlockingPosType(ROLE) == BLOCKING_POS_ENUM)
            {
                rolesVec.push_back(ROLE);
            }
        }

        if (rolesVec.empty())
        {
            std::ostringstream ss;
            ss << "role::RolesOfBlockingPosType(" << BLOCKING_POS_ENUM << ") produced no roles.";
            throw std::range_error(ss.str());
        }

        return rolesVec;
    }

}
}
