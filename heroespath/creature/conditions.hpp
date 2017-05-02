#ifndef APPBASE_HEROESPATH_CREATURE_CONDITIONS_INCLUDED
#define APPBASE_HEROESPATH_CREATURE_CONDITIONS_INCLUDED
//
// conditions.hpp
//  Instances and base classes of specific Conditions.
//
#include "utilz/boost-serialize-includes.hpp"

#include "heroespath/creature/condition.hpp"
#include "heroespath/creature/creature.hpp"
#include "heroespath/stats/stat-set.hpp"

#include <string>


namespace heroespath
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
        Cond_Frightened() : Condition(condition::Frightened), inverseModifyStatSet_() {}
        virtual ~Cond_Frightened() {}
        virtual void Change(CreaturePtrC_t creaturePtrC) { inverseModifyStatSet_ = creaturePtrC->DivideStatsAndCreateInverseModifyStatSet(1.0f, 2.0f, 1.0f, 1.0f, 2.0f); }
        virtual void Undo(CreaturePtrC_t creaturePtrC)   { creaturePtrC->Stats().ModifyCurrent(inverseModifyStatSet_); }
    private:
        stats::StatSet inverseModifyStatSet_;
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int) { ar & boost::serialization::base_object<Condition>(*this); ar & inverseModifyStatSet_; }
    };


    class Cond_Tripped : public Condition
    {
    public:
        Cond_Tripped() : Condition(condition::Tripped), inverseModifyStatSet_() {}
        virtual ~Cond_Tripped() {}
        virtual void Change(CreaturePtrC_t creaturePtrC) { inverseModifyStatSet_ = creaturePtrC->DivideStatsAndCreateInverseModifyStatSet(4.0f, 4.0f, 1.0f, 1.0f, 4.0f); }
        virtual void Undo(CreaturePtrC_t creaturePtrC)   { creaturePtrC->Stats().ModifyCurrent(inverseModifyStatSet_); }
    private:
        stats::StatSet inverseModifyStatSet_;
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int) { ar & boost::serialization::base_object<Condition>(*this); ar & inverseModifyStatSet_; }
    };


    class Cond_Dazed : public Condition
    {
    public:
        Cond_Dazed() : Condition(condition::Dazed), inverseModifyStatSet_() {}
        virtual ~Cond_Dazed() {}
        virtual void Change(CreaturePtrC_t creaturePtrC) { inverseModifyStatSet_ = creaturePtrC->DivideStatsAndCreateInverseModifyStatSet(2.0f, 3.0f, 1.0f, 1.0f, 3.0f, 2.0f); }
        virtual void Undo(CreaturePtrC_t creaturePtrC) { creaturePtrC->Stats().ModifyCurrent(inverseModifyStatSet_); }
    private:
        stats::StatSet inverseModifyStatSet_;
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int) { ar & boost::serialization::base_object<Condition>(*this); }
    };


    class Cond_Unconscious : public Condition
    {
    public:
        Cond_Unconscious() : Condition(condition::Unconscious) {}
        virtual ~Cond_Unconscious() {}
    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int) { ar & boost::serialization::base_object<Condition>(*this); }
    };


    class Cond_Stone : public Condition
    {
    public:
        Cond_Stone() : Condition(condition::Stone, true, stats::StatSet(), stats::StatSet(stats::INVALID_STAT_VAL, 0, 0, stats::INVALID_STAT_VAL, 0, 0)) {}
        virtual ~Cond_Stone() {}
    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int) { ar & boost::serialization::base_object<Condition>(*this); }
    };


    class Cond_Dead : public Condition
    {
    public:
        Cond_Dead() : Condition(condition::Dead, false, stats::StatSet(), stats::StatSet(0, 0, 0, 0, 0, 0)) {}
        virtual ~Cond_Dead() {}
    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int) { ar & boost::serialization::base_object<Condition>(*this); }
    };


    struct ConditionFactory
    {
        static const ConditionSPtr_t GOOD_SPTR;
        static ConditionSPtr_t Make(const condition::Enum E);
        static bool Test();
    };

}
}
}
#endif //APPBASE_HEROESPATH_CREATURE_CONDITIONS_INCLUDED
