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
#ifndef HEROESPATH_CREATURE_ROLEENUM_HPP_INCLUDED
#define HEROESPATH_CREATURE_ROLEENUM_HPP_INCLUDED
//
// role-enum.hpp
//  An enumeration defining each Role of creature, both player and non_player.
//
#include <string>
#include <vector>


namespace heroespath
{
namespace creature
{

    struct role
    {
        enum Enum
        {
            Archer,
            Bard,
            Beastmaster,
            Cleric,
            Knight,
            Sorcerer,
            Thief,
            Firebrand,
            Sylavin,
            Wolfen,
            PlayerRoleCount,
            Thug = PlayerRoleCount,
            Mugger,
            Drunk,
            Grunt,
            Brute,
            Berserker,
            Mountain,
            Captain,
            Chieftain,
            Trader,
            Warlord,
            Shaman,
            Smasher,
            Strangler,
            Soldier,
            TwoHeaded,
            Giant,
            Elder,
            FourArmed,
            Tendrilus,
            Wing,
            Whelp,
            Pod,
            Spike,
            Skeleton,
            Ranger,
            Water,
            Blacksmith,
            Spider,
            Beetle,
            Boar,
            Lion,
            Ramonaut,
            Serpent,
            Bat,
            Ghost,
            Cat,
            Wolf,
            Count
        };

        static const std::string ToString(const role::Enum);
        inline static const std::string Name(const role::Enum E) { return ToString(E); }
        static const std::string Desc(const role::Enum);
        static const std::string Abbr(const role::Enum);
        static bool CanFly(const role::Enum);
        static bool WillInitiallyFly(const role::Enum);
    };

    using RoleVec_t = std::vector<role::Enum>;

}
}

#endif //HEROESPATH_CREATURE_ROLEENUM_HPP_INCLUDED
