#ifndef GAME_CREATURE_CONDITION_INCLUDED
#define GAME_CREATURE_CONDITION_INCLUDED
//
// condition.hpp
//  Code that places a Creature under a temporary condition.
//  Examples would be poisoned, unconcious, etc.
//
#include "utilz/boost-serialize-includes.hpp"

#include "game/creature/condition-enum.hpp"
#include "game/stats/stat-set.hpp"

#include <memory>
#include <string>
#include <vector>


namespace game
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
        Condition(const Conditions::Enum  TYPE           = Conditions::Good,
                  const bool              IS_MAGICAL     = false,
                  const stats::StatSet &  STATS_MODIFIER = stats::StatSet(),
                  const stats::StatSet &  STATS_TO_SET   = stats::StatSet::INVALID_STATSET_);

        virtual ~Condition();

        inline const std::string Name() const                   { return Conditions::Name(type_); }
        inline Conditions::Enum Which() const                   { return type_; }
        inline const std::string Desc() const                   { return Conditions::Desc(type_); }
        const std::string ToString() const;
        const std::string LongDesc() const;
        inline const stats::StatSet StatsToModifyCopy() const   { return statsToModify_; }
        inline const stats::StatSet StatsToSetCopy() const      { return statsToSet_; }
        inline std::size_t Severity() const                     { return condition::Severity::Get(type_); }
        inline bool IsMagical() const                           { return isMagical_; }

        virtual const ConditionEnumVec_t InitialChange(CreaturePtrC_t);
        virtual const ConditionEnumVec_t PerTurnChange(CreaturePtrC_t);
        virtual const ConditionEnumVec_t FinalUndo(CreaturePtrC_t);

        friend bool operator<(const Condition & L, const Condition & R);
        friend bool operator==(const Condition & L, const Condition & R);

    private:
        Conditions::Enum type_;
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
#endif //GAME_CREATURE_CONDITION_INCLUDED
