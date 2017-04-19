#ifndef HEROESPATH_SPELL_SPELLTYPEENUM_HPP_INCLUDED
#define HEROESPATH_SPELL_SPELLTYPEENUM_HPP_INCLUDED
//
// spell-type-enum.hpp
//
#include <string>
#include <vector>


namespace heroespath
{
namespace spell
{

    struct SpellType
    {
        enum Enum
        {
            Attack = 0,
            Heal,
            EnchantItemHelpful,
            EnchantItemHarmful,
            EnchantCreatureHelpful,
            EnchantCreatureHarmful,
            MiscCombat,
            MiscNonCombat, //a misc spell that makes no sense to cast during combat, such as creating a light etc.
            Count
        };

        static const std::string ToString(const Enum);
    };

    using SpellTypeVec_t = std::vector<SpellType::Enum>;

}
}

#endif //HEROESPATH_SPELL_SPELLTYPEENUM_HPP_INCLUDED
