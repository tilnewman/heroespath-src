// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_CREATURE_ACHIEVEMENT_ENUM_HPP_INCLUDED
#define HEROESPATH_CREATURE_ACHIEVEMENT_ENUM_HPP_INCLUDED
//
// achievement-enum.hpp
//
#include "misc/enum-common.hpp"

#include <string>
#include <vector>

namespace heroespath
{
namespace creature
{

    struct AchievementType : public EnumBaseCounting<EnumFirstValue::Valid>
    {
        enum Enum : EnumUnderlying_t
        {
            None = 0,
            EnemiesFaced,
            MeleeHits,
            BattlesSurvived,
            ProjectileHits,
            BeastMindLinks,
            DodgedStanding,
            DodgedFlying,
            LocksPicked,
            BackstabHits,
            SongsPlayed,
            SpiritsLifted,
            BeastRoars,
            MoonHowls,
            PackActions,
            FlyingAttackHits,
            TurnsInFlight,
            SpellsCast,
            HealthGiven,
            HealthTraded,
            Count
        };

        static constexpr std::string_view Name(const AchievementType::Enum ENUM) noexcept
        {
            switch (ENUM)
            {
                case None: return "(None)";
                case EnemiesFaced: return "Enemies Faced";
                case MeleeHits: return "Melee Hits";
                case BattlesSurvived: return "Battles Survived";
                case ProjectileHits: return "Projectile Hits";
                case BeastMindLinks: return "Beast Mind-Links";
                case DodgedStanding: return "Dodges While Standing";
                case DodgedFlying: return "Dodges While Flying";
                case LocksPicked: return "Locks Picked";
                case BackstabHits: return "Backstab Hits";
                case SongsPlayed: return "Songs Played";
                case SpiritsLifted: return "Spirits Lifted";
                case BeastRoars: return "Beast Roars";
                case MoonHowls: return "Moon Howls";
                case PackActions: return "Pack Actions";
                case FlyingAttackHits: return "Flying Attack Hits";
                case TurnsInFlight: return "Turns In Flight";
                case SpellsCast: return "Spells Cast";
                case HealthGiven: return "Health Given";
                case HealthTraded: return "Health Traded";
                case Count:
                default: return "creature::AchievementType::Name(ENUM=out_of_range)";
            }
        }
    };

    using AchievementTypeVec_t = std::vector<AchievementType::Enum>;

} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_ACHIEVEMENT_ENUM_HPP_INCLUDED
