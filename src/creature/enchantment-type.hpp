// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_CREATURE_ENCHANTMENTTYPE_HPP_INCLUDED
#define HEROESPATH_CREATURE_ENCHANTMENTTYPE_HPP_INCLUDED
//
// enchantment-type.hpp
//  Defines special effects of an enchantment.
//
#include "misc/enum-util.hpp"

#include <string>

namespace heroespath
{
namespace creature
{

    struct EnchantmentType : misc::EnumBaseBitField<EnchantmentType>
    {
        enum Enum : misc::EnumUnderlying_t
        {
            None = 0,
            WhenUsed = 1 << 0,
            WhenHeld = 1 << 1,
            WhenEquipped = 1 << 2,
            RemoveAfterUse = 1 << 3,
            BoundToNothing = 1 << 4,
            BoundToItem = 1 << 5,
            AllowsFlight = 1 << 6,
            CurseWithoutItem = 1 << 7,
            BlessWithoutItem = 1 << 8,
            OnlyIfSetIsComplete = 1 << 9,
            Last = OnlyIfSetIsComplete
        };

        static const std::string ToStringPopulate(
            const misc::EnumUnderlying_t ENUM_VALUE, const std::string & SEPARATOR);
    };

} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_ENCHANTMENTTYPE_HPP_INCLUDED
