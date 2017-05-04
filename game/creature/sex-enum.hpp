#ifndef GAME_CREATURE_SEXENUM_INCLUDED
#define GAME_CREATURE_SEXENUM_INCLUDED
//
// sex-enum.hpp
//  An enumeration of all sexes.
//
#include <string>


namespace game
{
namespace creature
{

    struct sex
    {
        enum Enum
        {
            Unknown = 0, //some creatures will be too bizarre to be either male or female
            Male,
            Female,
            Count
        };

        static const std::string ToString(const sex::Enum E);

        static bool IsValid(const sex::Enum E);

        static const std::string HeSheIt(const sex::Enum E, const bool WILL_CAPITALIZE);

        static const std::string HisHerIts(const sex::Enum E,
                                           const bool      WILL_CAPITALIZE,
                                           const bool      WILL_POSSESSIVE_HER);

        static const std::string HimHerIt(const sex::Enum E, const bool WILL_CAPITALIZE);
    };

}
}
#endif //GAME_CREATURE_SEXENUM_INCLUDED
