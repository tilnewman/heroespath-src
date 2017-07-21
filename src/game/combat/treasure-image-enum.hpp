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
#ifndef GAME_COMBAT_TREASUREIMAGEENUM_HPP_INCLUDED
#define GAME_COMBAT_TREASUREIMAGEENUM_HPP_INCLUDED
//
// treasure-image-enum.hpp
//
#include <string>


namespace game
{
namespace combat
{

    struct TreasureImage
    {
        enum Enum
        {
            BonePile = 0,
            ChestClosed,
            ChestOpen,
            LockboxClosed,
            LockboxOpen,
            Count
        };

        static const std::string ToString(const TreasureImage::Enum);
        static const std::string ToKey(const TreasureImage::Enum);
    };

}
}

#endif //GAME_COMBAT_TREASUREIMAGEENUM_HPP_INCLUDED
