#ifndef HEROESPATH_CREATURE_SPELL_SPELLENUM_HPP_INCLUDED
#define HEROESPATH_CREATURE_SPELL_SPELLENUM_HPP_INCLUDED
//
// spell-enum.hpp
//
#include <string>


namespace heroespath
{
namespace spell
{

    struct Spells
    {
        enum Enum
        {
            Sparks = 0,
            Count
        };

        static const std::string ToString(const Spells::Enum E);//straight name without spaces
        static const std::string Name(const Spells::Enum E);//humanreadable with spaces
        static const std::string Desc(const Spells::Enum E);
    };

}
}


#endif //HEROESPATH_CREATURE_SPELL_SPELLENUM_HPP_INCLUDED
