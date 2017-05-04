//
// spell-base.cpp
//
#include "spell-base.hpp"
#include <sstream>


namespace game
{
namespace spell
{

    Spell::Spell(const Spells::Enum    WHICH,
                 const SpellType::Enum TYPE,
                 const stats::Rank_t   RANK)
    :
        which_(WHICH),
        rank_ (RANK),
        type_ (TYPE)
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
        ss << Spells::Desc(which_) << ", a " << SpellType::ToString(type_) << " spell requiring rank " << rank_ << ".";
        return ss.str();
    }

}
}