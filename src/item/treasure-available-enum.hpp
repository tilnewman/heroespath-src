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
#include "misc/enum-common.hpp"

namespace heroespath
{
namespace item
{

    struct TreasureAvailable : public EnumBaseCounting<>
    {
        enum Enum : EnumUnderlying_t
        {
            NoTreasure = 0,
            HeldOnly,
            LockboxOnly,
            HeldAndLockbox,
            Count
        };
    };

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_TREASUREAVAILABLEENUM_HPP_INCLUDED
