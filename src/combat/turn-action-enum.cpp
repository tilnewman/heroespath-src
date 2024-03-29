// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// turn-state-enum.cpp
//
#include "turn-action-enum.hpp"

#include "misc/log-macros.hpp"

namespace heroespath
{
namespace combat
{

    const std::string TurnAction::ToString(const TurnAction::Enum ENUM)
    {
        switch (ENUM)
        {
            case Nothing:
            {
                return "Nothing";
            }
            case Attack:
            {
                return "Attack";
            }
            case Block:
            {
                return "Block";
            }
            case Advance:
            {
                return "Advance";
            }
            case Retreat:
            {
                return "Retreat";
            }
            case Fly:
            {
                return "Fly";
            }
            case Land:
            {
                return "Land";
            }
            case Cast:
            {
                return "Cast";
            }
            case PlaySong:
            {
                return "PlaySong";
            }
            case SkyPounce:
            {
                return "SkyPounce";
            }
            case LandPounce:
            {
                return "LandPounce";
            }
            case Roar:
            {
                return "Roar";
            }
            case ChangeWeapon:
            {
                return "ChangeWeapon";
            }
            case Run:
            {
                return "Run";
            }
            case TreasureUnlock:
            {
                return "TreasureUnlock";
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

    const std::string TurnAction::Name(const TurnAction::Enum ENUM)
    {
        switch (ENUM)
        {
            case Nothing:
            {
                return "Does Nothing";
            }
            case Attack:
            {
                return "Attacks";
            }
            case Block:
            {
                return "Blocks";
            }
            case Advance:
            {
                return "Advances";
            }
            case Retreat:
            {
                return "Retreats";
            }
            case Fly:
            {
                return "Flies";
            }
            case Land:
            {
                return "Lands";
            }
            case Cast:
            {
                return "Casts a Spell";
            }
            case PlaySong:
            {
                return "Plays a Magical Song";
            }
            case SkyPounce:
            {
                return "Pounces from the Sky";
            }
            case LandPounce:
            {
                return "Pounces";
            }
            case Roar:
            {
                return "Roars";
            }
            case ChangeWeapon:
            {
                return "Changes Weapons";
            }
            case Run:
            {
                return "Runs Away";
            }
            case TreasureUnlock:
            {
                return "Springs the trap";
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
