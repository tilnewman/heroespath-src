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
#ifndef GAME_ITEM_WEAPONTYPES_HPP_INCLUDED
#define GAME_ITEM_WEAPONTYPES_HPP_INCLUDED
//
// weapons-types.hpp
//  A set of enumerations that together define a weapon.
//
#include <string>


namespace game
{
namespace item
{
namespace weapon
{

    struct sword_type
    {
        enum Enum
        {
            Shortsword = 0,
            Gladius,
            Cutlass,
            Falcata,
            Rapier,
            Saber,
            Broadsword,
            Longsword,
            Knightlysword,
            Flamberg,
            Claymore,
            Count
        };

        static const std::string ToString(const sword_type::Enum);
    };


    struct axe_type
    {
        enum Enum
        {
            Handaxe = 0,
            Sickle,
            Battleaxe,
            Waraxe,
            Count
        };

        static const std::string ToString(const axe_type::Enum);
    };


    struct club_type
    {
        enum Enum
        {
            Spiked = 0,
            Maul,
            Mace,
            Warhammer,
            Count
        };

        static const std::string ToString(const club_type::Enum);
    };


    struct whip_type
    {
        enum Enum
        {
            Bullwhip = 0,
            Flail,
            MaceAndChain,
            Count
        };

        static const std::string ToString(const whip_type::Enum);
    };


    struct projectile_type
    {
        enum Enum
        {
            Blowpipe = 0,
            Sling,
            Shortbow,
            Longbow,
            Crossbow,
            CompositeBow,
            Count
        };

        static const std::string ToString(const projectile_type::Enum);
    };


    struct bladedstaff_type
    {
        enum Enum
        {
            ShortSpear = 0,
            Spear,
            Scythe,
            Pike,
            Partisan,
            Halberd,
            Count
        };

        static const std::string ToString(const bladedstaff_type::Enum);
    };

}
}
}
#endif //GAME_ITEM_WEAPONTYPES_HPP_INCLUDED
