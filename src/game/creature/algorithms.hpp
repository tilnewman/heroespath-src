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
#include "game/creature/creature.hpp"

#include "misc/vectors.hpp"

#include <string>
#include <vector>


namespace game
{
namespace creature
{

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

        enum class TypeOpt
        {
            Player,
            NonPlayer
        };

        static std::size_t PlayersByType(
            CreaturePVec_t & pVec_OutParam,
            const TypeOpt    TYPE_OPTION,
            const PlayerOpt  PLAYER_OPTIONS = PlayerOpt::None);

        static const CreaturePVec_t PlayersByType(
            const TypeOpt    TYPE_OPTION,
            const PlayerOpt  OPTIONS = PlayerOpt::None);

        enum class NamesOpt
        {
            NameOnly,
            WithRaceAndRole
        };

        static const std::string Names(
            const CreaturePVec_t &,
            const std::size_t           MAX_COUNT    = 0,
            const misc::Vector::JoinOpt JOIN_OPTIONS = misc::Vector::JoinOpt::None,
            const NamesOpt              OPTIONS      = NamesOpt::NameOnly);

        enum class CriteriaOpt
        {
            Meets,
            DoesNotMeet
        };

        static const CreaturePVec_t FindByAlive(
            const CreaturePVec_t &,
            const CriteriaOpt CRITERIA_OPTION = CriteriaOpt::Meets);

        static void SortByLowestHealthNormal(CreaturePVec_t &);
        static const CreaturePVec_t FindLowestHealthNormal(const CreaturePVec_t &);

        static void SortByLowestHealth(CreaturePVec_t &);
        static const CreaturePVec_t FindLowestHealth(const CreaturePVec_t &);

        static void SortByLowestHealthRatio(CreaturePVec_t &);
        static const CreaturePVec_t FindLowestHealthRatio(const CreaturePVec_t &);

        enum class CondSingleOpt
        {
            Has,
            DoesNotHave
        };

        static const CreaturePVec_t FindByConditionMeaningNotAThreatPermenantly(
            const CreaturePVec_t &,
            const CondSingleOpt      CONDITION_OPTION   = CondSingleOpt::Has,
            const creature::UnconOpt UNCONSCIOUS_OPTION = UnconOpt::Exclude);

        static const CreaturePVec_t FindByConditionMeaningNotAThreatTemporarily(
            const CreaturePVec_t &,
            const CondSingleOpt CONDITION_OPTION = CondSingleOpt::Has);

        static const CreaturePVec_t FindByRace(
            const CreaturePVec_t &,
            const race::Enum,
            const CriteriaOpt CRITERIA_OPTION = CriteriaOpt::Meets);

        static const CreaturePVec_t FindByRole(
            const CreaturePVec_t &,
            const role::Enum,
            const CriteriaOpt CRITERIA_OPTION = CriteriaOpt::Meets);

        static const CreaturePVec_t FindByCanCast(
            const CreaturePVec_t &,
            const CriteriaOpt CRITERIA_OPTION = CriteriaOpt::Meets);

        static const CreaturePVec_t FindByIsBeast(
            const CreaturePVec_t &,
            const CriteriaOpt CRITERIA_OPTION = CriteriaOpt::Meets);

        static const CreaturePVec_t FindByIsHoldingProjWeapon(
            const CreaturePVec_t &,
            const CriteriaOpt CRITERIA_OPTION = CriteriaOpt::Meets);

        static const CreaturePVec_t FindByCanFly(
            const CreaturePVec_t &,
            const CriteriaOpt CRITERIA_OPTION = CriteriaOpt::Meets);

        static const CreaturePVec_t FindByIsFlying(
            const CreaturePVec_t &,
            const CriteriaOpt CRITERIA_OPTION = CriteriaOpt::Meets);

        static const CreaturePVec_t FindByCanTakeAction(
            const CreaturePVec_t &,
            const CriteriaOpt CRITERIA_OPTION = CriteriaOpt::Meets);

        enum class CondAllOpt
        {
            HasAll,
            DoesNotHaveAll
        };

        static const CreaturePVec_t FindByHasCondition(
            const CreaturePVec_t &,
            const Conditions::Enum,
            const CondAllOpt    COND_ALL_OPTION    = CondAllOpt::HasAll,
            const CondSingleOpt COND_SINGLE_OPTION = CondSingleOpt::Has);

        static const CreaturePVec_t FindByHasConditions(
            const CreaturePVec_t &,
            const CondEnumVec_t &,
            const CondAllOpt    COND_ALL_OPTION    = CondAllOpt::HasAll,
            const CondSingleOpt COND_SINGLE_OPTION = CondSingleOpt::Has);
    };

}
}

#endif //GAME_CREATURE_CREATUREVECALGORITHMS_HPP_INCLUDED
