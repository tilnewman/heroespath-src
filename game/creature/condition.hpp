#ifndef GAME_CREATURE_CONDITION_INCLUDED
#define GAME_CREATURE_CONDITION_INCLUDED
//
// condition.hpp
//  Code that places a Creature under a temporary condition.
//  Examples would be poisoned, unconcious, etc.
//
#include "game/creature/condition-enum.hpp"
#include "game/stats/stat-set.hpp"
#include "game/stats/stat-mult-set.hpp"

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
        Condition(const Condition &) =delete;

        //prevent copy assignment
        Condition & operator=(const Condition &) =delete;

    public:
        explicit Condition(const Conditions::Enum     TYPE          = Conditions::Good,
                           const bool                 IS_MAGICAL    = false,
                           const stats::StatMultSet & STAT_MULT_SET = stats::StatMultSet());

        virtual ~Condition();

        inline const std::string Name() const               { return Conditions::Name(type_); }
        inline Conditions::Enum Which() const               { return type_; }
        inline const std::string Desc() const               { return Conditions::Desc(type_); }
        const std::string ToString() const;
        const std::string LongDesc() const;
        inline std::size_t Severity() const                 { return condition::Severity::Get(type_); }
        inline bool IsMagical() const                       { return isMagical_; }
        inline const stats::StatMultSet StatMult() const    { return statMultSet_; }

        virtual const ConditionEnumVec_t InitialChange(CreaturePtrC_t);
        virtual const ConditionEnumVec_t PerTurnChange(CreaturePtrC_t);
        virtual const ConditionEnumVec_t FinalUndo(CreaturePtrC_t);

        friend bool operator<(const Condition & L, const Condition & R);
        friend bool operator==(const Condition & L, const Condition & R);

    private:
        Conditions::Enum    type_;
        bool                isMagical_;
        stats::StatMultSet  statMultSet_;
    };


    using ConditionPtr_t = Condition *;
    using ConditionPVec_t = std::vector<ConditionPtr_t>;

    using ConditionSPtr_t = std::shared_ptr<Condition>;
    using ConditionSVec_t = std::vector<ConditionSPtr_t>;


    bool operator<(const Condition & L, const Condition & R);

    bool operator==(const Condition & L, const Condition & R);

    inline bool operator!=(const Condition & L, const Condition & R)
    {
        return ! (L == R);
    }

}
}
#endif //GAME_CREATURE_CONDITION_INCLUDED
