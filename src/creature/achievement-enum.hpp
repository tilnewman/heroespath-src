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
#include "misc/enum-util.hpp"

#include <string>
#include <vector>

namespace heroespath
{
namespace creature
{

    struct AchievementType
        : public misc::EnumBaseCounting<AchievementType, misc::EnumFirstValue::Valid>
    {
        enum Enum : misc::EnumUnderlying_t
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
            BackstabsHits,
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

        static const std::string ToString(const Enum);
        static const std::string Name(const Enum);
    };

    using AchievementTypeVec_t = std::vector<AchievementType::Enum>;

} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_ACHIEVEMENT_ENUM_HPP_INCLUDED
