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
namespace condition
{

    //needs to be in order of increasing severity
    //
    //NOTE:  Keep in sync with:
    //  game::creature::Algorithms::DiscardWithConditionsMeaningNotAThreat()
    //  game::creature::CanTakeActionStr()
    //  party-serialize-includes.hpp
    //
    enum Enum
    {
        Good = 0,
        Frightened,
        Dazed,
        Tripped,
        //Diseased,
        //Poisoned,
        Unconscious,
        Stone,
        Dead,
        Count
    };

    using ConditionEnumVec_t = std::vector<Enum>;


    const std::string Name(const condition::Enum E);
    const std::string ToString(const condition::Enum E);
    const std::string Desc(const condition::Enum E);


    struct Severity
    {
        static const std::size_t ALL;
        static const std::size_t BENEFITIAL;
        static const std::size_t LEAST_HARMFUL;
        static const std::size_t MOST_HARMFUL;

        //larger return values are worse
        static std::size_t ByCondition(const condition::Enum E);
    };

}
}
}
#endif //GAME_CONDITIONENUM_INCLUDED
