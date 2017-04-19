#ifndef HEROESPATH_CREATURE_ACHIEVEMENT_ENUM_HPP_INCLUDED
#define HEROESPATH_CREATURE_ACHIEVEMENT_ENUM_HPP_INCLUDED
//
// achievement-enum.hpp
//
#include <string>


namespace heroespath
{
namespace creature
{

    struct AchievementType
    {
        enum Enum
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

        static const std::string ToString(const AchievementType::Enum E);
        static const std::string Name(const AchievementType::Enum E);
    };

}
}


#endif //HEROESPATH_CREATURE_ACHIEVEMENT_ENUM_HPP_INCLUDED
