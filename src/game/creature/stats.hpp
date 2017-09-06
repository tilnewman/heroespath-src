///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef GAME_CREATURE_STATS_HPP_INCLUDED
#define GAME_CREATURE_STATS_HPP_INCLUDED
//
// stats.hpp
//
#include "game/stats/stat-set.hpp"
#include "game/creature/role-enum.hpp"
#include "game/creature/race-enum.hpp"


namespace game
{
namespace creature
{

    //forward declarations
    class Creature;
    using CreaturePtr_t = Creature *;


    //A collection of stat related creature functions.
    struct Stats
    {
        enum With : unsigned
        {
            None              = 0,
            Luck              = 1 << 0,
            RaceRoleBonus     = 1 << 1,
            RankBonus         = 1 << 2,
            PlayerNaturalWins = 1 << 3
        };

        static float Ratio(
            const CreaturePtr_t       CREATURE_PTR,
            const stats::Traits::Enum TRAIT_ENUM,
            const stats::Trait_t      TRAIT_BONUS = 0,
            const With                OPTIONS     = With::None);

        static float Ratio(
            const CreaturePtr_t        CREATURE_PTR,
            const stats::TraitsVec_t & TRAIT_ENUM_VEC,
            const stats::Trait_t       TRAIT_BONU     = 0,
            const With                 OPTIONS        = With::None);

        static stats::Trait_t Roll(
            const CreaturePtr_t       CREATURE_PTR,
            const stats::Traits::Enum TRAIT_ENUM,
            const With                OPTIONS      = With::None);

        static stats::Trait_t Roll(
            const CreaturePtr_t        CREATURE_PTR,
            const stats::TraitsVec_t & TRAIT_ENUM_VEC,
            const With                 OPTIONS        = With::None);

        static bool Test(
            const CreaturePtr_t       CREATURE_PTR,
            const stats::Traits::Enum TRAIT_ENUM,
            const float               RANK_BONUS_RATIO = 0.0f,
            const With                OPTIONS          = With::None);

        static bool Test(
            const CreaturePtr_t        CREATURE_PTR,
            const stats::TraitsVec_t & TRAIT_ENUM_VEC,
            const float                RANK_BONUS_RATIO = 0.0f,
            const With                 OPTIONS          = With::None);

        //You can leave DEFENDER_STAT stats::Traits::Count to mean the same as CHALLENGER_STAT
        static bool Versus(
            const CreaturePtr_t       CHALLENGER_PTR,
            const stats::Traits::Enum CHALLENGER_STAT,
            const CreaturePtr_t       DEFENDER_PTR,
            const stats::Traits::Enum DEFENDER_STAT        = stats::Traits::Count,
            const stats::Trait_t      CHALLENGER_BONUS_PER = 0,
            const stats::Trait_t      DEFENDER_BONUS_PER   = 0,
            const With                OPTIONS              = With::None);

        //You can leave the DEFENDER_STAT_VEC empty to mean both vecs are the same.
        static bool Versus(
            const CreaturePtr_t        CHALLENGER_PTR,
            const stats::TraitsVec_t & CHALLENGER_STAT_VEC,
            const CreaturePtr_t        DEFENDER_PTR,
            const stats::TraitsVec_t & DEFENDER_STAT_VEC_PARAM = stats::TraitsVec_t(),
            const stats::Trait_t       CHALLENGER_BONUS_PER    = 0,
            const stats::Trait_t       DEFENDER_BONUS_PER      = 0,
            const With                 OPTIONS                 = With::None);

        static int RandomRatio(
            const CreaturePtr_t       CREATURE_PTR,
            const stats::Traits::Enum TRAIT_ENUM,
            const int                 RAND_SPREAD,
            const float               RANK_BONUS_MULT = 0.0f,
            const stats::Trait_t      TRAIT_BONUS     = 0,
            const With                OPTIONS         = With::None);

    private:
        static stats::Trait_t LuckBonus(const CreaturePtr_t CREATURE_PTR);

        static stats::Trait_t RollBonusByRace(const stats::Trait_t      STAT_VALUE,
                                              const stats::Traits::Enum TRAIT_ENUM,
                                              const race::Enum          RACE_ENUM);

        static stats::Trait_t RollBonusByRole(const stats::Trait_t      STAT_VALUE,
                                              const stats::Traits::Enum TRAIT_ENUM,
                                              const role::Enum          ROLE_ENUM);

    };

}
}

#endif //GAME_CREATURE_STATS_HPP_INCLUDED
