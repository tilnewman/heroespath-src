// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_CREATURE_STATS_HPP_INCLUDED
#define HEROESPATH_CREATURE_STATS_HPP_INCLUDED
//
// stats.hpp
//
#include "creature/race-enum.hpp"
#include "creature/role-enum.hpp"
#include "misc/not-null.hpp"
#include "stats/stat-set.hpp"

namespace heroespath
{
namespace creature
{

    // forward declarations
    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;

    // A collection of stat related creature functions.
    struct Stats
    {
        enum With : unsigned
        {
            None = 0,
            Luck = 1 << 0,
            RaceRoleBonus = 1 << 1,
            RankBonus = 1 << 2,
            PlayerNaturalWins = 1 << 3,
            StandardBonus = 1 << 4
        };

        static float Ratio(
            const CreaturePtr_t CREATURE_PTR,
            const stats::Traits::Enum TRAIT_ENUM,
            const stats::Trait_t TRAIT_BONUS = 0,
            const With OPTIONS = With::None);

        static float Ratio(
            const CreaturePtr_t CREATURE_PTR,
            const stats::TraitsVec_t & TRAIT_ENUM_VEC,
            const stats::Trait_t TRAIT_BONU = 0,
            const With OPTIONS = With::None);

        static stats::Trait_t Roll(
            const CreaturePtr_t CREATURE_PTR,
            const stats::Traits::Enum TRAIT_ENUM,
            const With OPTIONS = With::None);

        static stats::Trait_t Roll(
            const CreaturePtr_t CREATURE_PTR,
            const stats::TraitsVec_t & TRAIT_ENUM_VEC,
            const With OPTIONS = With::None);

        static bool Test(
            const CreaturePtr_t CREATURE_PTR,
            const stats::Traits::Enum TRAIT_ENUM,
            const With OPTIONS = With::None);

        static bool Test(
            const CreaturePtr_t CREATURE_PTR,
            const stats::TraitsVec_t & TRAIT_ENUM_VEC,
            const With OPTIONS = With::None);

        // You can leave DEFENDER_STAT stats::Traits::Count to mean the same as CHALLENGER_STAT
        static bool Versus(
            const CreaturePtr_t CHALLENGER_PTR,
            const stats::Traits::Enum CHALLENGER_STAT,
            const CreaturePtr_t DEFENDER_PTR,
            const stats::Traits::Enum DEFENDER_STAT = stats::Traits::Count,
            const stats::Trait_t CHALLENGER_BONUS_PER = 0,
            const stats::Trait_t DEFENDER_BONUS_PER = 0,
            const With OPTIONS = With::None);

        // You can leave the DEFENDER_STAT_VEC empty to mean both vecs are the same.
        static bool Versus(
            const CreaturePtr_t CHALLENGER_PTR,
            const stats::TraitsVec_t & CHALLENGER_STAT_VEC,
            const CreaturePtr_t DEFENDER_PTR,
            const stats::TraitsVec_t & DEFENDER_STAT_VEC_PARAM = stats::TraitsVec_t(),
            const stats::Trait_t CHALLENGER_BONUS_PER = 0,
            const stats::Trait_t DEFENDER_BONUS_PER = 0,
            const With OPTIONS = With::None);

        static int RandomRatio(
            const CreaturePtr_t CREATURE_PTR,
            const stats::Traits::Enum TRAIT_ENUM,
            const int RAND_SPREAD,
            const float RANK_BONUS_MULT = 0.0f,
            const stats::Trait_t TRAIT_BONUS = 0,
            const With OPTIONS = With::None);

    private:
        static stats::Trait_t LuckBonus(const CreaturePtr_t CREATURE_PTR);

        static stats::Trait_t RollBonusByRace(
            const stats::Trait_t STAT_VALUE,
            const stats::Traits::Enum TRAIT_ENUM,
            const race::Enum RACE_ENUM);

        static stats::Trait_t RollBonusByRole(
            const stats::Trait_t STAT_VALUE,
            const stats::Traits::Enum TRAIT_ENUM,
            const role::Enum ROLE_ENUM);
    };

} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_STATS_HPP_INCLUDED
