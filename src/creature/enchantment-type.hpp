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
#ifndef HEROESPATH_CREATURE_ENCHANTMENTTYPE_HPP_INCLUDED
#define HEROESPATH_CREATURE_ENCHANTMENTTYPE_HPP_INCLUDED
//
// enchantment-type.hpp
//  Defines special effects of an enchantment.
//
#include <string>

namespace heroespath
{
namespace creature
{

    struct EnchantmentType
    {
        enum Enum : unsigned int
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
            OnlyIfSetIsComplete = 1 << 9
        };

        static const std::string ToString(const Enum);
    };
} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_ENCHANTMENTTYPE_HPP_INCLUDED
