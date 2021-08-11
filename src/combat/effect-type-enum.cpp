// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// effect-type-enum.cpp
//
#include "effect-type-enum.hpp"

#include "misc/log-macros.hpp"

namespace heroespath
{
namespace combat
{

    const std::string combat::EffectType::ToString(const Enum ENUM)
    {
        switch (ENUM)
        {
            case CreatureHarmDamage:
            {
                return "CreatureHarmDamage";
            }
            case CreatureHarmMisc:
            {
                return "CreatureHarmMisc";
            }
            case CreatureHelpHeal:
            {
                return "CreatureHelpHeal";
            }
            case CreatureHelpMisc:
            {
                return "CreatureHelpMisc";
            }
            case ItemHarmBreak:
            {
                return "ItemHarmBreak";
            }
            case ItemHarmMisc:
            {
                return "ItemHarmMisc";
            }
            case ItemHelpFix:
            {
                return "ItemHelpFix";
            }
            case ItemHelpMisc:
            {
                return "ItemHelpMisc";
            }
            case Misc:
            {
                return "Misc";
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

    const std::string combat::EffectType::Name(const Enum ENUM)
    {
        switch (ENUM)
        {
            case CreatureHarmDamage:
            {
                return "Creature Damage";
            }
            case CreatureHarmMisc:
            {
                return "Creature Harm";
            }
            case CreatureHelpHeal:
            {
                return "Creature Heal";
            }
            case CreatureHelpMisc:
            {
                return "Creature Help";
            }
            case ItemHarmBreak:
            {
                return "Item Break";
            }
            case ItemHarmMisc:
            {
                return "Item Harm";
            }
            case ItemHelpFix:
            {
                return "Item Fix";
            }
            case ItemHelpMisc:
            {
                return "Item Help";
            }
            case Misc:
            {
                return "Misc";
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
