// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_ITEM_TREASUREIMAGEENUM_HPP_INCLUDED
#define HEROESPATH_ITEM_TREASUREIMAGEENUM_HPP_INCLUDED
//
// treasure-image-enum.hpp
//
#include "misc/enum-common.hpp"

#include <string>

namespace heroespath
{
namespace item
{

    struct TreasureImage : public EnumBaseCounting<>
    {
        enum Enum : EnumUnderlying_t
        {
            BonePile = 0,
            ChestClosed,
            ChestOpen,
            LockboxClosed,
            LockboxOpen,
            Count
        };

        static const std::string_view ImageConfigKey(const Enum);

        static constexpr std::string_view
            ToContainerName(const TreasureImage::Enum IMAGE, const bool WILL_CAPITALIZE = false)
        {
            if ((IMAGE == ChestClosed) || (IMAGE == ChestOpen))
            {
                if (WILL_CAPITALIZE)
                {
                    return "Chest";
                }
                else
                {
                    return "chest";
                }
            }
            else if ((IMAGE == LockboxClosed) || (IMAGE == LockboxOpen))
            {
                if (WILL_CAPITALIZE)
                {
                    return "Lockbox";
                }
                else
                {
                    return "lockbox";
                }
            }
            else
            {
                return "";
            }
        }
    };

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_TREASUREIMAGEENUM_HPP_INCLUDED
