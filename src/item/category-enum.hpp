// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_ITEM_CATEGORY_ENUM_HPP_INCLUDED
#define HEROESPATH_ITEM_CATEGORY_ENUM_HPP_INCLUDED
//
// category-enum.hpp
//
#include "misc/enum-common.hpp"

#include <string>

namespace heroespath
{
namespace item
{

    struct Category : public EnumBaseBitField<>
    {
        enum Enum : EnumUnderlying_t
        {
            None = 0, // valid items will never have no Category flag bits set
            Broken = 1 << 0, // useless, unable to do anything, all magic/enchantments fail
            Useable = 1 << 1,
            Equipable = 1 << 2,
            Wearable = 1 << 3,
            OneHanded = 1 << 4,
            TwoHanded = 1 << 5,
            ConsumedOnUse = 1 << 6,
            ShowsEnemyInfo = 1 << 7
        };

        static constexpr Enum Last = ShowsEnemyInfo;

        static const std::string Name(const Enum ENUM, const EnumStringHow & HOW = EnumStringHow());
    };

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_CATEGORY_ENUM_HPP_INCLUDED
