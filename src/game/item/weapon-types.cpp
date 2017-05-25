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
// weapon-types.cpp
//
#include "weapon-types.hpp"

#include <exception>
#include <sstream>


namespace game
{
namespace item
{
namespace weapon
{

    const std::string sword_type::ToString(const sword_type::Enum E)
    {
        switch(E)
        {
            case Claymore:      { return "Claymore"; }
            case Longsword:     { return "Longsword"; }
            case Flamberg:      { return "Flamberg"; }
            case Knightlysword: { return "KnightlySword"; }
            case Broadsword:    { return "Broadsword"; }
            case Falcata:       { return "Falcata"; }
            case Saber:         { return "Saber"; }
            case Cutlass:       { return "Cutlass"; }
            case Rapier:        { return "Rapier"; }
            case Gladius:       { return "Gladius"; }
            case Shortsword:    { return "Shortsword"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::item::weapon::sword_type::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    const std::string axe_type::ToString(const axe_type::Enum E)
    {
        switch(E)
        {
            case Handaxe:   { return "Handaxe"; }
            case Sickle:    { return "Sickle"; }
            case Battleaxe: { return "Battleaxe"; }
            case Waraxe:    { return "Waraxe"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::item::weapon::axe_type::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    const std::string club_type::ToString(const club_type::Enum E)
    {
        switch(E)
        {
            case Spiked:    { return "Spiked"; }
            case Maul:      { return "Maul"; }
            case Mace:      { return "Mace"; }
            case Warhammer: { return "Warhammer"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::item::weapon::club_type::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    const std::string whip_type::ToString(const whip_type::Enum E)
    {
        switch(E)
        {
            case Bullwhip:     { return "Bullwhip"; }
            case Flail:        { return "Flail"; }
            case MaceAndChain: { return "MaceAndChain"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::item::weapon::whip_type::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    const std::string projectile_type::ToString(const projectile_type::Enum E)
    {
        switch (E)
        {
            case Blowpipe:     { return "Blowpipe"; }
            case Sling:        { return "Sling"; }
            case Longbow:      { return "Longbow"; }
            case CompositeBow: { return "CompositeBow"; }
            case Crossbow:     { return "Crossbow"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::item::weapon::projectile_type::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    const std::string bladedstaff_type::ToString(const bladedstaff_type::Enum E)
    {
        switch (E)
        {
            case Spear:      { return "Spear"; }
            case ShortSpear: { return "ShortSpear"; }
            case Scythe:     { return "Scythe"; }
            case Pike:       { return "Pike"; }
            case Partisan:   { return "Partisan"; }
            case Halberd:    { return "Halberd"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::item::weapon::bladedstaff_type::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }

}
}
}
