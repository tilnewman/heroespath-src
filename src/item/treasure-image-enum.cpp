// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// treasure-image-enum.cpp
//
#include "treasure-image-enum.hpp"

#include "misc/log-macros.hpp"
#include "misc/random.hpp"

namespace heroespath
{
namespace item
{

    const std::string TreasureImage::ToString(const TreasureImage::Enum IMAGE)
    {
        switch (IMAGE)
        {
            case BonePile:
            {
                return "BonePile";
            }
            case ChestClosed:
            {
                return "ChestClosed";
            }
            case ChestOpen:
            {
                return "ChestOpen";
            }
            case LockboxClosed:
            {
                return "LockboxClosed";
            }
            case LockboxOpen:
            {
                return "LockboxOpen";
            }
            case Count:
            {
                return "(Count)";
            }
            default:
            {
                M_HP_LOG_ERR(
                    "enum_value=" << static_cast<EnumUnderlying_t>(IMAGE) << " is invalid. (count="
                                  << static_cast<EnumUnderlying_t>(Count) << ")");

                return "";
            }
        }
    }

    const std::string TreasureImage::ImageConfigKey(const TreasureImage::Enum IMAGE)
    {
        switch (IMAGE)
        {
            case BonePile:
            {
                const auto RAND { misc::Random(2) };
                if (RAND == 0)
                {
                    return "media-image-bone-pile-bone-pile-1";
                }
                else if (RAND == 1)
                {
                    return "media-image-bone-pile-bone-pile-2";
                }
                else
                {
                    return "media-image-bone-pile-bone-pile-3";
                }
            }
            case ChestClosed:
            {
                return "media-image-misc-chest-closed";
            }
            case ChestOpen:
            {
                return "media-image-misc-chest-open";
            }
            case LockboxClosed:
            {
                return "media-image-misc-lockbox-closed";
            }
            case LockboxOpen:
            {
                return "media-image-misc-lockbox-open";
            }
            case Count:
            {
                return "(Count)";
            }
            default:
            {
                M_HP_LOG_ERR(
                    "enum_value=" << static_cast<EnumUnderlying_t>(IMAGE) << " is invalid. (count="
                                  << static_cast<EnumUnderlying_t>(Count) << ")");

                return "";
            }
        }
    }

    const std::string
        TreasureImage::ToContainerName(const TreasureImage::Enum IMAGE, const bool WILL_CAPITALIZE)
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
} // namespace item
} // namespace heroespath
