// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_CONDITIONENUM_HPP_INCLUDED
#define HEROESPATH_CONDITIONENUM_HPP_INCLUDED
//
// condition-enum.hpp
//  An enumeration defining each type of Condition.
//
#include "misc/enum-util.hpp"

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
    struct Conditions : public misc::EnumBaseCounting<Conditions, misc::EnumFirstValueValid>
    {
        enum Enum : misc::EnumUnderlying_t
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
