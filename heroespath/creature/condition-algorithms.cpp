//
// condition-algorithms.cpp
//
#include "condition-algorithms.hpp"

#include "heroespath/log-macros.hpp"
#include "heroespath/creature/condition.hpp"

#include <sstream>
#include <algorithm>


namespace heroespath
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
                                        const bool              WILL_AND)
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


    void Algorithms::Append(const ConditionSVec_t & CONDITIONS_TO_ADD_SVEC,
                            ConditionSVec_t &       conditionsToAddToSVec,
                            const bool              WILL_UNIQUE)
    {
        if (CONDITIONS_TO_ADD_SVEC.empty() == false)
        {
            std::copy(CONDITIONS_TO_ADD_SVEC.begin(),
                      CONDITIONS_TO_ADD_SVEC.end(),
                      back_inserter(conditionsToAddToSVec));

            if (WILL_UNIQUE)
            {
                std::unique(conditionsToAddToSVec.begin(), conditionsToAddToSVec.end());
            }
        }
    }


    const ConditionSVec_t Algorithms::AppendCopy(const ConditionSVec_t & CONDITIONS_A_SVEC,
                                                 const ConditionSVec_t & CONDITIONS_B_SVEC,
                                                 const bool              WILL_UNIQUE)
    {
        auto conditionsSVec{ CONDITIONS_B_SVEC };
        Append(CONDITIONS_A_SVEC, conditionsSVec, WILL_UNIQUE);
        return conditionsSVec;
    }


    const ConditionSVec_t Algorithms::Exclude(const ConditionSVec_t & CONDITIONS_SVEC,
                                              const condition::Enum   ENUM)
    {
        const ConditionEnumVec_t ENUM_VEC{ ENUM };
        return Exclude(CONDITIONS_SVEC, ENUM_VEC);
    }
    
    
    const ConditionSVec_t Algorithms::Exclude(const ConditionSVec_t &    CONDITIONS_SVEC,
                                              const ConditionEnumVec_t & ENUM_VEC)
    {
        ConditionSVec_t excludedConditionsSVec;
    
        if (CONDITIONS_SVEC.empty() == false)
        {
            std::copy_if(CONDITIONS_SVEC.begin(),
                         CONDITIONS_SVEC.end(),
                         back_inserter(excludedConditionsSVec),
                         [&ENUM_VEC] (const ConditionSPtr_t & CSPTR)
                         {
                             for(auto const NEXT_ENUM : ENUM_VEC)
                                 if (CSPTR->Which() == NEXT_ENUM)
                                     return false;
    
                             return true;
                         });
        }
    
        return excludedConditionsSVec;
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
