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
#include "misc/config-file.hpp"

namespace heroespath
{
namespace creature
{

    RoleStatSetMap_t RoleStatModifier::roleStatSetMap_;

    const StatSet RoleStatModifier::Get(const role::Enum ENUM)
    {
        if (roleStatSetMap_.Empty())
        {
            roleStatSetMap_.Reserve(64);

            for (EnumUnderlying_t i(0); i < role::PlayerRoleCount; ++i)
            {
                const auto NEXT_ENUM { static_cast<role::Enum>(i) };
                const auto NEXT_ENUM_STR { NAMEOF_ENUM_STR(NEXT_ENUM) };

                const auto NEXT_KEY_STR { "creature-role-stat-modifiers-" + NEXT_ENUM_STR };

                const auto STAT_STR { misc::ConfigFile::Instance()->Value(NEXT_KEY_STR) };

                roleStatSetMap_[NEXT_ENUM] = StatModifierLoader::ConvertStringToStatSet(STAT_STR);
            }
        }

        return roleStatSetMap_[ENUM];
    }

} // namespace creature
} // namespace heroespath
