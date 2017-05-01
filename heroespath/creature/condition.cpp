//
// condition.hpp
//
#include "condition.hpp"

#include "utilz/assertlogandthrow.hpp"
#include "heroespath/creature/creature.hpp"

#include <sstream>
#include <tuple>


namespace heroespath
{
namespace creature
{

    Condition::Condition(const condition::Enum  TYPE,
                         const bool             IS_MAGICAL,
                         const stats::StatSet & STATS_TO_MODIFY,
                         const stats::StatSet & STATS_TO_SET)
    :
        type_          (TYPE),
        isMagical_     (IS_MAGICAL),
        statsToModify_ (STATS_TO_MODIFY),
        statsToSet_    (STATS_TO_SET),
        statsToSetUndo_()
    {}


    Condition::~Condition()
    {}


    const std::string Condition::ToString() const
    {
        std::ostringstream ss;
        ss << condition::Name(type_);

        if (isMagical_)
        {
            ss << " (magical)";
        }

        ss  << ".  " << condition::Desc(type_);

        const std::string STATS_STR(statsToModify_.ToStringNormal(true, true, true));
        if (STATS_STR.empty() == false)
        {
            ss << "  Stats modified:" << STATS_STR;
        }

        if (statsToSet_ != stats::StatSet::INVALID_STATSET_)
        {
            ss << "  Stats set:" << statsToSet_.ToStringNormal(true, true, false, false, true);
        }

        return ss.str();
    }


    const std::string Condition::LongDesc() const
    {
        std::ostringstream ss;
        ss << condition::Desc(type_);

        if (condition::Good != type_)
        {
            if (isMagical_)
            {
                ss << " (a magical condition), ";
            }
            else
            {
                ss << ", ";
            }

            const std::string STATS_STR(statsToModify_.ToStringNormal(false, false, true));
            if (STATS_STR.empty() == false)
            {
                ss << "that modifies the following stats:  " << STATS_STR << ", ";
            }

            if (statsToSet_ == stats::StatSet::INVALID_STATSET_)
            {
                ss << "that sets the following stats:  " << statsToSet_.ToStringNormal(false, false, false, false, true);
            }
        }

        ss << ".";
        return ss.str();
    }


    bool operator<(const Condition & L, const Condition & R)
    {
        return std::tie(L.type_,
                        L.isMagical_,
                        L.statsToModify_,
                        L.statsToSet_,
                        L.statsToSetUndo_)
               <
               std::tie(R.type_,
                        R.isMagical_,
                        R.statsToModify_,
                        R.statsToSet_,
                        R.statsToSetUndo_);
    }


    bool operator==(const Condition & L, const Condition & R)
    {
        return std::tie(L.type_,
                        L.isMagical_,
                        L.statsToModify_,
                        L.statsToSet_,
                        L.statsToSetUndo_)
               ==
               std::tie(R.type_,
                        R.isMagical_,
                        R.statsToModify_,
                        R.statsToSet_,
                        R.statsToSetUndo_);
    }


    void Condition::Change(CreaturePtrC_t creaturePtrC)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((creaturePtrC != nullptr), "Condition::Change(creaturePtr==nullptr)  type=\"" << condition::Name(type_) << "\", was given a null creaturePtr.");

        //Conditions are temporary so they effect the current stat value only, not the normal
        creaturePtrC->Stats().ModifyCurrent(statsToModify_);
        statsToSetUndo_ = creaturePtrC->Stats().ModifyCurrentToValid(statsToSet_);
    }


    void Condition::Undo(CreaturePtrC_t creaturePtrC)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((creaturePtrC != nullptr), "Condition::Undo(creaturePtr==nullptr)  type=\"" << condition::Name(type_) << "\", was given a null creaturePtr.");

        //Conditions are temporary so they effect the current stat value only, not the normal
        creaturePtrC->Stats().ModifyCurrent(statsToModify_.CreateInvertCopy());
        creaturePtrC->Stats().ModifyCurrent(statsToSetUndo_);
    }

}
}
