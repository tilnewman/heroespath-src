#ifndef GAME_CREATURE_ACHIEVEMENT_ENUM_HPP_INCLUDED
#define GAME_CREATURE_ACHIEVEMENT_ENUM_HPP_INCLUDED
//
// achievement-enum.hpp
//
#include <string>
#include <vector>


namespace game
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

        static const std::string ToString(const Enum);
        static const std::string Name(const Enum);
    };

    using AchievementTypeVec_t = std::vector<AchievementType::Enum>;

}
}


#endif //GAME_CREATURE_ACHIEVEMENT_ENUM_HPP_INCLUDED