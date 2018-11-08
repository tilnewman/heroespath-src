// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// achievement.cpp
//
#include "achievement-enum.hpp"

#include "misc/log-macros.hpp"

namespace heroespath
{
namespace creature
{

    const std::string AchievementType::ToString(const AchievementType::Enum ENUM)
    {
        switch (ENUM)
        {
            case None:
            {
                return "(None)";
            }
            case EnemiesFaced:
            {
                return "EnemiesFaced";
            }
            case MeleeHits:
            {
                return "MeleeHits";
            }
            case BattlesSurvived:
            {
                return "BattlesSurvived";
            }
            case ProjectileHits:
            {
                return "ProjectileHits";
            }
            case BeastMindLinks:
            {
                return "BeastMindLinks";
            }
            case DodgedStanding:
            {
                return "DodgedStanding";
            }
            case DodgedFlying:
            {
                return "DodgedFlying";
            }
            case LocksPicked:
            {
                return "LocksPicked";
            }
            case BackstabsHits:
            {
                return "BackstabHits";
            }
            case SongsPlayed:
            {
                return "SongsPlayed";
            }
            case SpiritsLifted:
            {
                return "SpiritsLifted";
            }
            case BeastRoars:
            {
                return "BeastRoars";
            }
            case MoonHowls:
            {
                return "MoonHowls";
            }
            case PackActions:
            {
                return "PackActions";
            }
            case FlyingAttackHits:
            {
                return "FlyingAttackHits";
            }
            case TurnsInFlight:
            {
                return "TurnsInFlight";
            }
            case SpellsCast:
            {
                return "SpellsCast";
            }
            case HealthGiven:
            {
                return "HealthGiven";
            }
            case HealthTraded:
            {
                return "HealthTraded";
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

    const std::string AchievementType::Name(const AchievementType::Enum ENUM)
    {
        switch (ENUM)
        {
            case None:
            {
                return "(None)";
            }
            case EnemiesFaced:
            {
                return "Enemies Faced";
            }
            case MeleeHits:
            {
                return "Melee Hits";
            }
            case BattlesSurvived:
            {
                return "Battles Survived";
            }
            case ProjectileHits:
            {
                return "Projectile Hits";
            }
            case BeastMindLinks:
            {
                return "Beast Mind-Links";
            }
            case DodgedStanding:
            {
                return "Dodges While Standing";
            }
            case DodgedFlying:
            {
                return "Dodges While Flying";
            }
            case LocksPicked:
            {
                return "Locks Picked";
            }
            case BackstabsHits:
            {
                return "Backstab Hits";
            }
            case SongsPlayed:
            {
                return "Songs Played";
            }
            case SpiritsLifted:
            {
                return "Spirits Lifted";
            }
            case BeastRoars:
            {
                return "Beast Roars";
            }
            case MoonHowls:
            {
                return "Moon Howls";
            }
            case PackActions:
            {
                return "Pack Actions";
            }
            case FlyingAttackHits:
            {
                return "Flying Attack Hits";
            }
            case TurnsInFlight:
            {
                return "Turns In Flight";
            }
            case SpellsCast:
            {
                return "Spells Cast";
            }
            case HealthGiven:
            {
                return "Health Given";
            }
            case HealthTraded:
            {
                return "Health Traded";
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

} // namespace creature
} // namespace heroespath
