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
// enchantment-type.hpp
//
#include "enchantment-type.hpp"

namespace heroespath
{
namespace creature
{
    void EnchantmentType::ToStringPopulate(
        std::ostringstream & ss,
        const misc::EnumUnderlying_t ENUM_VALUE,
        const std::string & SEPARATOR)
    {
        AppendNameIfBitIsSet(ss, ENUM_VALUE, EnchantmentType::WhenUsed, "when used:", SEPARATOR);
        AppendNameIfBitIsSet(ss, ENUM_VALUE, EnchantmentType::WhenHeld, "when held:", SEPARATOR);

        AppendNameIfBitIsSet(
            ss, ENUM_VALUE, EnchantmentType::WhenEquipped, "when equipped:", SEPARATOR);

        AppendNameIfBitIsSet(
            ss, ENUM_VALUE, EnchantmentType::RemoveAfterUse, "RemoveAfterUse", SEPARATOR);

        AppendNameIfBitIsSet(
            ss, ENUM_VALUE, EnchantmentType::BoundToNothing, "item not required", SEPARATOR);

        AppendNameIfBitIsSet(
            ss, ENUM_VALUE, EnchantmentType::BoundToItem, "bound to the item", SEPARATOR);

        AppendNameIfBitIsSet(
            ss, ENUM_VALUE, EnchantmentType::AllowsFlight, "allows flying", SEPARATOR);

        AppendNameIfBitIsSet(
            ss,
            ENUM_VALUE,
            EnchantmentType::CurseWithoutItem,
            "allows casting curse without a cursed item",
            SEPARATOR);
        AppendNameIfBitIsSet(
            ss,
            ENUM_VALUE,
            EnchantmentType::BlessWithoutItem,
            "allows casting bless without a blessed item",
            SEPARATOR);
        AppendNameIfBitIsSet(
            ss,
            ENUM_VALUE,
            EnchantmentType::OnlyIfSetIsComplete,
            "only if all items in the set are equipped",
            SEPARATOR);
    }

} // namespace creature
} // namespace heroespath
