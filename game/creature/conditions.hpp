#ifndef GAME_CREATURE_CONDITIONS_INCLUDED
#define GAME_CREATURE_CONDITIONS_INCLUDED
//
// conditions.hpp
//  Instances and base classes of specific Conditions.
//
#include "utilz/boost-serialize-includes.hpp"

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

    class Cond_Good : public Condition
    {
    public:
        Cond_Good() : Condition() {}
        virtual ~Cond_Good() {}
    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int) { ar & boost::serialization::base_object<Condition>(*this); }
    };


    class Cond_Frightened : public Condition
    {
    public:
        Cond_Frightened() : Condition(Conditions::Frightened), inverseModifyStatSet_() {}
        virtual ~Cond_Frightened() {}
        virtual const ConditionEnumVec_t Change(CreaturePtrC_t creaturePtrC) { inverseModifyStatSet_ = creaturePtrC->DivideStatsAndCreateInverseModifyStatSet(1.0f, 2.0f, 1.0f, 1.0f, 2.0f); return ConditionEnumVec_t(); }
        virtual const ConditionEnumVec_t Undo(CreaturePtrC_t creaturePtrC)   { creaturePtrC->Stats().ModifyCurrent(inverseModifyStatSet_);  return ConditionEnumVec_t(); }
    private:
        stats::StatSet inverseModifyStatSet_;
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int) { ar & boost::serialization::base_object<Condition>(*this); ar & inverseModifyStatSet_; }
    };


    class Cond_Tripped : public Condition
    {
    public:
        Cond_Tripped() : Condition(Conditions::Tripped), inverseModifyStatSet_() {}
        virtual ~Cond_Tripped() {}
        virtual const ConditionEnumVec_t Change(CreaturePtrC_t creaturePtrC) { inverseModifyStatSet_ = creaturePtrC->DivideStatsAndCreateInverseModifyStatSet(4.0f, 4.0f, 1.0f, 1.0f, 4.0f); return ConditionEnumVec_t(); }
        virtual const ConditionEnumVec_t Undo(CreaturePtrC_t creaturePtrC)   { creaturePtrC->Stats().ModifyCurrent(inverseModifyStatSet_); return ConditionEnumVec_t(); }
    private:
        stats::StatSet inverseModifyStatSet_;
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int) { ar & boost::serialization::base_object<Condition>(*this); ar & inverseModifyStatSet_; }
    };


    class Cond_Dazed : public Condition
    {
    public:
        Cond_Dazed() : Condition(Conditions::Dazed), inverseModifyStatSet_() {}
        virtual ~Cond_Dazed() {}
        virtual const ConditionEnumVec_t Change(CreaturePtrC_t creaturePtrC) { inverseModifyStatSet_ = creaturePtrC->DivideStatsAndCreateInverseModifyStatSet(2.0f, 3.0f, 1.0f, 1.0f, 3.0f, 2.0f); return ConditionEnumVec_t(); }
        virtual const ConditionEnumVec_t Undo(CreaturePtrC_t creaturePtrC) { creaturePtrC->Stats().ModifyCurrent(inverseModifyStatSet_); return ConditionEnumVec_t(); }
    private:
        stats::StatSet inverseModifyStatSet_;
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int) { ar & boost::serialization::base_object<Condition>(*this); }
    };


    class Cond_Unconscious : public Condition
    {
    public:
        Cond_Unconscious() : Condition(Conditions::Unconscious) {}
        virtual ~Cond_Unconscious() {}
    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int) { ar & boost::serialization::base_object<Condition>(*this); }
    };


    class Cond_Stone : public Condition
    {
    public:
        Cond_Stone() : Condition(Conditions::Stone, true, stats::StatSet(), stats::StatSet(stats::Stat::VAL_INVALID_, 0, 0, stats::Stat::VAL_INVALID_, 0, 0)) {}
        virtual ~Cond_Stone() {}
    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int) { ar & boost::serialization::base_object<Condition>(*this); }
    };


    class Cond_Dead : public Condition
    {
    public:
        Cond_Dead() : Condition(Conditions::Dead, false, stats::StatSet(), stats::StatSet(0, 0, 0, 0, 0, 0)) {}
        virtual ~Cond_Dead() {}
    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int) { ar & boost::serialization::base_object<Condition>(*this); }
    };


    class Cond_AsleepNatural : public Condition
    {
    public:
        Cond_AsleepNatural() : Condition(Conditions::AsleepNatural, false, stats::StatSet(), stats::StatSet(0, 0, stats::Stat::VAL_INVALID_, stats::Stat::VAL_INVALID_, 0, stats::Stat::VAL_INVALID_)) {}
        virtual ~Cond_AsleepNatural() {}
    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int) { ar & boost::serialization::base_object<Condition>(*this); }
    };


    class Cond_AsleepMagical : public Condition
    {
    public:
        Cond_AsleepMagical() : Condition(Conditions::AsleepMagical, false, stats::StatSet(), stats::StatSet(0, 0, stats::Stat::VAL_INVALID_, stats::Stat::VAL_INVALID_, 0, stats::Stat::VAL_INVALID_)) {}
        virtual ~Cond_AsleepMagical() {}
    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int) { ar & boost::serialization::base_object<Condition>(*this); }
    };


    class Cond_Poisoned : public Condition
    {
    public:
        Cond_Poisoned() : Condition(Conditions::Poisoned, false, stats::StatSet(), stats::StatSet()) {}
        virtual const ConditionEnumVec_t PerTurnChange(CreaturePtrC_t creaturePtrC);
        virtual ~Cond_Poisoned() {}
    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int) { ar & boost::serialization::base_object<Condition>(*this); }
    };


    struct ConditionFactory
    {
        static const ConditionSPtr_t GOOD_SPTR;
        static ConditionSPtr_t Make(const Conditions::Enum E);
        static bool Test();
    };

}
}
}
#endif //GAME_CREATURE_CONDITIONS_INCLUDED
