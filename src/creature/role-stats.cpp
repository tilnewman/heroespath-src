// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
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

    const StatSet RoleStatModifier::Get(const role::Enum ENUM)
    {
        if (roleStatSetMap_.Empty())
        {
            for (misc::EnumUnderlying_t i(0); i < role::PlayerRoleCount; ++i)
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
