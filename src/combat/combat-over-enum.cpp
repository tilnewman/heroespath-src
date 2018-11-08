// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// combat-over-enum.cpp
//
#include "combat-over-enum.hpp"

#include "misc/log-macros.hpp"

namespace heroespath
{
namespace combat
{

    const std::string CombatEnd::ToString(const CombatEnd::Enum ENUM)
    {
        switch (ENUM)
        {
            case Win:
            {
                return "Win";
            }
            case Lose:
            {
                return "Lose";
            }
            case Ran:
            {
                return "Ran";
            }
            case Count:
            {
                return "(Count)";
            }
            default:
            {
                M_HP_LOG_ERR(
                    "enum_value=" << static_cast<EnumUnderlying_t>(ENUM) << " is invalid. (count="
                                  << static_cast<EnumUnderlying_t>(Count) << ")");

                return "";
            }
        }
    }

} // namespace combat
} // namespace heroespath
