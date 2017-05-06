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

        static const std::string ToString(const Spells::Enum E);//straight name without spaces
        static const std::string Name(const Spells::Enum E);//humanreadable with spaces
        static const std::string Desc(const Spells::Enum E);
    };

}
}


#endif //GAME_CREATURE_SPELL_SPELLENUM_HPP_INCLUDED
