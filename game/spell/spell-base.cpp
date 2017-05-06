//
// spell-base.cpp
//
#include "spell-base.hpp"
#include <sstream>


namespace game
{
namespace spell
{

    Spell::Spell(const Spells::Enum     WHICH,
                 const SpellType::Enum  TYPE,
                 const SpellClass::Enum CLASS,
                 const stats::Mana_t    MANA_COST,
                 const stats::Rank_t    RANK)
    :
        which_   (WHICH),
        rank_    (RANK),
        type_    (TYPE),
        class_   (CLASS),
        manaCost_(MANA_COST)
    {}


    Spell::~Spell()
    {}


    const std::string Spell::ToString() const
    {
        return Spells::Name(which_);
    }


    const std::string Spell::LongDesc() const
    {
        std::ostringstream ss;
        ss << Spells::Desc(which_) << ", a " << SpellType::Name(type_)
           << " " << SpellClass::ToString(class_, true)
           << " that costs " << manaCost_
           << " and requires a rank of " << rank_
           << ".";
        return ss.str();
    }

}
}