#ifndef GAME_CREATURE_SPELL_SPELLENUM_HPP_INCLUDED
#define GAME_CREATURE_SPELL_SPELLENUM_HPP_INCLUDED
//
// spell-enum.hpp
//
#include <string>


namespace game
{
namespace spell
{

    struct Spells
    {
        enum Enum
        {
            Sparks = 0,
            Bandage,
            Sleep,
            Awaken,
            Trip,
            Lift,
            Daze,
            Frighten,
            ClearMind,
            Poison,
            Antidote,//cures poison
            Count
        };

        //straight name without spaces
        static const std::string ToString(const Spells::Enum);

        //human readable with spaces
        static const std::string Name(const Spells::Enum);
        static const std::string ShortDesc(const Spells::Enum);
        static const std::string ExtraDesc(const Spells::Enum);
    };

}
}


#endif //GAME_CREATURE_SPELL_SPELLENUM_HPP_INCLUDED
