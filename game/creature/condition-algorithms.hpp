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
namespace condition
{

    //A collection of functions that operate on vectors of Conditions.
    struct Algorithms
    {
        static const std::string Names(const ConditionEnumVec_t & CONDITIONS_VEC,
                                       const std::string          SEPARATOR       = ", ",
                                       const bool                 WILL_WRAP       = false,
                                       const std::size_t          MAX_TO_LIST     = 0, //zero means 'all'
                                       const std::size_t          MIN_SEVERITY    = 0,
                                       const bool                 WILL_AND        = false,
                                       const bool                 WILL_ELLIPSIS   = false);

        static void SortBySeverity(ConditionEnumVec_t & conditionsVec,
                                   const bool           SORT_DESCENDING = false);

        static const ConditionEnumVec_t SortBySeverityCopy(const ConditionEnumVec_t & CONDITIONS_VEC,
                                                           const bool                 SORT_DESCENDING = false);


    };

}
}
}

#endif //HEROSPATH_CREATURE_CONDITIONALGORITHMS_HPP_INCLUDED
