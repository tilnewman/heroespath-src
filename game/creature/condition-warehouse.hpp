#ifndef GAME_CREATURE_CONDITIONWAREHOUSE_HPP_INCLUDED
#define GAME_CREATURE_CONDITIONWAREHOUSE_HPP_INCLUDED
//
// condition-warehouse.hpp
//
#include "game/creature/condition-enum.hpp"

#include <memory>
#include <vector>


namespace game
{
namespace creature
{
    class Condition;
    using ConditionPtr_t = Condition *;
    using ConditionSPtr_t = std::shared_ptr<Condition>;
    using ConditionSVec_t = std::vector<ConditionSPtr_t>;

namespace condition
{

    struct Warehouse
    {
        static void Setup();
        static bool Test();
        static ConditionPtr_t Get(const Conditions::Enum);
        
    private:
        static ConditionSVec_t conditionsSVec_;
    };

}
}
}


#endif //GAME_CREATURE_CONDITIONWAREHOUSE_HPP_INCLUDED
