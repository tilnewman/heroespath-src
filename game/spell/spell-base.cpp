//
// spell-base.cpp
//
#include "spell-base.hpp"

#include "utilz/strings.hpp"

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
        std::ostringstream ss;
        ss  << Name() << "  -" << DescDetails();
        return ss.str();
    }


    const std::string Spell::DescDetails() const
    {
        std::ostringstream ss;
        ss  << "A " << utilz::String::DecorateNumber(rank_) << " rank"
            << " " << SpellType::Name(type_) << " spell"
            << " " << SpellClass::ToString(class_, true)
            << " that costs " << manaCost_ << " mana.";

        return ss.str();
    }


    const std::string Spell::DescComplete() const
    {
        std::ostringstream ss;
        ss << Desc() << "  " << DescExtra() << " " << DescDetails();
        return ss.str();
    }

}
}
