//
// condition-algorithms.cpp
//
#include "condition-algorithms.hpp"

#include "game/log-macros.hpp"
#include "game/creature/condition.hpp"

#include <sstream>
#include <algorithm>


namespace game
{
namespace creature
{
namespace condition
{

    const std::string Algorithms::Names(const ConditionSVec_t & CONDITIONS_SVEC,
                                        const std::string       SEPARATOR,
                                        const bool              WILL_WRAP,
                                        const std::size_t       MAX_TO_LIST,
                                        const std::size_t       MIN_SEVERITY,
                                        const bool              WILL_AND,
                                        const bool              WILL_ELLIPSIS)
    {
        auto const CONDITION_COUNT{ CONDITIONS_SVEC.size() };
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
                if ((MIN_SEVERITY == 0) || (CONDITIONS_SVEC[i]->Severity() >= MIN_SEVERITY))
                {
                    if (i != 0)
                    {
                        ss << SEPARATOR;
                    }

                    if ((WILL_AND) && (CONDITION_COUNT >= 2) && (i == (CONDITION_COUNT - 1)))
                    {
                        ss << "and ";
                    }

                    ss << CONDITIONS_SVEC[i]->Name();

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


    void Algorithms::SortBySeverity(ConditionSVec_t & conditionsSVec,
                                    const bool        SORT_DESCENDING)
    {
        if (conditionsSVec.size() > 1)
        {
            std::sort(conditionsSVec.begin(),
                      conditionsSVec.end(),
                      [SORT_DESCENDING]
                      (const ConditionSPtr_t & A, const ConditionSPtr_t & B)
                      { if (SORT_DESCENDING) return (A->Severity() > B->Severity()); else return (A->Severity() < B->Severity()); });
        }
    }


    const ConditionSVec_t Algorithms::SortBySeverityCopy(const ConditionSVec_t & CONDITIONS_SVEC,
                                                         const bool              SORT_DESCENDING)
    {
        auto sortedConditionsSVec{ CONDITIONS_SVEC };
        SortBySeverity(sortedConditionsSVec, SORT_DESCENDING);
        return sortedConditionsSVec;
    }

}
}
}
