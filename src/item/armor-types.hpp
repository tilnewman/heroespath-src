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
#ifndef HEROESPATH_ITEM_ARMORTYPES_HPP_INCLUDED
#define HEROESPATH_ITEM_ARMORTYPES_HPP_INCLUDED
//
// armor-types.hpp
//  A set of enumerations that define a piece of armor.
//
#include "misc/enum-util.hpp"

#include <string>

namespace heroespath
{
namespace item
{
    namespace armor
    {

        struct shield_type : public misc::EnumBaseCounting<shield_type, misc::EnumFirstValueValid>
        {
            enum Enum : misc::EnumUnderlying_t
            {
                Buckler = 0,
                Kite,
                Heater,
                Pavis,
                Count
            };

            static const std::string ToString(const shield_type::Enum);
        };

        struct helm_type : public misc::EnumBaseCounting<helm_type, misc::EnumFirstValueValid>
        {
            enum Enum : misc::EnumUnderlying_t
            {
                Leather = 0,
                MailCoif,
                Kettle,
                Archers,
                Bascinet,
                Great,
                Count
            };

            static const std::string ToString(const helm_type::Enum);
            static const std::string Name(const helm_type::Enum E);
        };

        struct base_type : public misc::EnumBaseCounting<base_type, misc::EnumFirstValueValid>
        {
            enum Enum : misc::EnumUnderlying_t
            {
                Plain = 0,
                Mail,
                Scale,
                Plate,
                Count
            };

            static const std::string ToString(const base_type::Enum);
            static Enum FromString(const std::string & S)
            {
                return static_cast<Enum>(EnumBaseCounting::FromString(S));
            }
        };

        struct cover_type : public misc::EnumBaseCounting<cover_type, misc::EnumFirstValueValid>
        {
            enum Enum : misc::EnumUnderlying_t
            {
                Cape = 0,
                Vest,
                Robe,
                Cloak,
                Count
            };

            static const std::string ToString(const cover_type::Enum);
        };

    } // namespace armor
} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_ARMORTYPES_HPP_INCLUDED
