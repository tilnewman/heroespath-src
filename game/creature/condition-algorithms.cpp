//
// condition-algorithms.cpp
//
#include "condition-algorithms.hpp"

#include "game/log-macros.hpp"
#include "game/creature/condition.hpp"
#include "game/creature/condition-warehouse.hpp"

#include <sstream>
#include <algorithm>


namespace game
{
namespace creature
{
namespace condition
{

    const std::string Algorithms::Names(const ConditionEnumVec_t & CONDITIONS_VEC,
                                        const std::string          SEPARATOR,
                                        const bool                 WILL_WRAP,
                                        const std::size_t          MAX_TO_LIST,
                                        const std::size_t          MIN_SEVERITY,
                                        const bool                 WILL_AND,
                                        const bool                 WILL_ELLIPSIS)
    {
        auto const CONDITION_COUNT{ CONDITIONS_VEC.size() };
        if (CONDITION_COUNT == 0)
        {
            return "";
        }
        else
        {
            std::size_t count(0);
            std::ostringstream ss;

            for (std::size_t i(0); i<CONDITION_COUNT; ++i)
            {
                auto const CONDITION_PTR{ condition::Warehouse::Get(CONDITIONS_VEC[i]) };
                if ((MIN_SEVERITY == 0) || (CONDITION_PTR->Severity() >= MIN_SEVERITY))
                {
                    if (i != 0)
                    {
                        ss << SEPARATOR;
                    }

                    if ((WILL_AND) && (CONDITION_COUNT >= 2) && (i == (CONDITION_COUNT - 1)))
                    {
                        ss << "and ";
                    }

                    ss << CONDITION_PTR->Name();

                    if ((MAX_TO_LIST != 0) && (++count >= MAX_TO_LIST))
                    {
                        if (WILL_ELLIPSIS && (i < (CONDITION_COUNT - 1)))
                        {
                            ss << "...";
                        }

                        break;
                    }
                }
            }

            if (ss.str().empty())
            {
                return "";
            }
            else
            {
                if (WILL_WRAP)
                {
                    return "(" + ss.str() + ")";
                }
                else
                {
                    return ss.str();
                }
            }
        }
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

}
}
}
