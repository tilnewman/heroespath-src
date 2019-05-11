// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// role-enum.cpp
//
#include "role-enum.hpp"

#include "misc/config-file.hpp"
#include "misc/enum-util.hpp"
#include "misc/log-macros.hpp"

namespace heroespath
{
namespace creature
{

    const std::string role::Desc(const role::Enum ENUM)
    {
        return misc::ConfigFile::Instance()->Value(
            "creature-role-" + NAMEOF_ENUM_STR(ENUM) + "-desc");
    }

    std::vector<role::Enum>
        role::RolesOfBlockingPosType(const combat::BlockingPosType::Enum BLOCKING_POS_ENUM)
    {
        std::vector<role::Enum> rolesVec;
        for (EnumUnderlying_t i(0); i < role::Count; ++i)
        {
            const auto ROLE { static_cast<role::Enum>(i) };
            if (BlockingPosType(ROLE) == BLOCKING_POS_ENUM)
            {
                rolesVec.emplace_back(ROLE);
            }
        }

        if (rolesVec.empty())
        {
            M_HP_LOG_ERR(
                "role::RolesOfBlockingPosType(" << BLOCKING_POS_ENUM << ") produced no roles.");
        }

        return rolesVec;
    }

} // namespace creature
} // namespace heroespath
