#ifndef HEROESPATH_CREATURE_CREATUREVECALGORITHMS_HPP_INCLUDED
#define HEROESPATH_CREATURE_CREATUREVECALGORITHMS_HPP_INCLUDED
//
// algorithms.hpp
//
#include "heroespath/creature/race-enum.hpp"
#include "heroespath/creature/role-enum.hpp"

#include <string>
#include <vector>


namespace heroespath
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

        static std::size_t Players(CreaturePVec_t &, const bool LIVING_ONLY = false);
        static const CreaturePVec_t Players(const bool LIVING_ONLY = false);

        static std::size_t NonPlayers(CreaturePVec_t &, const bool LIVING_ONLY = false);
        static const CreaturePVec_t NonPlayers(const bool LIVING_ONLY = false);

        static std::size_t PlayersByType(CreaturePVec_t &, const bool WILL_FIND_PLAYERS);
        static const CreaturePVec_t PlayersByType(const bool WILL_FIND_PLAYERS);

        static const std::string Names(const CreaturePVec_t &, const bool WILL_WRAP = false, const bool WILL_APPEND_AND = false);

        static const CreaturePVec_t FindByAlive(const CreaturePVec_t &, const bool WILL_FIND_ALIVE = true);

        //appends B into A, stable unless WILL_MAKE_UNIQUE==true
        static void Append(CreaturePVec_t & a_PVec, const CreaturePVec_t & B_PVec, const bool WILL_MAKE_UNIQUE = false);

        static const CreaturePVec_t Exclude(const CreaturePVec_t & SOURCE_PVEC, CreaturePtr_t creatureToExcludePtr);
        static const CreaturePVec_t Exclude(const CreaturePVec_t & SOURCE_PVEC, const CreaturePVec_t & TO_EXCLUDE_PVEC);

        static const CreaturePtrC_t SelectRandom(const CreaturePVec_t &);
        static void Shuffle(CreaturePVec_t &);

        static void SortByLowestHealthNormal(CreaturePVec_t &);
        static const CreaturePVec_t FindLowestHealthNormal(const CreaturePVec_t &);

        static void SortByLowestHealth(CreaturePVec_t &);
        static const CreaturePVec_t FindLowestHealth(const CreaturePVec_t &);

        static void SortByLowestHealthRatio(CreaturePVec_t &);
        static const CreaturePVec_t FindLowestHealthRatio(const CreaturePVec_t &);

        static const CreaturePVec_t FindByConditionMeaningNotAThreatPermenantly(const CreaturePVec_t &, const bool HAS_CONDITION = true);
        static const CreaturePVec_t FindByConditionMeaningNotAThreatTemporarily(const CreaturePVec_t &, const bool HAS_CONDITION = true);

        static const CreaturePVec_t FindByRace(const CreaturePVec_t &, const race::Enum, const bool IS_GIVEN_RACE = true);
        static const CreaturePVec_t FindByRole(const CreaturePVec_t &, const role::Enum, const bool IS_GIVEN_ROLE = true);
        static const CreaturePVec_t FindBySpellCaster(const CreaturePVec_t &, const bool CAN_CAST = true);
        static const CreaturePVec_t FindByBeast(const CreaturePVec_t &, const bool IS_BEAST = true);
        static const CreaturePVec_t FindByProjectileWeapons(const CreaturePVec_t &, const bool IS_HOLDING_PROJ_WEAPON = true);
        static const CreaturePVec_t FindByCanFly(const CreaturePVec_t &, const bool CAN_FLY = true);
        static const CreaturePVec_t FindByFlying(const CreaturePVec_t &, const bool IS_FLYING = true);
        static const CreaturePVec_t FindByCanTakeAction(const CreaturePVec_t &, const bool CAN_TAKE_ACTION = true);
    };

}
}

#endif //HEROESPATH_CREATURE_CREATUREVECALGORITHMS_HPP_INCLUDED
