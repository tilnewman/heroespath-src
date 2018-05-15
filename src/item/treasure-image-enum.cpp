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

#include "misc/random.hpp"

#include <exception>
#include <sstream>

namespace heroespath
{
namespace item
{

    const std::string TreasureImage::ToString(const TreasureImage::Enum E)
    {
        switch (E)
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
            default:
            {
                std::ostringstream ss;
                ss << "combat::TreasureImage::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }

    const std::string TreasureImage::ToImageKey(const TreasureImage::Enum E)
    {
        switch (E)
        {
            case BonePile:
            {
                auto const RAND{ misc::random::Int(2) };
                if (RAND == 0)
                {
                    return "media-images-bones-bone-pile-1";
                }
                else if (RAND == 1)
                {
                    return "media-images-bones-bone-pile-2";
                }
                else
                {
                    return "media-images-bones-bone-pile-3";
                }
            }
            case ChestClosed:
            {
                return "media-images-chest-closed";
            }
            case ChestOpen:
            {
                return "media-images-chest-open";
            }
            case LockboxClosed:
            {
                return "media-images-lockbox-closed";
            }
            case LockboxOpen:
            {
                return "media-images-lockbox-open";
            }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "combat::TreasureImage::ToKey(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }

    const std::string
        TreasureImage::ToContainerName(const TreasureImage::Enum E, const bool WILL_CAPITALIZE)
    {
        if ((E == ChestClosed) || (E == ChestOpen))
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
        else if ((E == LockboxClosed) || (E == LockboxOpen))
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
