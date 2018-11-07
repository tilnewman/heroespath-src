// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_ITEM_TREASUREAVAILABLEENUM_HPP_INCLUDED
#define HEROESPATH_ITEM_TREASUREAVAILABLEENUM_HPP_INCLUDED
//
// treasure-available-enum.hpp
//
#include "misc/enum-util.hpp"

#include <string>

namespace heroespath
{
namespace item
{

    struct TreasureAvailable
        : public misc::EnumBaseCounting<TreasureAvailable, misc::EnumFirstValue::Valid>
    {
        enum Enum : misc::EnumUnderlying_t
        {
            NoTreasure = 0,
            HeldOnly,
            LockboxOnly,
            HeldAndLockbox,
            Count
        };

        static const std::string ToString(const Enum);
    };

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_TREASUREAVAILABLEENUM_HPP_INCLUDED
