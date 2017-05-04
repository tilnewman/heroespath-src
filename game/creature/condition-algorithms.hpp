#ifndef HEROSPATH_CREATURE_CONDITIONALGORITHMS_HPP_INCLUDED
#define HEROSPATH_CREATURE_CONDITIONALGORITHMS_HPP_INCLUDED
//
// condition-algorithms.hpp
//
#include "game/creature/condition-enum.hpp"

#include <string>
#include <memory>
#include <vector>


namespace game
{
namespace creature
{
    //forward declarations
    class Condition;
    using ConditionSPtr_t = std::shared_ptr<Condition>;
    using ConditionSVec_t = std::vector<ConditionSPtr_t>;

namespace condition
{

    //A collection of functions that operate on vectors of Conditions.
    struct Algorithms
    {
        static const std::string Names(const ConditionSVec_t & CONDITIONS_SVEC,
                                       const std::string       SEPARATOR             = ", ",
                                       const bool              WILL_WRAP             = false,
                                       const std::size_t       MAX_TO_LIST           = 0, //zero means 'all'
                                       const std::size_t       MIN_SEVERITY          = 0,
                                       const bool              WILL_AND              = false);

        //stable unless WILL_UNIQUE == true
        static void Append(const ConditionSVec_t & CONDITIONS_TO_ADD_SVEC,
                           ConditionSVec_t &       conditionsToAddToSVec,
                           const bool              WILL_UNIQUE = false);

        //stable unless WILL_UNIQUE == true
        static const ConditionSVec_t AppendCopy(const ConditionSVec_t & CONDITIONS_A_SVEC,
                                                const ConditionSVec_t & CONDITIONS_B_SVEC,
                                                const bool              WILL_UNIQUE = false);

        static const ConditionSVec_t Exclude(const ConditionSVec_t & CONDITIONS_SVEC,
                                             const condition::Enum   ENUM);

        static const ConditionSVec_t Exclude(const ConditionSVec_t &    CONDITIONS_SVEC,
                                             const ConditionEnumVec_t & ENUM_VEC);

        static void SortBySeverity(ConditionSVec_t & conditionsSVec,
                                   const bool        SORT_DESCENDING = false);

        static const ConditionSVec_t SortBySeverityCopy(const ConditionSVec_t & CONDITIONS_SVEC,
                                                        const bool              SORT_DESCENDING = false);


    };

}
}
}

#endif //HEROSPATH_CREATURE_CONDITIONALGORITHMS_HPP_INCLUDED
