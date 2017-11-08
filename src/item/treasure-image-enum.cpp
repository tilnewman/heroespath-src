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
// treasure-image-enum.cpp
//
#include "treasure-image-enum.hpp"

#include "misc/random.hpp"

#include <sstream>
#include <exception>


namespace heroespath
{
namespace item
{

    const std::string TreasureImage::ToString(const TreasureImage::Enum E)
    {
        switch (E)
        {
            case BonePile:      { return "BonePile"; }
            case ChestClosed:   { return "ChestClosed"; }
            case ChestOpen:     { return "ChestOpen"; }
            case LockboxClosed: { return "LockboxClosed"; }
            case LockboxOpen:   { return "LockboxOpen"; }
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
            case ChestClosed:   { return "media-images-chest-closed"; }
            case ChestOpen:     { return "media-images-chest-open"; }
            case LockboxClosed: { return "media-images-lockbox-closed"; }
            case LockboxOpen:   { return "media-images-lockbox-open"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "combat::TreasureImage::ToKey(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    const std::string TreasureImage::ToContainerName(
        const TreasureImage::Enum E,
        const bool WILL_CAPITALIZE)
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

}
}
