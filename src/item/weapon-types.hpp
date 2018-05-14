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
#ifndef HEROESPATH_ITEM_WEAPONTYPES_HPP_INCLUDED
#define HEROESPATH_ITEM_WEAPONTYPES_HPP_INCLUDED
//
// weapons-types.hpp
//  A set of enumerations that together define a weapon.
//
#include "misc/enum-util.hpp"

#include <string>

namespace heroespath
{
namespace item
{
    namespace weapon
    {

        struct sword_type : public misc::EnumBaseCounting<sword_type, misc::EnumFirstValueValid>
        {
            enum Enum : misc::EnumUnderlying_t
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
            static const std::string Name(const sword_type::Enum);
        };

        struct axe_type : public misc::EnumBaseCounting<axe_type, misc::EnumFirstValueValid>
        {
            enum Enum : misc::EnumUnderlying_t
            {
                Handaxe = 0,
                Sickle,
                Battleaxe,
                Waraxe,
                Count
            };

            static const std::string ToString(const axe_type::Enum);
            static const std::string Name(const axe_type::Enum);
        };

        struct club_type : public misc::EnumBaseCounting<club_type, misc::EnumFirstValueValid>
        {
            enum Enum : misc::EnumUnderlying_t
            {
                Spiked = 0,
                Maul,
                Mace,
                Warhammer,
                Count
            };

            static const std::string ToString(const club_type::Enum);
        };

        struct whip_type : public misc::EnumBaseCounting<whip_type, misc::EnumFirstValueValid>
        {
            enum Enum : misc::EnumUnderlying_t
            {
                Bullwhip = 0,
                Flail,
                MaceAndChain,
                Count
            };

            static const std::string ToString(const whip_type::Enum);
            static const std::string Name(const whip_type::Enum);
        };

        struct projectile_type
            : public misc::EnumBaseCounting<projectile_type, misc::EnumFirstValueValid>
        {
            enum Enum : misc::EnumUnderlying_t
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
            static const std::string Name(const projectile_type::Enum);
        };

        struct bladedstaff_type
            : public misc::EnumBaseCounting<bladedstaff_type, misc::EnumFirstValueValid>
        {
            enum Enum : misc::EnumUnderlying_t
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
            static const std::string Name(const bladedstaff_type::Enum);
        };

    } // namespace weapon
} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_WEAPONTYPES_HPP_INCLUDED
