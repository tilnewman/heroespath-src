#ifndef APPBASE_HEROESPATH_CREATURE_CONDITION_INCLUDED
#define APPBASE_HEROESPATH_CREATURE_CONDITION_INCLUDED
//
// condition.hpp
//  Code that places a Creature under a temporary condition.
//  Examples would be poisoned, unconcious, etc.
//
#include "sfml-util/boost-serialize-includes.hpp"

#include "heroespath/creature/condition-enum.hpp"
#include "heroespath/stats/stat-set.hpp"

#include <memory>
#include <string>
#include <vector>


namespace heroespath
{
namespace creature
{

    //forward declarations
    class Creature;
    using CreaturePtr_t  = Creature *;
    using CreaturePtrC_t = Creature * const;


    //base class for all conditions
    class Condition
    {
        //prevent copy construction
        Condition(const Condition &);

        //prevent copy assignment
        Condition & operator=(const Condition &);

    public:
        Condition(const condition::Enum  TYPE           = condition::Good,
                  const bool             IS_MAGICAL     = false,
                  const stats::StatSet & STATS_MODIFIER = stats::StatSet(),
                  const stats::StatSet & STATS_TO_SET   = stats::StatSet::INVALID_STATSET_);

        ~Condition();

        inline virtual const std::string Name() const                   { return condition::Name(type_); }
        inline virtual condition::Enum Which() const                    { return type_; }
        inline virtual const std::string Desc() const                   { return condition::Desc(type_); }
        virtual const std::string ToString() const;
        virtual const std::string LongDesc() const;
        inline virtual const stats::StatSet StatsToModifyCopy() const   { return statsToModify_; }
        inline virtual const stats::StatSet StatsToSetCopy() const      { return statsToSet_; }
        inline virtual std::size_t Severity() const                     { return condition::Severity::ByCondition(type_); }
        inline virtual bool IsMagical() const                           { return isMagical_; }

        virtual void Change(CreaturePtrC_t);
        virtual void Undo(CreaturePtrC_t);

        friend bool operator<(const Condition & L, const Condition & R);
        friend bool operator==(const Condition & L, const Condition & R);

    private:
        condition::Enum type_;
        bool            isMagical_;
        stats::StatSet  statsToModify_;
        stats::StatSet  statsToSet_;
        stats::StatSet  statsToSetUndo_;

    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & type_;
            ar & isMagical_;
            ar & statsToModify_;
            ar & statsToSet_;
            ar & statsToSetUndo_;
        }
    };

    using ConditionSPtr_t = std::shared_ptr<Condition>;

    using ConditionSVec_t      = std::vector<ConditionSPtr_t>;
    using ConditionSVecIter_t  = ConditionSVec_t::iterator;
    using ConditionSVecCIter_t = ConditionSVec_t::const_iterator;

    bool operator<(const Condition & L, const Condition & R);

    bool operator==(const Condition & L, const Condition & R);

    inline bool operator!=(const Condition & L, const Condition & R) { return ! (L == R); }

}
}
#endif //APPBASE_HEROESPATH_CREATURE_CONDITION_INCLUDED
