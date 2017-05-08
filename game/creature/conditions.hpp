#ifndef GAME_CREATURE_CONDITIONS_INCLUDED
#define GAME_CREATURE_CONDITIONS_INCLUDED
//
// conditions.hpp
//  Instances and base classes of specific Conditions.
//
#include "game/creature/condition.hpp"
#include "game/creature/creature.hpp"
#include "game/stats/stat-set.hpp"

#include <string>


namespace game
{
namespace creature
{
namespace condition
{

    class Good : public Condition
    {
    public:
        Good() : Condition() {}
        virtual ~Good() {}
    };


    class Frightened : public Condition
    {
    public:
        Frightened() : Condition(Conditions::Frightened, false, stats::StatSet(), stats::StatSet::INVALID_STATSET_, 1.0f, 2.0f, 1.0f, 1.0f, 2.0f) {}
        virtual ~Frightened() {}
    };


    class Tripped : public Condition
    {
    public:
        Tripped() : Condition(Conditions::Tripped, false, stats::StatSet(), stats::StatSet::INVALID_STATSET_, 4.0f, 4.0f, 1.0f, 1.0f, 4.0f) {}
        virtual ~Tripped() {}
    };


    class Dazed : public Condition
    {
    public:
        Dazed() : Condition(Conditions::Dazed, false, stats::StatSet(), stats::StatSet::INVALID_STATSET_, 2.0f, 3.0f, 1.0f, 1.0f, 3.0f, 2.0f) {}
        virtual ~Dazed() {}
    };


    class Unconscious : public Condition
    {
    public:
        Unconscious() : Condition(Conditions::Unconscious) {}
        virtual ~Unconscious() {}
    };


    class Stone : public Condition
    {
    public:
        Stone() : Condition(Conditions::Stone, true, stats::StatSet(), stats::StatSet(stats::Stat::VAL_INVALID_, 0, 0, stats::Stat::VAL_INVALID_, 0, 0)) {}
        virtual ~Stone() {}
    };


    class Dead : public Condition
    {
    public:
        Dead() : Condition(Conditions::Dead, false, stats::StatSet(), stats::StatSet(0, 0, 0, 0, 0, 0)) {}
        virtual ~Dead() {}
    };


    class AsleepNatural : public Condition
    {
    public:
        AsleepNatural() : Condition(Conditions::AsleepNatural, false, stats::StatSet(), stats::StatSet(0, 0, stats::Stat::VAL_INVALID_, stats::Stat::VAL_INVALID_, 0, stats::Stat::VAL_INVALID_)) {}
        virtual ~AsleepNatural() {}
    };


    class AsleepMagical : public Condition
    {
    public:
        AsleepMagical() : Condition(Conditions::AsleepMagical, false, stats::StatSet(), stats::StatSet(0, 0, stats::Stat::VAL_INVALID_, stats::Stat::VAL_INVALID_, 0, stats::Stat::VAL_INVALID_)) {}
        virtual ~AsleepMagical() {}
    };


    class Poisoned : public Condition
    {
    public:
        Poisoned() : Condition(Conditions::Poisoned, false, stats::StatSet(), stats::StatSet()) {}
        virtual ~Poisoned() {}
        virtual const ConditionEnumVec_t PerTurnChange(CreaturePtrC_t creaturePtrC);
    };

}
}
}
#endif //GAME_CREATURE_CONDITIONS_INCLUDED
