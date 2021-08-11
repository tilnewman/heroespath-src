// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// sfx-set-enum.cpp
//
#include "sfx-set-enum.hpp"

#include "misc/log-macros.hpp"

namespace heroespath
{
namespace gui
{

    const std::string sound_effect_set::ToString(const sound_effect_set::Enum ENUM)
    {
        switch (ENUM)
        {
            case Prompt:
            {
                return "Prompt";
            }
            case Switch:
            {
                return "Switch";
            }
            case TickOn:
            {
                return "TickOn";
            }
            case TickOff:
            {
                return "TickOff";
            }
            case Thock:
            {
                return "Thock";
            }
            case Coin:
            {
                return "Coin";
            }
            case Gem:
            {
                return "Gem";
            }
            case MeteorShard:
            {
                return "MeteorShard";
            }
            case ItemGive:
            {
                return "ItemGive";
            }
            case ItemDrop:
            {
                return "ItemDrop";
            }
            case BlowpipeShoot:
            {
                return "BlowpipeShoot";
            }
            case BlowpipeHit:
            {
                return "BlowpipeHit";
            }
            case ArrowShoot:
            {
                return "ArrowShoot";
            }
            case ArrowHit:
            {
                return "ArrowHit";
            }
            case ProjectileMiss:
            {
                return "ProjectileMiss";
            }
            case MeleeMiss:
            {
                return "MeleeMiss";
            }
            case FistHit:
            {
                return "FistHit";
            }
            case TentacleHit:
            {
                return "TentacleHit";
            }
            case WhipHit:
            {
                return "WhipHit";
            }
            case WhipMiss:
            {
                return "WhipMiss";
            }
            case MaterialHitMetal:
            {
                return "MaterialHitMetal";
            }
            case MaterialHitMisc:
            {
                return "MaterialHitMisc";
            }
            case ClawHit:
            {
                return "ClawHit";
            }
            case Wind:
            {
                return "Wind";
            }
            case SpellSelect:
            {
                return "SpellSelect";
            }
            case SpellBandage:
            {
                return "SpellBandage";
            }
            case DrumBlip:
            {
                return "DrumBlip";
            }
            case GuitarStrum:
            {
                return "GuitarStrum";
            }
            case CombatWin:
            {
                return "CombatWin";
            }
            case CombatLose:
            {
                return "CombatLose";
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

} // namespace gui
} // namespace heroespath
