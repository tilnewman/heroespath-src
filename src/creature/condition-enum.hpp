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
#ifndef HEROESPATH_CONDITIONENUM_HPP_INCLUDED
#define HEROESPATH_CONDITIONENUM_HPP_INCLUDED
//
// condition-enum.hpp
//  An enumeration defining each type of Condition.
//
#include <string>
#include <vector>

namespace heroespath
{
namespace creature
{

    // Note:  Must be in order of increasing severity.
    //
    //  Keep in sync with:
    //      creature::HasConditionNotAThreatPerm() and HasConditionNotAThreatTemp()
    //      creature::CanTakeActionStr()
    //      CombatStage::RemoveSingleTurnTemporaryConditions()
    //      descriptions in the game data file
    //      interactions in fight.cpp
    //      testing code in conditions.cpp creature::condition::Warehouse::Fill()
    //
    struct Conditions
    {
        enum Enum
        {
            Good = 0,
            Bold,
            Heroic,
            Daunted,
            Panic,
            Dazed,
            Tripped,
            AsleepNatural,
            Poisoned,
            Pounced,
            AsleepMagical,
            Unconscious,
            Stone,
            Dead,
            Count
        };

        static const std::string Name(const Enum);
        static const std::string ToString(const Enum);
        static const std::string Desc(const Enum);
        static const std::string ImageFilename(const Enum);
    };

    using CondEnumVec_t = std::vector<Conditions::Enum>;

    namespace condition
    {

        struct Severity
        {
            // Note:  Keep the GOOD condition at a different Severity from
            //       LEAST_BENEFITIAL, so that GOOD can be avoided when
            //       displaying conditions on the battlefield.
            //
            //       Keep ALL equal to zero so that min severity works.
            //
            //       Keep LEAST_... < MOST_... so descending sort works.
            //
            static const std::size_t ALL;
            static const std::size_t GOOD;
            static const std::size_t LEAST_BENEFITIAL;
            static const std::size_t MOST_BENEFITIAL;
            static const std::size_t LEAST_HARMFUL;
            static const std::size_t MOST_HARMFUL;

            // Note:  Larger return values are more harmful.
            //       Each Condition must have a unique Severity.
            static std::size_t Get(const Conditions::Enum);
        };
    } // namespace condition
} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CONDITIONENUM_HPP_INCLUDED
