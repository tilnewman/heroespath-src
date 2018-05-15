// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROSPATH_CREATURE_CONDITIONALGORITHMS_HPP_INCLUDED
#define HEROSPATH_CREATURE_CONDITIONALGORITHMS_HPP_INCLUDED
//
// condition-algorithms.hpp
//
#include "creature/condition-enum.hpp"
#include "misc/vectors.hpp"

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace creature
{
    namespace condition
    {

        // A collection of functions that operate on vectors of Conditions.
        struct Algorithms
        {
            enum class SortOpt
            {
                Ascending,
                Descending
            };

            static const std::string Names(
                const CondEnumVec_t & CONDITIONS_VEC,
                const std::size_t MAX_TO_LIST = 0,
                const std::size_t MIN_SEVERITY = 0,
                const SortOpt SORT_OPTION = SortOpt::Ascending,
                const misc::Vector::JoinOpt JOIN_OPTIONS = misc::Vector::JoinOpt::None);

            // Note:  Each Condition has a unique Severity so these sorts are complete.
            static void SortBySeverity(
                CondEnumVec_t & conditionsVec, const SortOpt SORT_OPTION = SortOpt::Descending);

            static const CondEnumVec_t SortBySeverityCopy(
                const CondEnumVec_t & CONDITIONS_VEC,
                const SortOpt SORT_OPTION = SortOpt::Descending);

            static void
                RemoveByMinSeverity(CondEnumVec_t & conditionsVec, const std::size_t MIN_SEVERITY);

            static const CondEnumVec_t RemoveByMinSeverityCopy(
                const CondEnumVec_t & CONDITIONS_VEC, const std::size_t MIN_SEVERITY);
        };
    } // namespace condition
} // namespace creature
} // namespace heroespath

#endif // HEROSPATH_CREATURE_CONDITIONALGORITHMS_HPP_INCLUDED
