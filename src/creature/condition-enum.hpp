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
#include "misc/enum-common.hpp"

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
    struct Conditions : public EnumBaseCounting<EnumFirstValue::Valid>
    {
        enum Enum : EnumUnderlying_t
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

        static constexpr std::string_view Name(const Enum ENUM)
        {
            if (ENUM == AsleepNatural)
            {
                return "Asleep";
            }
            else if (ENUM == AsleepMagical)
            {
                return "Magical Sleep";
            }
            else
            {
                return NAMEOF_ENUM(ENUM);
            }
        }

        static const std::string Desc(const Enum);
        static const std::string ImageFilename(const Enum);
        static const std::string ImageDirectory();
        static const std::string ImagePath(const Enum);
    };

    using CondEnumVec_t = std::vector<Conditions::Enum>;

    namespace condition
    {

        struct Severity
        {
            // Note:  Keep the GOOD condition at a different Severity from
            //       LEAST_BENEFICIAL, so that GOOD can be avoided when
            //       displaying conditions on the battlefield.
            //
            //       Keep ALL=0 so that min severity works.
            //
            //       Keep LEAST_... < MOST_... so descending sort works.
            //
            static constexpr std::size_t ALL { 0 };
            static constexpr std::size_t GOOD { 1 };
            static constexpr std::size_t LEAST_BENEFITIAL { 2 };
            static constexpr std::size_t MOST_BENEFITIAL { 1000 };
            static constexpr std::size_t LEAST_HARMFUL { 1001 };
            static constexpr std::size_t MOST_HARMFUL { 2000 };

            // Note:  Larger return values are more harmful.
            //       Each Condition must have a unique Severity.
            static constexpr std::size_t Get(const Conditions::Enum ENUM) noexcept
            {
                switch (ENUM)
                {
                    case Conditions::Good: return GOOD;
                    case Conditions::Bold: return 100;
                    case Conditions::Heroic: return 400;
                    case Conditions::Daunted: return LEAST_HARMFUL; // 1001
                    case Conditions::Panic: return 1050;
                    case Conditions::Dazed: return 1100;
                    case Conditions::Tripped: return 1200;
                    case Conditions::AsleepNatural: return 1300;
                    case Conditions::Poisoned: return 1400;
                    case Conditions::Pounced: return 1450;
                    case Conditions::AsleepMagical: return 1500;
                    case Conditions::Unconscious: return 1800;
                    case Conditions::Stone: return 1900;
                    case Conditions::Dead: return MOST_HARMFUL; // 2000
                    case Conditions::Count:
                    default: return 0;
                }
            }
        };

    } // namespace condition
} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CONDITIONENUM_HPP_INCLUDED
