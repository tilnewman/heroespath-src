#ifndef GAME_SPELL_SPELLTYPEENUM_HPP_INCLUDED
#define GAME_SPELL_SPELLTYPEENUM_HPP_INCLUDED
//
// spell-type-enum.hpp
//
#include <string>
#include <vector>


namespace game
{
namespace spell
{

    struct SpellType
    {
        enum Enum
        {
            Attack = 0,
            Heal,
            EffectItem,
            EnchantItemHelpful,
            EnchantItemHarmful,
            EffectCreature,
            EnchantCreatureHelpful,
            EnchantCreatureHarmful,
            Misc,
            Count
        };

        static const std::string ToString(const Enum);
        static const std::string Name(const Enum);
    };

    using SpellTypeVec_t = std::vector<SpellType::Enum>;

}
}

#endif //GAME_SPELL_SPELLTYPEENUM_HPP_INCLUDED
