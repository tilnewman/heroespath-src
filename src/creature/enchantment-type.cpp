// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// enchantment-type.hpp
//
#include "enchantment-type.hpp"

#include "misc/enum-util.hpp"

namespace heroespath
{
namespace creature
{

    const std::string EnchantmentType::ToString(const Enum ENUM, const EnumStringHow & HOW)
    {
        return EnumUtil<EnchantmentType>::ToString(ENUM, HOW);
    }

    const std::string EnchantmentType::ToStringPopulate(
        const EnumUnderlying_t ENUM_VALUE, const std::string & SEPARATOR)
    {
        std::string str;
        str.reserve(128);
        AppendNameIfBitIsSet(str, ENUM_VALUE, EnchantmentType::WhenUsed, "when used:", SEPARATOR);
        AppendNameIfBitIsSet(str, ENUM_VALUE, EnchantmentType::WhenHeld, "when held:", SEPARATOR);

        AppendNameIfBitIsSet(
            str, ENUM_VALUE, EnchantmentType::WhenEquipped, "when equipped:", SEPARATOR);

        AppendNameIfBitIsSet(
            str, ENUM_VALUE, EnchantmentType::RemoveAfterUse, "RemoveAfterUse", SEPARATOR);

        AppendNameIfBitIsSet(
            str, ENUM_VALUE, EnchantmentType::BoundToNothing, "item not required", SEPARATOR);

        AppendNameIfBitIsSet(
            str, ENUM_VALUE, EnchantmentType::BoundToItem, "bound to the item", SEPARATOR);

        AppendNameIfBitIsSet(
            str, ENUM_VALUE, EnchantmentType::AllowsFlight, "allows flying", SEPARATOR);

        AppendNameIfBitIsSet(
            str,
            ENUM_VALUE,
            EnchantmentType::CurseWithoutItem,
            "allows casting curse without a cursed item",
            SEPARATOR);
        AppendNameIfBitIsSet(
            str,
            ENUM_VALUE,
            EnchantmentType::BlessWithoutItem,
            "allows casting bless without a blessed item",
            SEPARATOR);
        AppendNameIfBitIsSet(
            str,
            ENUM_VALUE,
            EnchantmentType::OnlyIfSetIsComplete,
            "only if all items in the set are equipped",
            SEPARATOR);

        return str;
    }

} // namespace creature
} // namespace heroespath
