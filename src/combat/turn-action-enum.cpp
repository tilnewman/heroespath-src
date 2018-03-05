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
// turn-state-enum.cpp
//
#include "turn-action-enum.hpp"
#include <exception>
#include <sstream>

namespace heroespath
{
namespace combat
{

    const std::string TurnAction::ToString(const TurnAction::Enum E)
    {
        switch (E)
        {
            case Nothing:
            {
                return "Nothing";
            }
            case Attack:
            {
                return "Attack";
            }
            case Block:
            {
                return "Block";
            }
            case Advance:
            {
                return "Advance";
            }
            case Retreat:
            {
                return "Retreat";
            }
            case Fly:
            {
                return "Fly";
            }
            case Land:
            {
                return "Land";
            }
            case Cast:
            {
                return "Cast";
            }
            case PlaySong:
            {
                return "PlaySong";
            }
            case SkyPounce:
            {
                return "SkyPounce";
            }
            case LandPounce:
            {
                return "LandPounce";
            }
            case Roar:
            {
                return "Roar";
            }
            case ChangeWeapon:
            {
                return "ChangeWeapon";
            }
            case Run:
            {
                return "Run";
            }
            case TreasureUnlock:
            {
                return "TreasureUnlock";
            }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "combat::TurnAction::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }

    const std::string TurnAction::Name(const TurnAction::Enum E)
    {
        switch (E)
        {
            case Nothing:
            {
                return "Does Nothing";
            }
            case Attack:
            {
                return "Attacks";
            }
            case Block:
            {
                return "Blocks";
            }
            case Advance:
            {
                return "Advances";
            }
            case Retreat:
            {
                return "Retreats";
            }
            case Fly:
            {
                return "Flies";
            }
            case Land:
            {
                return "Lands";
            }
            case Cast:
            {
                return "Casts a Spell";
            }
            case PlaySong:
            {
                return "Plays a Magical Song";
            }
            case SkyPounce:
            {
                return "Pounces from the Sky";
            }
            case LandPounce:
            {
                return "Pounces";
            }
            case Roar:
            {
                return "Roars";
            }
            case ChangeWeapon:
            {
                return "Changes Weapons";
            }
            case Run:
            {
                return "Runs Away";
            }
            case TreasureUnlock:
            {
                return "Springs the trap";
            }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "combat::TurnAction::Name(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }
} // namespace combat
} // namespace heroespath
