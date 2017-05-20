//
// condition-algorithms.cpp
//
#include "condition-algorithms.hpp"

#include "game/log-macros.hpp"
#include "game/creature/condition.hpp"
#include "game/creature/condition-warehouse.hpp"

#include "utilz/vectors.hpp"

#include <sstream>
#include <algorithm>


namespace game
{
namespace creature
{
namespace condition
{

    const std::string Algorithms::Names(const ConditionEnumVec_t & CONDITIONS_VEC,
                                        const bool                 WILL_WRAP,
                                        const bool                 WILL_AND,
                                        const std::size_t          MAX_COUNT,
                                        const bool                 WILL_ELLIPSIS,
                                        const std::size_t          MIN_SEVERITY,
                                        const bool                 WILL_SORT_DESCENDING)

    {
        auto tempVec{ CONDITIONS_VEC };
        RemoveByMinSeverity(tempVec, MIN_SEVERITY);

        if (WILL_SORT_DESCENDING)
        {
            SortBySeverity(tempVec, true);
        }

        return utilz::Vector::Join<Conditions::Enum>(tempVec,
                                                     WILL_WRAP,
                                                     WILL_AND,
                                                     MAX_COUNT,
                                                     WILL_ELLIPSIS,
                                                     [](const Conditions::Enum E) -> const std::string { return Conditions::Name(E); });
    }


    void Algorithms::SortBySeverity(ConditionEnumVec_t & conditionsVec,
                                    const bool           SORT_DESCENDING)
    {
        if (conditionsVec.size() > 1)
        {
            std::sort(conditionsVec.begin(),
                      conditionsVec.end(),
                      [SORT_DESCENDING]
                      (const Conditions::Enum A, const Conditions::Enum B)
                      { if (SORT_DESCENDING) return (condition::Severity::Get(A) > condition::Severity::Get(B)); else return (condition::Severity::Get(A) < condition::Severity::Get(B)); });
        }
    }


    const ConditionEnumVec_t Algorithms::SortBySeverityCopy(const ConditionEnumVec_t & CONDITIONS_VEC,
                                                            const bool                 SORT_DESCENDING)
    {
        auto sortedConditionsVec{ CONDITIONS_VEC };
        SortBySeverity(sortedConditionsVec, SORT_DESCENDING);
        return sortedConditionsVec;
    }


    void Algorithms::RemoveByMinSeverity(ConditionEnumVec_t & conditionsVec,
                                         const std::size_t    MIN_SEVERITY)
    {
        if (conditionsVec.empty() == false)
        {
            conditionsVec.erase(std::remove_if(conditionsVec.begin(),
                                               conditionsVec.end(),
                                               [=](const Conditions::Enum E)
                                                {
                                                    return ((MIN_SEVERITY != 0) && (condition::Severity::Get(E) <= MIN_SEVERITY));
                                                }),
                                conditionsVec.end());
        }
    }


    const ConditionEnumVec_t Algorithms::RemoveByMinSeverityCopy(const ConditionEnumVec_t & CONDITIONS_VEC,
                                                                 const std::size_t          MIN_SEVERITY)
    {
        auto tempVec{ CONDITIONS_VEC };
        RemoveByMinSeverity(tempVec, MIN_SEVERITY);
        return tempVec;
    }

}
}
}
