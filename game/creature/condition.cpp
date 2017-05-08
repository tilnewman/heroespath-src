//
// condition.hpp
//
#include "condition.hpp"

#include "utilz/assertlogandthrow.hpp"
#include "game/creature/creature.hpp"

#include <sstream>
#include <tuple>


namespace game
{
namespace creature
{

    Condition::Condition(const Conditions::Enum     TYPE,
                         const bool                 IS_MAGICAL,
                         const stats::StatMultSet & STAT_MULT_SET)
    :
        type_       (TYPE),
        isMagical_  (IS_MAGICAL),
        statMultSet_(STAT_MULT_SET)
    {}


    Condition::~Condition()
    {}


    const std::string Condition::ToString() const
    {
        std::ostringstream ss;
        ss << Conditions::Name(type_);

        if (isMagical_)
        {
            ss << " (magical)";
        }

        ss  << ".  " << Conditions::Desc(type_);
        ss << "  Stat Mults: " << statMultSet_.ToString(true);

        return ss.str();
    }


    const std::string Condition::LongDesc() const
    {
        std::ostringstream ss;
        ss << Conditions::Desc(type_);
        ss << "  Stat Multipliers: " << statMultSet_.ToString(true);
        if (Conditions::Good != type_)
        {
            if (isMagical_)
            {
                ss << " (a magical condition), ";
            }
            else
            {
                ss << ", ";
            }
        }

        ss << ".";
        return ss.str();
    }


    bool operator<(const Condition & L, const Condition & R)
    {
        return std::tie(L.type_,
                        L.isMagical_,
                        L.statMultSet_)
               <
               std::tie(R.type_,
                        R.isMagical_,
                        R.statMultSet_);
    }


    bool operator==(const Condition & L, const Condition & R)
    {
        return std::tie(L.type_,
                        L.isMagical_,
                        L.statMultSet_)
               ==
               std::tie(R.type_,
                        R.isMagical_,
                        R.statMultSet_);
    }


    const ConditionEnumVec_t Condition::InitialChange(CreaturePtrC_t creaturePtrC)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((creaturePtrC != nullptr), "Condition::InitialChange(creaturePtr==nullptr)  type=\"" << Conditions::Name(type_) << "\", was given a null creaturePtr.");
        return ConditionEnumVec_t();
    }


    const ConditionEnumVec_t Condition::PerTurnChange(CreaturePtrC_t creaturePtrC)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((creaturePtrC != nullptr), "Condition::PerTurnChange(creaturePtr==nullptr)  type=\"" << Conditions::Name(type_) << "\", was given a null creaturePtr.");
        return ConditionEnumVec_t();
    }


    const ConditionEnumVec_t Condition::FinalUndo(CreaturePtrC_t creaturePtrC)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((creaturePtrC != nullptr), "Condition::FinalUndo(creaturePtr==nullptr)  type=\"" << Conditions::Name(type_) << "\", was given a null creaturePtr.");
        return ConditionEnumVec_t();
    }

}
}
