// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
//
// condition-algorithms.cpp
//
#include "condition-algorithms.hpp"

#include "log/log-macros.hpp"
#include "creature/condition.hpp"
#include "creature/condition-warehouse.hpp"

#include <sstream>
#include <algorithm>


namespace heroespath
{
namespace creature
{
namespace condition
{

    const std::string Algorithms::Names(
        const CondEnumVec_t &       CONDITIONS_VEC,
        const std::size_t           MAX_COUNT,
        const std::size_t           MIN_SEVERITY,
        const SortOpt               SORT_OPTION,
        const misc::Vector::JoinOpt JOIN_OPTIONS)
    {
        auto tempVec{ CONDITIONS_VEC };
        RemoveByMinSeverity(tempVec, MIN_SEVERITY);
        SortBySeverity(tempVec, SORT_OPTION);

        return misc::Vector::Join<Conditions::Enum>(
            tempVec,
            MAX_COUNT,
            JOIN_OPTIONS,
            [](const Conditions::Enum E) -> const std::string
            {
                return Conditions::Name(E);
            });
    }


    void Algorithms::SortBySeverity(
        CondEnumVec_t & conditionsVec,
        const SortOpt   SORT_OPTION)
    {
        if (conditionsVec.size() > 1)
        {
            std::sort(
                conditionsVec.begin(),
                conditionsVec.end(),
                [SORT_OPTION](const Conditions::Enum A, const Conditions::Enum B)
                {
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
        const CondEnumVec_t & CONDITIONS_VEC,
        const SortOpt         SORT_OPTION)
    {
        auto sortedConditionsVec{ CONDITIONS_VEC };
        SortBySeverity(sortedConditionsVec, SORT_OPTION);
        return sortedConditionsVec;
    }


    void Algorithms::RemoveByMinSeverity(
        CondEnumVec_t &   conditionsVec,
        const std::size_t MIN_SEVERITY)
    {
        if (conditionsVec.empty() == false)
        {
            conditionsVec.erase(std::remove_if(
                conditionsVec.begin(),
                conditionsVec.end(),
                [=](const Conditions::Enum E)
                {
                    return ((MIN_SEVERITY != 0) && (condition::Severity::Get(E) <= MIN_SEVERITY));
                }),
                conditionsVec.end());
        }
    }


    const CondEnumVec_t Algorithms::RemoveByMinSeverityCopy(
        const CondEnumVec_t & CONDITIONS_VEC,
        const std::size_t     MIN_SEVERITY)
    {
        auto tempVec{ CONDITIONS_VEC };
        RemoveByMinSeverity(tempVec, MIN_SEVERITY);
        return tempVec;
    }

}
}
}
