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
// role.cpp
//
#include "role.hpp"

#include "game/creature/role-stats.hpp"

#include <boost/algorithm/string/erase.hpp>

#include <sstream>


namespace game
{
namespace creature
{

    const RoleVec_t Role::ROLEVEC_ALL_PLAYER_ROLES_{ role::Knight, role::Archer, role::Thief, role::Bard, role::Beastmaster, role::Wolfen, role::Firebrand, role::Sylavin, role::Cleric, role::Sorcerer };


    Role::Role(const role::Enum ROLE)
    :
        role_         (ROLE),
        statModifiers_(RoleStatModifier::Get(ROLE) )
    {}


    Role::~Role()
    {}


    const std::string Role::CharacterRoleString(const role::Enum E, const bool WILL_WRAP)
    {
        std::ostringstream ss;

        if (WILL_WRAP)
            ss << "(";

        if (E == role::PlayerRoleCount)
            ss << "ALL player character roles";
        else
            ss << role::Name(E);

        if (WILL_WRAP)
            ss << ")";

        return ss.str();
    }


    const std::string Role::CharacterRoleString(const RoleVec_t & ROLE_VEC, const bool WILL_WRAP)
    {
        const std::string SEP_STR(", ");

        std::ostringstream ss;

        if (WILL_WRAP)
            ss << "(";

        if ((ROLE_VEC.size() == 1) && (ROLE_VEC[0] == role::PlayerRoleCount))
        {
            ss << CharacterRoleString(ROLE_VEC[0], false);
        }
        else
        {
            for (auto const NEXT_ROLE_ENUM : ROLE_VEC)
                ss << role::Name(NEXT_ROLE_ENUM) << SEP_STR;
        }

        if (WILL_WRAP)
            ss << ")";

        return boost::algorithm::erase_all_copy(ss.str(), SEP_STR);
    }

}
}
