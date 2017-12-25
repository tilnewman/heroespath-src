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
#ifndef HEROSPATH_CREATURE_CONDITIONALGORITHMS_HPP_INCLUDED
#define HEROSPATH_CREATURE_CONDITIONALGORITHMS_HPP_INCLUDED
//
// condition-algorithms.hpp
//
#include "creature/condition-enum.hpp"
#include "misc/vectors.hpp"

#include <string>
#include <memory>
#include <vector>


namespace heroespath
{
namespace creature
{
namespace condition
{

    //A collection of functions that operate on vectors of Conditions.
    struct Algorithms
    {
        enum class SortOpt
        {
            Ascending,
            Descending
        };

        static const std::string Names(
            const CondEnumVec_t &       CONDITIONS_VEC,
            const std::size_t           MAX_TO_LIST    = 0,
            const std::size_t           MIN_SEVERITY   = 0,
            const SortOpt               SORT_OPTION    = SortOpt::Ascending,
            const misc::Vector::JoinOpt JOIN_OPTIONS   = misc::Vector::JoinOpt::None);

        //Note:  Each Condition has a unique Severity so these sorts are complete.
        static void SortBySeverity(
            CondEnumVec_t & conditionsVec,
            const SortOpt   SORT_OPTION = SortOpt::Descending);

        static const CondEnumVec_t SortBySeverityCopy(
            const CondEnumVec_t & CONDITIONS_VEC,
            const SortOpt         SORT_OPTION = SortOpt::Descending);

        static void RemoveByMinSeverity(
            CondEnumVec_t &   conditionsVec,
            const std::size_t MIN_SEVERITY);

        static const CondEnumVec_t RemoveByMinSeverityCopy(
            const CondEnumVec_t & CONDITIONS_VEC,
            const std::size_t     MIN_SEVERITY);
    };

}
}
}

#endif //HEROSPATH_CREATURE_CONDITIONALGORITHMS_HPP_INCLUDED