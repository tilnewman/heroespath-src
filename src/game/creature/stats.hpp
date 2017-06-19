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
#include "game/stats/stat-enum.hpp"
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
        static float Ratio(
            const CreaturePtr_t     CREATURE_PTR,
            const stats::stat::Enum STAT_ENUM,
            const bool              WILL_INCLUDE_STANDARD_OFFSET = false,
            const bool              WILL_INCLUDE_LUCK            = false,
            const bool              WILL_INCLUDE_RACEROLE_BONUS  = false);

        static float Ratio(
            const CreaturePtr_t          CREATURE_PTR,
            const stats::StatEnumVec_t & STAT_ENUM_VEC,
            const bool                   WILL_INCLUDE_STANDARD_OFFSET = false,
            const bool                   WILL_INCLUDE_LUCK            = false,
            const bool                   WILL_INCLUDE_RACEROLE_BONUS  = false);

        static stats::Stat_t RatioOfStat(
            const CreaturePtr_t     CREATURE_PTR,
            const stats::stat::Enum STAT_ENUM,
            const bool              WILL_INCLUDE_STANDARD_OFFSET = false,
            const bool              WILL_INCLUDE_LUCK            = false,
            const bool              WILL_INCLUDE_RACEROLE_BONUS  = false);

        static stats::Stat_t RatioOfStat(
            const CreaturePtr_t          CREATURE_PTR,
            const stats::StatEnumVec_t & STAT_ENUM_VEC,
            const bool                   WILL_INCLUDE_STANDARD_OFFSET = false,
            const bool                   WILL_INCLUDE_LUCK            = false,
            const bool                   WILL_INCLUDE_RACEROLE_BONUS  = false);

        static bool Roll(
            const CreaturePtr_t     CREATURE_PTR,
            const stats::stat::Enum STAT_ENUM,
            const float             RANK_BONUS_RATIO             = 0.0f,
            const bool              WILL_INCLUDE_LUCK            = false,
            const bool              WILL_INCLUDE_RACEROLE_BONUS  = false);

        static bool Roll(
            const CreaturePtr_t          CREATURE_PTR,
            const stats::StatEnumVec_t & STAT_ENUM_VEC,
            const float                  RANK_BONUS_RATIO             = 0.0f,
            const bool                   WILL_INCLUDE_LUCK            = false,
            const bool                   WILL_INCLUDE_RACEROLE_BONUS  = false);

        //You can leave DEFENDER_STAT stats::stat::Count to mean the same as CHALLENGER_STAT
        static bool Versus(
            const CreaturePtr_t     CHALLENGER_PTR,
            const stats::stat::Enum CHALLENGER_STAT,
            const CreaturePtr_t     DEFENDER_PTR,
            const stats::stat::Enum DEFENDER_STAT                = stats::stat::Count,
            const stats::Stat_t     CHALLENGER_BONUS_PER         = 0,
            const stats::Stat_t     DEFENDER_BONUS_PER           = 0,
            const bool              WILL_INCLUDE_STANDARD_OFFSET = false,
            const bool              WILL_INCLUDE_RACEROLE_BONUS  = false,
            const bool              WILL_INCLUDE_RANK            = false,
            const bool              WILL_INCLUDE_PLAYER_LUCK     = false,
            const bool              ALLOW_PLAYER_NATURAL_WINS    = false);

        //You can leave the DEFENDER_STAT_VEC empty to mean both vecs are the same.
        static bool Versus(
            const CreaturePtr_t          CHALLENGER_PTR,
            const stats::StatEnumVec_t & CHALLENGER_STAT_VEC,
            const CreaturePtr_t          DEFENDER_PTR,
            const stats::StatEnumVec_t & DEFENDER_STAT_VEC_PARAM      = stats::StatEnumVec_t(),
            const stats::Stat_t          CHALLENGER_BONUS_PER         = 0,
            const stats::Stat_t          DEFENDER_BONUS_PER           = 0,
            const bool                   WILL_INCLUDE_STANDARD_OFFSET = false,
            const bool                   WILL_INCLUDE_RACEROLE_BONUS  = false,
            const bool                   WILL_INCLUDE_RANK            = false,
            const bool                   WILL_INCLUDE_PLAYER_LUCK     = false,
            const bool                   ALLOW_PLAYER_NATURAL_WINS    = false);

        static int RandomRatioWithFloorAndRankBonus(
            const CreaturePtr_t     CREATURE_PTR,
            const stats::stat::Enum STAT_ENUM,
            const int               RAND_SPREAD,
            const int               FLOOR_DIVISOR               = 0,//zero means 'no floor'
            const float             RANK_BONUS_MULT             = 0.0f,//zero means 'no rank bonus'
            const bool              WILL_INCLUDE_LUCK           = false,
            const bool              WILL_INCLUDE_RACEROLE_BONUS = false);
    
    private:
        static stats::Stat_t LuckBonus(const CreaturePtr_t CREATURE_PTR);

        static stats::Stat_t RollBonusByRace(const stats::Stat_t     STAT_VALUE,
                                             const stats::stat::Enum STAT_ENUM,
                                             const race::Enum        RACE_ENUM);

        static stats::Stat_t RollBonusByRole(const stats::Stat_t     STAT_VALUE,
                                             const stats::stat::Enum STAT_ENUM,
                                             const role::Enum        ROLE_ENUM);

    };

}
}

#endif //GAME_CREATURE_STATS_HPP_INCLUDED
