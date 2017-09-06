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
#ifndef GAME_CREATURE_CREATUREVECALGORITHMS_HPP_INCLUDED
#define GAME_CREATURE_CREATUREVECALGORITHMS_HPP_INCLUDED
//
// algorithms.hpp
//
#include "game/creature/race-enum.hpp"
#include "game/creature/role-enum.hpp"
#include "game/creature/condition-enum.hpp"

#include <string>
#include <vector>


namespace game
{
namespace creature
{

    //forward declarations
    class Creature;
    using CreaturePtr_t   = Creature *;
    using CreaturePtrC_t  = Creature * const;
    using CreatureCPtr_t  = const Creature *;
    using CreatureCPtrC_t = const Creature * const;
    using CreaturePVec_t  = std::vector<CreaturePtr_t>;


    struct Algorithms
    {
        static const CreaturePVec_t MakeIntoVector(const CreaturePtr_t);

        enum PlayerOpt : unsigned
        {
            None             = 0,
            Living           = 1 << 0,
            Runaway          = 1 << 1,
            LivingAndRunaway = Living | Runaway,
        };

        enum class TypeOpt
        {
            Player,
            NonPlayer
        };

        static std::size_t Players(
            CreaturePVec_t & pVec_OutParam,
            const PlayerOpt  OPTIONS = PlayerOpt::None);

        static const CreaturePVec_t Players(
            const PlayerOpt OPTIONS = PlayerOpt::None);

        static std::size_t NonPlayers(
            CreaturePVec_t & pVec_OutParam,
            const PlayerOpt  OPTIONS = PlayerOpt::None);

        static const CreaturePVec_t NonPlayers(
            const PlayerOpt OPTIONS = PlayerOpt::None);

        static std::size_t PlayersByType(
            CreaturePVec_t & pVec_OutParam,
            const TypeOpt    TYPE_OPTION,
            const PlayerOpt  PLAYER_OPTIONS = PlayerOpt::None);

        static const CreaturePVec_t PlayersByType(
            const TypeOpt    TYPE_OPTION,
            const PlayerOpt  OPTIONS = PlayerOpt::None);

        /*
        static std::size_t Players(CreaturePVec_t & pVec_OutParam,
                                   const bool       LIVING_ONLY      = false,
                                   const bool       INCLUDE_RUNAWAYS = false);

        static const CreaturePVec_t Players(const bool LIVING_ONLY      = false,
                                            const bool INCLUDE_RUNAWAYS = false);

        static std::size_t NonPlayers(CreaturePVec_t & pVec_OutParam,
                                      const bool       LIVING_ONLY      = false,
                                      const bool       INCLUDE_RUNAWAYS = false);

        static const CreaturePVec_t NonPlayers(const bool LIVING_ONLY      = false,
                                               const bool INCLUDE_RUNAWAYS = false);

        static std::size_t PlayersByType(CreaturePVec_t & pVec_OutParam,
                                         const bool       WILL_FIND_PLAYERS,
                                         const bool       LIVING_ONLY       = false,
                                         const bool       INCLUDE_RUNAWAYS  = false);

        static const CreaturePVec_t PlayersByType(const bool WILL_FIND_PLAYERS,
                                                  const bool LIVING_ONLY       = false,
                                                  const bool INCLUDE_RUNAWAYS  = false);*/

        static const std::string Names(const CreaturePVec_t &,
                                       const bool             WILL_WRAP              = false,
                                       const bool             WILL_APPEND_AND        = false,
                                       const bool             WILL_ADD_RACE_AND_ROLE = false,
                                       const std::size_t      MAX_COUNT              = 0,
                                       const bool             WILL_ELLIPSIS          = false);

        static const CreaturePVec_t FindByAlive(const CreaturePVec_t &, const bool WILL_FIND_ALIVE = true);

        static void SortByLowestHealthNormal(CreaturePVec_t &);
        static const CreaturePVec_t FindLowestHealthNormal(const CreaturePVec_t &);

        static void SortByLowestHealth(CreaturePVec_t &);
        static const CreaturePVec_t FindLowestHealth(const CreaturePVec_t &);

        static void SortByLowestHealthRatio(CreaturePVec_t &);
        static const CreaturePVec_t FindLowestHealthRatio(const CreaturePVec_t &);

        static const CreaturePVec_t FindByConditionMeaningNotAThreatPermenantly(
            const CreaturePVec_t &,
            const bool HAS_CONDITION = true,
            const bool WILL_INCLUDE_UNCONSCIOUS = false);

        static const CreaturePVec_t FindByConditionMeaningNotAThreatTemporarily(
            const CreaturePVec_t &,
            const bool HAS_CONDITION = true);

        static const CreaturePVec_t FindByRace(
            const CreaturePVec_t &,
            const race::Enum,
            const bool IS_GIVEN_RACE = true);

        static const CreaturePVec_t FindByRole(
            const CreaturePVec_t &,
            const role::Enum,
            const bool IS_GIVEN_ROLE = true);

        static const CreaturePVec_t FindBySpellCaster(
            const CreaturePVec_t &,
            const bool CAN_CAST = true);

        static const CreaturePVec_t FindByBeast(
            const CreaturePVec_t &,
            const bool IS_BEAST = true);

        static const CreaturePVec_t FindByProjectileWeapons(
            const CreaturePVec_t &,
            const bool IS_HOLDING_PROJ_WEAPON = true);

        static const CreaturePVec_t FindByCanFly(
            const CreaturePVec_t &,
            const bool CAN_FLY = true);

        static const CreaturePVec_t FindByFlying(
            const CreaturePVec_t &,
            const bool IS_FLYING = true);

        static const CreaturePVec_t FindByCanTakeAction(
            const CreaturePVec_t &,
            const bool CAN_TAKE_ACTION = true);

        static const CreaturePVec_t FindByCondition(
            const CreaturePVec_t &,
            const Conditions::Enum,
            const bool HAS_ALL = true,
            const bool HAS_CONDITION = true);

        static const CreaturePVec_t FindByCondition(
            const CreaturePVec_t &,
            const CondEnumVec_t &,
            const bool HAS_ALL = true,
            const bool HAS_CONDITION = true);
    };

}
}

#endif //GAME_CREATURE_CREATUREVECALGORITHMS_HPP_INCLUDED
