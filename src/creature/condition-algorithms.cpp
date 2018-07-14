// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// condition-algorithms.cpp
//
#include "condition-algorithms.hpp"

#include "creature/condition.hpp"
#include "log/log-macros.hpp"

#include <algorithm>
#include <sstream>

namespace heroespath
{
namespace creature
{
    namespace condition
    {

        const std::string Algorithms::Names(
            const CondEnumVec_t & CONDITIONS_VEC,
            const std::size_t MAX_COUNT,
            const std::size_t MIN_SEVERITY,
            const SortOpt SORT_OPTION,
            const misc::Vector::JoinOpt JOIN_OPTIONS)
        {
            auto tempVec { CONDITIONS_VEC };
            RemoveByMinSeverity(tempVec, MIN_SEVERITY);
            SortBySeverity(tempVec, SORT_OPTION);

            return misc::Vector::Join<Conditions::Enum>(
                tempVec,
                MAX_COUNT,
                JOIN_OPTIONS,
                [](const Conditions::Enum & E) -> const std::string {
                    return Conditions::Name(E);
                });
        }

        void Algorithms::SortBySeverity(CondEnumVec_t & conditionsVec, const SortOpt SORT_OPTION)
        {
            if (conditionsVec.size() > 1)
            {
                std::sort(
                    conditionsVec.begin(),
                    conditionsVec.end(),
                    [SORT_OPTION](const Conditions::Enum A, const Conditions::Enum B) {
                        if (SORT_OPTION == SortOpt::Ascending)
                        {
                            return (condition::Severity::Get(A) < condition::Severity::Get(B));
                        }
                        else
                        {
                            return (condition::Severity::Get(A) > condition::Severity::Get(B));
                        }
                    });
            }
        }

        const CondEnumVec_t Algorithms::SortBySeverityCopy(
            const CondEnumVec_t & CONDITIONS_VEC, const SortOpt SORT_OPTION)
        {
            auto sortedConditionsVec { CONDITIONS_VEC };
            SortBySeverity(sortedConditionsVec, SORT_OPTION);
            return sortedConditionsVec;
        }

        void Algorithms::RemoveByMinSeverity(
            CondEnumVec_t & conditionsVec, const std::size_t MIN_SEVERITY)
        {
            if (conditionsVec.empty() == false)
            {
                conditionsVec.erase(
                    std::remove_if(
                        conditionsVec.begin(),
                        conditionsVec.end(),
                        [=](const Conditions::Enum E) {
                            return (
                                (MIN_SEVERITY != 0)
                                && (condition::Severity::Get(E) <= MIN_SEVERITY));
                        }),
                    conditionsVec.end());
            }
        }

        const CondEnumVec_t Algorithms::RemoveByMinSeverityCopy(
            const CondEnumVec_t & CONDITIONS_VEC, const std::size_t MIN_SEVERITY)
        {
            auto tempVec { CONDITIONS_VEC };
            RemoveByMinSeverity(tempVec, MIN_SEVERITY);
            return tempVec;
        }
    } // namespace condition
} // namespace creature
} // namespace heroespath
