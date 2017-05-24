//
// spell-base.cpp
//
#include "spell-base.hpp"

#include "utilz/strings.hpp"
#include "utilz/random.hpp"

#include <sstream>
#include <algorithm>


namespace game
{
namespace spell
{

    const std::string Spell::EFFECT_STR_SUCCESS_         { "" };
    const std::string Spell::EFFECT_STR_NOTHING_TO_DO_   { "Nothing to do." };
    const int         Spell::EFFECTS_ALL_CREATURES_COUNT_{ -1 };
    const std::string Spell::EFFECT_STR_IS_ALREADY_      { " is already " };
    const std::string Spell::EFFECT_STR_RESISTED_        { " resisted the spell" };


    Spell::Spell(const Spells::Enum     WHICH,
                 const SpellType::Enum  TYPE,
                 const Phase::Enum      VALID_PHASES,
                 const stats::Mana_t    MANA_COST,
                 const stats::Rank_t    RANK,
                 const TargetType::Enum TARGET_TYPE)
    :
        which_      (WHICH),
        rank_       (RANK),
        type_       (TYPE),
        validPhases_(VALID_PHASES),
        manaCost_   (MANA_COST),
        targetType_ (TARGET_TYPE)
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
            << " that can be cast during " << Phase::ToString(validPhases_, false)
            << ", targeting " << TargetType::Name(targetType_)
            << ", and costing " << manaCost_ << " mana.";

        return ss.str();
    }


    const std::string Spell::DescComplete() const
    {
        std::ostringstream ss;
        ss << Desc() << "  " << DescExtra() << " " << DescDetails();
        return ss.str();
    }


    int Spell::GenerateValue(const int FLOOR,
                             const int THE_RAND_MAX,
                             const int RANK,
                             const int RANK_MAX) const
    {
        int x{ FLOOR };

        if (RANK_MAX == 0)
        {
            x += RANK;
        }
        else
        {
            x += std::min(RANK, RANK_MAX);
        }

        x += utilz::random::Int(THE_RAND_MAX);

        return x;
    }

}
}
