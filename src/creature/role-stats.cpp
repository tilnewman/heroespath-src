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
// role-stats.cpp
//
#include "role-stats.hpp"

#include "creature/stat-modifier-loader.hpp"
#include "game/game-data-file.hpp"

namespace heroespath
{
namespace creature
{

    RoleStatSetMap_t RoleStatModifier::roleStatSetMap_;

    const stats::StatSet RoleStatModifier::Get(const role::Enum ENUM)
    {
        if (roleStatSetMap_.empty())
        {
            for (int i(0); i < role::PlayerRoleCount; ++i)
            {
                auto const NEXT_ENUM{ static_cast<role::Enum>(i) };
                auto const NEXT_ENUM_STR{ role::ToString(NEXT_ENUM) };
                auto const NEXT_KEY_STR{ "heroespath-creature-role-stat-modifiers-"
                                         + NEXT_ENUM_STR };
                std::string nextStatSetStr{ "" };
                game::GameDataFile::Instance()->GetStr(nextStatSetStr, NEXT_KEY_STR);
                roleStatSetMap_[NEXT_ENUM]
                    = StatModifierLoader::ConvertStringToStatSet(nextStatSetStr);
            }
        }

        return roleStatSetMap_[ENUM];
    }
} // namespace creature
} // namespace heroespath
