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

#include "misc/boost-string-includes.hpp"
#include "misc/enum-util.hpp"

namespace heroespath
{
namespace creature
{

    const std::string EnchantmentType::ToString(const Enum ENUM, const EnumStringHow & HOW)
    {
        return EnumUtil<EnchantmentType>::ToString(ENUM, HOW);
    }

    const std::string EnchantmentType::EffectStr(const Enum ENUM)
    {
        std::string str(ToString(ENUM, EnumStringHow(",", Wrap::No, NoneEmpty::No)));
        str.reserve(str.size() * 3);

        boost::algorithm::ireplace_all(str, ToString(WhenUsed), "when used:");

        boost::algorithm::ireplace_all(str, ToString(WhenHeld), "when held:");

        boost::algorithm::ireplace_all(str, ToString(WhenEquipped), "when equipped:");

        boost::algorithm::ireplace_all(str, ToString(RemoveAfterUse), "RemoveAfterUse");

        boost::algorithm::ireplace_all(str, ToString(BoundToNothing), "item not required");

        boost::algorithm::ireplace_all(str, ToString(BoundToItem), "bound to the item");

        boost::algorithm::ireplace_all(str, ToString(AllowsFlight), "allows flying");

        boost::algorithm::ireplace_all(
            str, ToString(CurseWithoutItem), "allows casting curse without a cursed item");

        boost::algorithm::ireplace_all(
            str, ToString(BlessWithoutItem), "allows casting bless without a blessed item");

        boost::algorithm::ireplace_all(
            str, ToString(OnlyIfSetIsComplete), "only if all items in the set are equipped");

        return str;
    }

    const std::string EnchantmentType::ToStringPopulate(
        const EnumUnderlying_t ENUM_VALUE, const std::string & SEPARATOR)
    {
        std::string str;
        str.reserve(128);
        AppendNameIfBitIsSet(str, ENUM_VALUE, EnchantmentType::WhenUsed, "WhenUsed", SEPARATOR);
        AppendNameIfBitIsSet(str, ENUM_VALUE, EnchantmentType::WhenHeld, "WhenHeld", SEPARATOR);
        AppendNameIfBitIsSet(
            str, ENUM_VALUE, EnchantmentType::WhenEquipped, "WhenEquipped", SEPARATOR);
        AppendNameIfBitIsSet(
            str, ENUM_VALUE, EnchantmentType::RemoveAfterUse, "RemoveAfterUse", SEPARATOR);
        AppendNameIfBitIsSet(
            str, ENUM_VALUE, EnchantmentType::BoundToNothing, "BoundToNothing", SEPARATOR);
        AppendNameIfBitIsSet(
            str, ENUM_VALUE, EnchantmentType::BoundToItem, "BoundToItem", SEPARATOR);
        AppendNameIfBitIsSet(
            str, ENUM_VALUE, EnchantmentType::AllowsFlight, "AllowsFlight", SEPARATOR);
        AppendNameIfBitIsSet(
            str, ENUM_VALUE, EnchantmentType::CurseWithoutItem, "CurseWithoutItem", SEPARATOR);
        AppendNameIfBitIsSet(
            str, ENUM_VALUE, EnchantmentType::BlessWithoutItem, "BlessWithoutItem", SEPARATOR);

        AppendNameIfBitIsSet(
            str,
            ENUM_VALUE,
            EnchantmentType::OnlyIfSetIsComplete,
            "OnlyIfSetIsComplete",
            SEPARATOR);

        return str;
    }

} // namespace creature
} // namespace heroespath
