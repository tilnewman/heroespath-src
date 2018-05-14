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
#ifndef HEROESPATH_GAME_PHASEENUM_HPP_INCLUDED
#define HEROESPATH_GAME_PHASEENUM_HPP_INCLUDED
//
// phase-enum.hpp
//
#include "misc/enum-util.hpp"

#include <string>

namespace heroespath
{
namespace game
{

    struct Phase : misc::EnumBaseBitField<Phase>
    {
        enum Enum : misc::EnumUnderlying_t
        {
            None = 0,
            Combat = 1 << 0,
            Exploring = 1 << 1,
            Conversation = 1 << 2,
            Quest = 1 << 3,
            Inventory = 1 << 4,
            Last = Inventory
        };

        static void ToStringPopulate(
            std::ostringstream & ss,
            const misc::EnumUnderlying_t ENUM_VALUE,
            const std::string & SEPARATOR = ", ");
    };

} // namespace game
} // namespace heroespath

#endif // HEROESPATH_GAME_PHASEENUM_HPP_INCLUDED
