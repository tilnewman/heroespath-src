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

    struct SpellClass
    {
        enum Enum : unsigned int
        {
            Combat          = 1 << 0,
            Exploring       = 1 << 1,
            Conversation    = 1 << 2,
            Quest           = 1 << 3,
            PlayerOnly      = 1 << 4,
            NonPlayerOnly   = 1 << 5
        };

        static const std::string ToString(const Enum, const bool WILL_WRAP);
        static Enum FromString(const std::string &);
    };


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
