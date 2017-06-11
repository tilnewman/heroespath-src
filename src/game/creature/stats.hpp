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
        static float Ratio(const CreaturePtr_t     CREATURE_PTR,
                           const stats::stat::Enum STAT_ENUM);

        static float Ratio(const CreaturePtr_t          CREATURE_PTR,
                           const stats::StatEnumVec_t & STAT_ENUM_VEC);

        static bool Roll(const CreaturePtr_t     CREATURE_PTR,
                         const stats::stat::Enum STAT_ENUM,
                         const bool              WILL_CONSIDER_RANK = false);

        static bool Roll(const CreaturePtr_t          CREATURE_PTR,
                         const stats::StatEnumVec_t & STAT_ENUM_VEC,
                         const bool                   WILL_CONSIDER_RANK = false);

        //Returns true if the challenger wins the "roll".
        //You can leave the DEFENDER_STAT_VEC empty to mean both vecs are the same.
        static bool Versus(
            const CreaturePtr_t          CHALLENGER_PTR,
            const stats::StatEnumVec_t & CHALLENGER_STAT_VEC,
            const CreaturePtr_t          DEFENDER_PTR,
            const stats::StatEnumVec_t & DEFENDER_STAT_VEC_PARAM = stats::StatEnumVec_t(),
            const bool                   WILL_CONDSIDER_RANK = true,
            const bool                   ALLOW_PLAYER_NATURAL_WINS = true);
        
    private:
        static stats::Stat_t RollBonusByRace(const stats::stat::Enum STAT_ENUM,
                                             const race::Enum        RACE_ENUM);

        static stats::Stat_t RollBonusByRole(const stats::stat::Enum STAT_ENUM,
                                             const role::Enum        ROLE_ENUM);

    };

}
}

#endif //GAME_CREATURE_STATS_HPP_INCLUDED
