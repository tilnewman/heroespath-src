#ifndef GAME_CONDITIONENUM_INCLUDED
#define GAME_CONDITIONENUM_INCLUDED
//
// condition-enum.hpp
//  An enumeration defining each type of Condition.
//
#include <string>
#include <vector>


namespace game
{
namespace creature
{

    //Note:  Must be in order of increasing severity.
    //
    //Note:  Keep in sync with:
    //  conditions in conditions.hpp
    //  creature::HasConditionNotAThreatPerm() and HasConditionNotAThreatTemp()
    //  creature::CanTakeActionStr()
    //  CombatStage::RemoveSingleTurnTemporaryConditions()
    //  descriptions in the game data file
    //  interactions in fight.cpp
    //  testing code in conditions.cpp ConditionFactory::Test()
    //
    struct Conditions
    {
        enum Enum
        {
            Good = 0,
            Frightened,
            Dazed,
            Tripped,
            AsleepNatural,
            Poisoned,
            AsleepMagical,
            Unconscious,
            Stone,
            Dead,
            Count
        };

        static const std::string Name(const Enum);
        static const std::string ToString(const Enum);
        static const std::string Desc(const Enum);

    };

    using ConditionEnumVec_t      = std::vector<Conditions::Enum>;
    using ConditionEnumVecIter_t  = ConditionEnumVec_t::iterator;
    using ConditionEnumVecCIter_t = ConditionEnumVec_t::const_iterator;

namespace condition
{

    struct Severity
    {
        //Note:  Keep the GOOD condition at a different Severity from
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


        //Note:  Larger return values are more harmful.
        //       Each Condition must have a unique Severity.
        static std::size_t Get(const Conditions::Enum);
    };

}
}
}

#endif //GAME_CONDITIONENUM_INCLUDED
